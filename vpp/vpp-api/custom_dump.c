/*
 *------------------------------------------------------------------
 * custom_dump.c - pretty-print API messages for replay
 *
 * Copyright (c) 2014 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *------------------------------------------------------------------
 */

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/unix/tuntap.h>
#include <vnet/mpls-gre/mpls.h>
#include <vnet/dhcp/proxy.h>
#include <vnet/dhcpv6/proxy.h>
#include <vnet/l2tp/l2tp.h>
#include <vnet/l2/l2_input.h>
#include <vnet/sr/sr_packet.h>
#include <vnet/vxlan-gpe/vxlan_gpe.h>
#include <vnet/classify/policer_classify.h>
#include <vnet/policer/xlate.h>
#include <vnet/policer/policer.h>
#include <vlib/vlib.h>
#include <vlib/unix/unix.h>
#include <vlibapi/api.h>
#include <vlibmemory/api.h>

#include <stats/stats.h>
#include <oam/oam.h>

#include <vnet/ethernet/ethernet.h>
#include <vnet/l2/l2_vtr.h>

#include <vpp-api/vpe_msg_enum.h>

#define vl_typedefs		/* define message structures */
#include <vpp-api/vpe_all_api_h.h>
#undef vl_typedefs

#define vl_endianfun		/* define message structures */
#include <vpp-api/vpe_all_api_h.h>
#undef vl_endianfun

#define vl_print(handle, ...) vlib_cli_output (handle, __VA_ARGS__)

#define FINISH                                  \
    vec_add1 (s, 0);                            \
    vl_print (handle, (char *)s);               \
    vec_free (s);                               \
    return handle;


static void *vl_api_create_loopback_t_print
  (vl_api_create_loopback_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: create_loopback ");
  s = format (s, "mac %U ", format_ethernet_address, &mp->mac_address);

  FINISH;
}

static void *vl_api_delete_loopback_t_print
  (vl_api_delete_loopback_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: delete_loopback ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_sw_interface_set_flags_t_print
  (vl_api_sw_interface_set_flags_t * mp, void *handle)
{
  u8 *s;
  s = format (0, "SCRIPT: sw_interface_set_flags ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->admin_up_down)
    s = format (s, "admin-up ");
  else
    s = format (s, "admin-down ");

  if (mp->link_up_down)
    s = format (s, "link-up");
  else
    s = format (s, "link-down");

  FINISH;
}

static void *vl_api_sw_interface_add_del_address_t_print
  (vl_api_sw_interface_add_del_address_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_add_del_address ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->is_ipv6)
    s = format (s, "%U/%d ", format_ip6_address,
		(ip6_address_t *) mp->address, mp->address_length);
  else
    s = format (s, "%U/%d ", format_ip4_address,
		(ip4_address_t *) mp->address, mp->address_length);

  if (mp->is_add == 0)
    s = format (s, "del ");
  if (mp->del_all)
    s = format (s, "del-all ");

  FINISH;
}

static void *vl_api_sw_interface_set_table_t_print
  (vl_api_sw_interface_set_table_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_set_table ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->vrf_id)
    s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  if (mp->is_ipv6)
    s = format (s, "ipv6 ");

  FINISH;
}

static void *vl_api_sw_interface_set_vpath_t_print
  (vl_api_sw_interface_set_vpath_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_set_vpath ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->enable)
    s = format (s, "vPath enable ");
  else
    s = format (s, "vPath disable ");

  FINISH;
}

static void *vl_api_sw_interface_set_l2_xconnect_t_print
  (vl_api_sw_interface_set_l2_xconnect_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_set_l2_xconnect ");

  s = format (s, "sw_if_index %d ", ntohl (mp->rx_sw_if_index));

  if (mp->enable)
    {
      s = format (s, "tx_sw_if_index %d ", ntohl (mp->tx_sw_if_index));
    }
  else
    s = format (s, "delete ");

  FINISH;
}

static void *vl_api_sw_interface_set_l2_bridge_t_print
  (vl_api_sw_interface_set_l2_bridge_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_set_l2_bridge ");

  s = format (s, "sw_if_index %d ", ntohl (mp->rx_sw_if_index));

  if (mp->enable)
    {
      s = format (s, "bd_id %d shg %d %senable ", ntohl (mp->bd_id),
		  mp->shg, ((mp->bvi) ? "bvi " : " "));
    }
  else
    s = format (s, "disable ");

  FINISH;
}

static void *vl_api_bridge_domain_add_del_t_print
  (vl_api_bridge_domain_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: bridge_domain_add_del ");

  s = format (s, "bd_id %d ", ntohl (mp->bd_id));

  if (mp->is_add)
    {
      s = format (s, "flood %d uu-flood %d forward %d learn %d arp-term %d",
		  mp->flood, mp->uu_flood, mp->forward, mp->learn,
		  mp->arp_term);
    }
  else
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_bridge_domain_dump_t_print
  (vl_api_bridge_domain_dump_t * mp, void *handle)
{
  u8 *s;
  u32 bd_id = ntohl (mp->bd_id);

  s = format (0, "SCRIPT: bridge_domain_dump ");

  if (bd_id != ~0)
    s = format (s, "bd_id %d ", bd_id);

  FINISH;
}

static void *vl_api_l2fib_add_del_t_print
  (vl_api_l2fib_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2fib_add_del ");

  s = format (s, "mac %U ", format_ethernet_address, &mp->mac);

  s = format (s, "bd_id %d ", ntohl (mp->bd_id));


  if (mp->is_add)
    {
      s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
      if (mp->static_mac)
	s = format (s, "%s", "static ");
      if (mp->filter_mac)
	s = format (s, "%s", "filter ");
      if (mp->bvi_mac)
	s = format (s, "%s", "bvi ");
    }
  else
    {
      s = format (s, "del ");
    }

  FINISH;
}

static void *
vl_api_l2_flags_t_print (vl_api_l2_flags_t * mp, void *handle)
{
  u8 *s;
  u32 flags = ntohl (mp->feature_bitmap);

  s = format (0, "SCRIPT: l2_flags ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

#define _(a,b) \
    if (flags & L2INPUT_FEAT_ ## a) s = format (s, #a " ");
  foreach_l2input_feat;
#undef _

  FINISH;
}

static void *vl_api_bridge_flags_t_print
  (vl_api_bridge_flags_t * mp, void *handle)
{
  u8 *s;
  u32 flags = ntohl (mp->feature_bitmap);

  s = format (0, "SCRIPT: bridge_flags ");

  s = format (s, "bd_id %d ", ntohl (mp->bd_id));

  if (flags & L2_LEARN)
    s = format (s, "learn ");
  if (flags & L2_FWD)
    s = format (s, "forward ");
  if (flags & L2_FLOOD)
    s = format (s, "flood ");
  if (flags & L2_UU_FLOOD)
    s = format (s, "uu-flood ");
  if (flags & L2_ARP_TERM)
    s = format (s, "arp-term ");

  if (mp->is_set == 0)
    s = format (s, "clear ");

  FINISH;
}

static void *vl_api_bd_ip_mac_add_del_t_print
  (vl_api_bd_ip_mac_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: bd_ip_mac_add_del ");
  s = format (s, "bd_id %d ", ntohl (mp->bd_id));

  if (mp->is_ipv6)
    s = format (s, "%U ", format_ip6_address,
		(ip6_address_t *) mp->ip_address);
  else
    s = format (s, "%U ", format_ip4_address,
		(ip4_address_t *) mp->ip_address);

  s = format (s, "%U ", format_ethernet_address, mp->mac_address);
  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_tap_connect_t_print
  (vl_api_tap_connect_t * mp, void *handle)
{
  u8 *s;
  u8 null_mac[6];

  memset (null_mac, 0, sizeof (null_mac));

  s = format (0, "SCRIPT: tap_connect ");
  s = format (s, "tapname %s ", mp->tap_name);
  if (mp->use_random_mac)
    s = format (s, "random-mac ");

  if (memcmp (mp->mac_address, null_mac, 6))
    s = format (s, "mac %U ", format_ethernet_address, mp->mac_address);

  FINISH;
}

static void *vl_api_tap_modify_t_print
  (vl_api_tap_modify_t * mp, void *handle)
{
  u8 *s;
  u8 null_mac[6];

  memset (null_mac, 0, sizeof (null_mac));

  s = format (0, "SCRIPT: tap_modify ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "tapname %s ", mp->tap_name);
  if (mp->use_random_mac)
    s = format (s, "random-mac ");

  if (memcmp (mp->mac_address, null_mac, 6))
    s = format (s, "mac %U ", format_ethernet_address, mp->mac_address);

  FINISH;
}

static void *vl_api_tap_delete_t_print
  (vl_api_tap_delete_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: tap_delete ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_sw_interface_tap_dump_t_print
  (vl_api_sw_interface_tap_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_tap_dump ");

  FINISH;
}


static void *vl_api_ip_add_del_route_t_print
  (vl_api_ip_add_del_route_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ip_add_del_route ");
  if (mp->is_add == 0)
    s = format (s, "del ");

  if (mp->next_hop_sw_if_index)
    s = format (s, "sw_if_index %d ", ntohl (mp->next_hop_sw_if_index));

  if (mp->is_ipv6)
    s = format (s, "%U/%d ", format_ip6_address, mp->dst_address,
		mp->dst_address_length);
  else
    s = format (s, "%U/%d ", format_ip4_address, mp->dst_address,
		mp->dst_address_length);
  if (mp->is_local)
    s = format (s, "local ");
  else if (mp->is_drop)
    s = format (s, "drop ");
  else if (mp->is_classify)
    s = format (s, "classify %d", ntohl (mp->classify_table_index));
  else
    {
      if (mp->is_ipv6)
	s = format (s, "via %U ", format_ip6_address, mp->next_hop_address);
      else
	s = format (s, "via %U ", format_ip4_address, mp->next_hop_address);
    }

  if (mp->vrf_id != 0)
    s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  if (mp->create_vrf_if_needed)
    s = format (s, "create-vrf ");

  if (mp->resolve_attempts != 0)
    s = format (s, "resolve-attempts %d ", ntohl (mp->resolve_attempts));

  if (mp->next_hop_weight != 1)
    s = format (s, "weight %d ", mp->next_hop_weight);

  if (mp->not_last)
    s = format (s, "not-last ");

  if (mp->is_multipath)
    s = format (s, "multipath ");

  if (mp->is_multipath)
    s = format (s, "multipath ");

  if (mp->lookup_in_vrf)
    s = format (s, "lookup-in-vrf %d ", ntohl (mp->lookup_in_vrf));

  FINISH;
}

static void *vl_api_proxy_arp_add_del_t_print
  (vl_api_proxy_arp_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: proxy_arp_add_del ");

  s = format (s, "%U - %U ", format_ip4_address, mp->low_address,
	      format_ip4_address, mp->hi_address);

  if (mp->vrf_id)
    s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_proxy_arp_intfc_enable_disable_t_print
  (vl_api_proxy_arp_intfc_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: proxy_arp_intfc_enable_disable ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "enable %d ", mp->enable_disable);

  FINISH;
}

static void *vl_api_mpls_add_del_decap_t_print
  (vl_api_mpls_add_del_decap_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_add_del_decap ");

  s = format (s, "rx_vrf_id %d ", ntohl (mp->rx_vrf_id));

  s = format (s, "tx_vrf_id %d ", ntohl (mp->tx_vrf_id));

  s = format (s, "label %d ", ntohl (mp->label));

  s = format (s, "next-index %d ", ntohl (mp->next_index));

  if (mp->s_bit == 0)
    s = format (s, "s-bit-clear ");

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_mpls_add_del_encap_t_print
  (vl_api_mpls_add_del_encap_t * mp, void *handle)
{
  u8 *s;
  int i;

  s = format (0, "SCRIPT: mpls_add_del_encap ");

  s = format (s, "vrf_id %d ", ntohl (mp->vrf_id));

  s = format (s, "dst %U ", format_ip4_address, mp->dst_address);

  for (i = 0; i < mp->nlabels; i++)
    s = format (s, "label %d ", ntohl (mp->labels[i]));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_mpls_gre_add_del_tunnel_t_print
  (vl_api_mpls_gre_add_del_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_gre_add_del_tunnel ");

  s = format (s, "src %U ", format_ip4_address, mp->src_address);

  s = format (s, "dst %U ", format_ip4_address, mp->dst_address);

  s = format (s, "adj %U/%d ", format_ip4_address,
	      (ip4_address_t *) mp->intfc_address, mp->intfc_address_length);

  s = format (s, "inner-vrf_id %d ", ntohl (mp->inner_vrf_id));

  s = format (s, "outer-vrf_id %d ", ntohl (mp->outer_vrf_id));

  if (mp->is_add == 0)
    s = format (s, "del ");

  if (mp->l2_only)
    s = format (s, "l2-only ");

  FINISH;
}

static void *vl_api_mpls_ethernet_add_del_tunnel_t_print
  (vl_api_mpls_ethernet_add_del_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_ethernet_add_del_tunnel ");

  s = format (s, "tx_sw_if_index %d ", ntohl (mp->tx_sw_if_index));

  s = format (s, "dst %U", format_ethernet_address, mp->dst_mac_address);

  s = format (s, "adj %U/%d ", format_ip4_address,
	      (ip4_address_t *) mp->adj_address, mp->adj_address_length);

  s = format (s, "vrf_id %d ", ntohl (mp->vrf_id));

  if (mp->l2_only)
    s = format (s, "l2-only ");

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_mpls_ethernet_add_del_tunnel_2_t_print
  (vl_api_mpls_ethernet_add_del_tunnel_2_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_ethernet_add_del_tunnel_2 ");

  s = format (s, "adj %U/%d ", format_ip4_address,
	      (ip4_address_t *) mp->adj_address, mp->adj_address_length);

  s = format (s, "next-hop %U ", format_ip4_address,
	      (ip4_address_t *) mp->next_hop_ip4_address_in_outer_vrf);

  s = format (s, "inner_vrf_id %d ", ntohl (mp->inner_vrf_id));

  s = format (s, "outer_vrf_id %d ", ntohl (mp->outer_vrf_id));

  s = format (s, "resolve-if-needed %d ", mp->resolve_if_needed);

  s = format (s, "resolve-attempts %d ", ntohl (mp->resolve_attempts));

  if (mp->l2_only)
    s = format (s, "l2-only ");

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_sw_interface_set_unnumbered_t_print
  (vl_api_sw_interface_set_unnumbered_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_set_unnumbered ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "unnum_if_index %d ", ntohl (mp->unnumbered_sw_if_index));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_ip_neighbor_add_del_t_print
  (vl_api_ip_neighbor_add_del_t * mp, void *handle)
{
  u8 *s;
  u8 null_mac[6];

  memset (null_mac, 0, sizeof (null_mac));

  s = format (0, "SCRIPT: ip_neighbor_add_del ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->is_static)
    s = format (s, "is_static ");

  s = format (s, "vrf_id %d ", ntohl (mp->vrf_id));

  if (memcmp (mp->mac_address, null_mac, 6))
    s = format (s, "mac %U ", format_ethernet_address, mp->mac_address);

  if (mp->is_ipv6)
    s =
      format (s, "dst %U ", format_ip6_address,
	      (ip6_address_t *) mp->dst_address);
  else
    s =
      format (s, "dst %U ", format_ip4_address,
	      (ip4_address_t *) mp->dst_address);

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *
vl_api_reset_vrf_t_print (vl_api_reset_vrf_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: reset_vrf ");

  if (mp->vrf_id)
    s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  if (mp->is_ipv6 != 0)
    s = format (s, "ipv6 ");

  FINISH;
}

static void *vl_api_create_vlan_subif_t_print
  (vl_api_create_vlan_subif_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: create_vlan_subif ");

  if (mp->sw_if_index)
    s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->vlan_id)
    s = format (s, "vlan_id %d ", ntohl (mp->vlan_id));

  FINISH;
}

#define foreach_create_subif_bit                \
_(no_tags)                                      \
_(one_tag)                                      \
_(two_tags)                                     \
_(dot1ad)                                       \
_(exact_match)                                  \
_(default_sub)                                  \
_(outer_vlan_id_any)                            \
_(inner_vlan_id_any)

static void *vl_api_create_subif_t_print
  (vl_api_create_subif_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: create_subif ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "sub_id %d ", ntohl (mp->sub_id));

  if (mp->outer_vlan_id)
    s = format (s, "outer_vlan_id %d ", ntohs (mp->outer_vlan_id));

  if (mp->inner_vlan_id)
    s = format (s, "inner_vlan_id %d ", ntohs (mp->inner_vlan_id));

#define _(a) if (mp->a) s = format (s, "%s ", #a);
  foreach_create_subif_bit;
#undef _

  FINISH;
}

static void *vl_api_delete_subif_t_print
  (vl_api_delete_subif_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: delete_subif ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_oam_add_del_t_print
  (vl_api_oam_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: oam_add_del ");

  if (mp->vrf_id)
    s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  s = format (s, "src %U ", format_ip4_address, mp->src_address);

  s = format (s, "dst %U ", format_ip4_address, mp->dst_address);

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *
vl_api_reset_fib_t_print (vl_api_reset_fib_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: reset_fib ");

  if (mp->vrf_id)
    s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  if (mp->is_ipv6 != 0)
    s = format (s, "ipv6 ");

  FINISH;
}

static void *vl_api_dhcp_proxy_config_t_print
  (vl_api_dhcp_proxy_config_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: dhcp_proxy_config ");

  s = format (s, "vrf_id %d ", ntohl (mp->vrf_id));

  if (mp->is_ipv6)
    {
      s = format (s, "svr %U ", format_ip6_address,
		  (ip6_address_t *) mp->dhcp_server);
      s = format (s, "src %U ", format_ip6_address,
		  (ip6_address_t *) mp->dhcp_src_address);
    }
  else
    {
      s = format (s, "svr %U ", format_ip4_address,
		  (ip4_address_t *) mp->dhcp_server);
      s = format (s, "src %U ", format_ip4_address,
		  (ip4_address_t *) mp->dhcp_src_address);
    }
  if (mp->is_add == 0)
    s = format (s, "del ");

  s = format (s, "insert-cid %d ", mp->insert_circuit_id);

  FINISH;
}

static void *vl_api_dhcp_proxy_config_2_t_print
  (vl_api_dhcp_proxy_config_2_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: dhcp_proxy_config_2 ");

  s = format (s, "rx_vrf_id %d ", ntohl (mp->rx_vrf_id));
  s = format (s, "server_vrf_id %d ", ntohl (mp->server_vrf_id));

  if (mp->is_ipv6)
    {
      s = format (s, "svr %U ", format_ip6_address,
		  (ip6_address_t *) mp->dhcp_server);
      s = format (s, "src %U ", format_ip6_address,
		  (ip6_address_t *) mp->dhcp_src_address);
    }
  else
    {
      s = format (s, "svr %U ", format_ip4_address,
		  (ip4_address_t *) mp->dhcp_server);
      s = format (s, "src %U ", format_ip4_address,
		  (ip4_address_t *) mp->dhcp_src_address);
    }
  if (mp->is_add == 0)
    s = format (s, "del ");

  s = format (s, "insert-cid %d ", mp->insert_circuit_id);

  FINISH;
}

static void *vl_api_dhcp_proxy_set_vss_t_print
  (vl_api_dhcp_proxy_set_vss_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: dhcp_proxy_set_vss ");

  s = format (s, "tbl_id %d ", ntohl (mp->tbl_id));

  s = format (s, "fib_id %d ", ntohl (mp->fib_id));

  s = format (s, "oui %d ", ntohl (mp->oui));

  if (mp->is_ipv6 != 0)
    s = format (s, "ipv6 ");

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_dhcp_client_config_t_print
  (vl_api_dhcp_client_config_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: dhcp_client_config ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "hostname %s ", mp->hostname);

  s = format (s, "want_dhcp_event %d ", mp->want_dhcp_event);

  s = format (s, "pid %d ", mp->pid);

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}


static void *vl_api_set_ip_flow_hash_t_print
  (vl_api_set_ip_flow_hash_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: set_ip_flow_hash ");

  s = format (s, "vrf_id %d ", ntohl (mp->vrf_id));

  if (mp->src)
    s = format (s, "src ");

  if (mp->dst)
    s = format (s, "dst ");

  if (mp->sport)
    s = format (s, "sport ");

  if (mp->dport)
    s = format (s, "dport ");

  if (mp->proto)
    s = format (s, "proto ");

  if (mp->reverse)
    s = format (s, "reverse ");

  if (mp->is_ipv6 != 0)
    s = format (s, "ipv6 ");

  FINISH;
}

static void *vl_api_sw_interface_ip6_set_link_local_address_t_print
  (vl_api_sw_interface_ip6_set_link_local_address_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_ip6_set_link_local_address ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "%U/%d ", format_ip6_address, mp->address,
	      mp->address_length);

  FINISH;
}

static void *vl_api_sw_interface_ip6nd_ra_prefix_t_print
  (vl_api_sw_interface_ip6nd_ra_prefix_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_ip6nd_ra_prefix ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "%U/%d ", format_ip6_address, mp->address,
	      mp->address_length);

  s = format (s, "val_life %d ", ntohl (mp->val_lifetime));

  s = format (s, "pref_life %d ", ntohl (mp->pref_lifetime));

  if (mp->use_default)
    s = format (s, "def ");

  if (mp->no_advertise)
    s = format (s, "noadv ");

  if (mp->off_link)
    s = format (s, "offl ");

  if (mp->no_autoconfig)
    s = format (s, "noauto ");

  if (mp->no_onlink)
    s = format (s, "nolink ");

  if (mp->is_no)
    s = format (s, "isno ");

  FINISH;
}

static void *vl_api_sw_interface_ip6nd_ra_config_t_print
  (vl_api_sw_interface_ip6nd_ra_config_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_ip6nd_ra_config ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "maxint %d ", ntohl (mp->max_interval));

  s = format (s, "minint %d ", ntohl (mp->min_interval));

  s = format (s, "life %d ", ntohl (mp->lifetime));

  s = format (s, "count %d ", ntohl (mp->initial_count));

  s = format (s, "interval %d ", ntohl (mp->initial_interval));

  if (mp->suppress)
    s = format (s, "suppress ");

  if (mp->managed)
    s = format (s, "managed ");

  if (mp->other)
    s = format (s, "other ");

  if (mp->ll_option)
    s = format (s, "ll ");

  if (mp->send_unicast)
    s = format (s, "send ");

  if (mp->cease)
    s = format (s, "cease ");

  if (mp->is_no)
    s = format (s, "isno ");

  if (mp->default_router)
    s = format (s, "def ");

  FINISH;
}

static void *vl_api_set_arp_neighbor_limit_t_print
  (vl_api_set_arp_neighbor_limit_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: set_arp_neighbor_limit ");

  s = format (s, "arp_nbr_limit %d ", ntohl (mp->arp_neighbor_limit));

  if (mp->is_ipv6 != 0)
    s = format (s, "ipv6 ");

  FINISH;
}

static void *vl_api_l2_patch_add_del_t_print
  (vl_api_l2_patch_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2_patch_add_del ");

  s = format (s, "rx_sw_if_index %d ", ntohl (mp->rx_sw_if_index));

  s = format (s, "tx_sw_if_index %d ", ntohl (mp->tx_sw_if_index));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_sr_tunnel_add_del_t_print
  (vl_api_sr_tunnel_add_del_t * mp, void *handle)
{
  u8 *s;
  ip6_address_t *this_address;
  int i;
  u16 flags_host_byte_order;
  u8 pl_flag;

  s = format (0, "SCRIPT: sr_tunnel_add_del ");

  if (mp->name[0])
    s = format (s, "name %s ", mp->name);

  s = format (s, "src %U dst %U/%d ", format_ip6_address,
	      (ip6_address_t *) mp->src_address,
	      format_ip6_address,
	      (ip6_address_t *) mp->dst_address, mp->dst_mask_width);

  this_address = (ip6_address_t *) mp->segs_and_tags;
  for (i = 0; i < mp->n_segments; i++)
    {
      s = format (s, "next %U ", format_ip6_address, this_address);
      this_address++;
    }
  for (i = 0; i < mp->n_tags; i++)
    {
      s = format (s, "tag %U ", format_ip6_address, this_address);
      this_address++;
    }

  flags_host_byte_order = clib_net_to_host_u16 (mp->flags_net_byte_order);

  if (flags_host_byte_order & IP6_SR_HEADER_FLAG_CLEANUP)
    s = format (s, " clean ");

  if (flags_host_byte_order & IP6_SR_HEADER_FLAG_PROTECTED)
    s = format (s, "protected ");

  for (i = 1; i <= 4; i++)
    {
      pl_flag = ip6_sr_policy_list_flags (flags_host_byte_order, i);

      switch (pl_flag)
	{
	case IP6_SR_HEADER_FLAG_PL_ELT_NOT_PRESENT:
	  continue;

	case IP6_SR_HEADER_FLAG_PL_ELT_INGRESS_PE:
	  s = format (s, "InPE %d ", i);
	  break;

	case IP6_SR_HEADER_FLAG_PL_ELT_EGRESS_PE:
	  s = format (s, "EgPE %d ", i);
	  break;

	case IP6_SR_HEADER_FLAG_PL_ELT_ORIG_SRC_ADDR:
	  s = format (s, "OrgSrc %d ", i);
	  break;

	default:
	  clib_warning ("BUG: pl elt %d value %d", i, pl_flag);
	  break;
	}
    }

  if (mp->policy_name[0])
    s = format (s, "policy_name %s ", mp->policy_name);

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_sr_policy_add_del_t_print
  (vl_api_sr_policy_add_del_t * mp, void *handle)
{
  u8 *s;
  int i;

  s = format (0, "SCRIPT: sr_policy_add_del ");

  if (mp->name[0])
    s = format (s, "name %s ", mp->name);


  if (mp->tunnel_names[0])
    {
      // start deserializing tunnel_names
      int num_tunnels = mp->tunnel_names[0];	//number of tunnels
      u8 *deser_tun_names = mp->tunnel_names;
      deser_tun_names += 1;	//moving along

      u8 *tun_name = 0;
      int tun_name_len = 0;

      for (i = 0; i < num_tunnels; i++)
	{
	  tun_name_len = *deser_tun_names;
	  deser_tun_names += 1;
	  vec_resize (tun_name, tun_name_len);
	  memcpy (tun_name, deser_tun_names, tun_name_len);
	  s = format (s, "tunnel %s ", tun_name);
	  deser_tun_names += tun_name_len;
	  tun_name = 0;
	}
    }

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_sr_multicast_map_add_del_t_print
  (vl_api_sr_multicast_map_add_del_t * mp, void *handle)
{

  u8 *s = 0;
  /* int i; */

  s = format (0, "SCRIPT: sr_multicast_map_add_del ");

  if (mp->multicast_address[0])
    s = format (s, "address %U ", format_ip6_address, &mp->multicast_address);

  if (mp->policy_name[0])
    s = format (s, "sr-policy %s ", &mp->policy_name);


  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}


static void *vl_api_classify_add_del_table_t_print
  (vl_api_classify_add_del_table_t * mp, void *handle)
{
  u8 *s;
  int i;

  s = format (0, "SCRIPT: classify_add_del_table ");

  if (mp->is_add == 0)
    {
      s = format (s, "table %d ", ntohl (mp->table_index));
      s = format (s, "del ");
    }
  else
    {
      s = format (s, "nbuckets %d ", ntohl (mp->nbuckets));
      s = format (s, "memory_size %d ", ntohl (mp->memory_size));
      s = format (s, "skip %d ", ntohl (mp->skip_n_vectors));
      s = format (s, "match %d ", ntohl (mp->match_n_vectors));
      s = format (s, "next-table %d ", ntohl (mp->next_table_index));
      s = format (s, "miss-next %d ", ntohl (mp->miss_next_index));
      s = format (s, "mask hex ");
      for (i = 0; i < ntohl (mp->match_n_vectors) * sizeof (u32x4); i++)
	s = format (s, "%02x", mp->mask[i]);
      vec_add1 (s, ' ');
    }

  FINISH;
}

static void *vl_api_classify_add_del_session_t_print
  (vl_api_classify_add_del_session_t * mp, void *handle)
{
  u8 *s;
  int i, limit = 0;

  s = format (0, "SCRIPT: classify_add_del_session ");

  s = format (s, "table_index %d ", ntohl (mp->table_index));
  s = format (s, "hit_next_index %d ", ntohl (mp->hit_next_index));
  s = format (s, "opaque_index %d ", ntohl (mp->opaque_index));
  s = format (s, "advance %d ", ntohl (mp->advance));
  if (mp->is_add == 0)
    s = format (s, "del ");

  s = format (s, "match hex ");
  for (i = 5 * sizeof (u32x4) - 1; i > 0; i--)
    {
      if (mp->match[i] != 0)
	{
	  limit = i + 1;
	  break;
	}
    }

  for (i = 0; i < limit; i++)
    s = format (s, "%02x", mp->match[i]);

  FINISH;
}

static void *vl_api_classify_set_interface_ip_table_t_print
  (vl_api_classify_set_interface_ip_table_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: classify_set_interface_ip_table ");

  if (mp->is_ipv6)
    s = format (s, "ipv6 ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "table %d ", ntohl (mp->table_index));

  FINISH;
}

static void *vl_api_classify_set_interface_l2_tables_t_print
  (vl_api_classify_set_interface_l2_tables_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: classify_set_interface_l2_tables ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "ip4-table %d ", ntohl (mp->ip4_table_index));
  s = format (s, "ip6-table %d ", ntohl (mp->ip6_table_index));
  s = format (s, "other-table %d ", ntohl (mp->other_table_index));
  s = format (s, "is-input %d ", mp->is_input);

  FINISH;
}

static void *vl_api_add_node_next_t_print
  (vl_api_add_node_next_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: add_node_next ");

  s = format (0, "node %s next %s ", mp->node_name, mp->next_name);

  FINISH;
}

static void *vl_api_l2tpv3_create_tunnel_t_print
  (vl_api_l2tpv3_create_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2tpv3_create_tunnel ");

  s = format (s, "client_address %U our_address %U ",
	      format_ip6_address, (ip6_address_t *) (mp->client_address),
	      format_ip6_address, (ip6_address_t *) (mp->our_address));
  s = format (s, "local_session_id %d ", ntohl (mp->local_session_id));
  s = format (s, "remote_session_id %d ", ntohl (mp->remote_session_id));
  s = format (s, "local_cookie %lld ",
	      clib_net_to_host_u64 (mp->local_cookie));
  s = format (s, "remote_cookie %lld ",
	      clib_net_to_host_u64 (mp->remote_cookie));
  if (mp->l2_sublayer_present)
    s = format (s, "l2-sublayer-present ");

  FINISH;
}

static void *vl_api_l2tpv3_set_tunnel_cookies_t_print
  (vl_api_l2tpv3_set_tunnel_cookies_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2tpv3_set_tunnel_cookies ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "new_local_cookie %llu ",
	      clib_net_to_host_u64 (mp->new_local_cookie));

  s = format (s, "new_remote_cookie %llu ",
	      clib_net_to_host_u64 (mp->new_remote_cookie));

  FINISH;
}

static void *vl_api_l2tpv3_interface_enable_disable_t_print
  (vl_api_l2tpv3_interface_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2tpv3_interface_enable_disable ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->enable_disable == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_l2tpv3_set_lookup_key_t_print
  (vl_api_l2tpv3_set_lookup_key_t * mp, void *handle)
{
  u8 *s;
  char *str = "unknown";

  s = format (0, "SCRIPT: l2tpv3_set_lookup_key ");

  switch (mp->key)
    {
    case L2T_LOOKUP_SRC_ADDRESS:
      str = "lookup_v6_src";
      break;
    case L2T_LOOKUP_DST_ADDRESS:
      str = "lookup_v6_dst";
      break;
    case L2T_LOOKUP_SESSION_ID:
      str = "lookup_session_id";
      break;
    default:
      break;
    }

  s = format (s, "%s ", str);

  FINISH;
}

static void *vl_api_sw_if_l2tpv3_tunnel_dump_t_print
  (vl_api_sw_if_l2tpv3_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_if_l2tpv3_tunnel_dump ");

  FINISH;
}

static void *vl_api_vxlan_add_del_tunnel_t_print
  (vl_api_vxlan_add_del_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: vxlan_add_del_tunnel ");

  if (mp->is_ipv6)
    {
      s = format (s, "src %U ", format_ip6_address,
		  (ip6_address_t *) mp->src_address);
      s = format (s, "dst %U ", format_ip6_address,
		  (ip6_address_t *) mp->dst_address);
    }
  else
    {
      s = format (s, "src %U ", format_ip4_address,
		  (ip4_address_t *) mp->src_address);
      s = format (s, "dst %U ", format_ip4_address,
		  (ip4_address_t *) mp->dst_address);
    }

  if (mp->encap_vrf_id)
    s = format (s, "encap-vrf-id %d ", ntohl (mp->encap_vrf_id));

  s = format (s, "decap-next %d ", ntohl (mp->decap_next_index));

  s = format (s, "vni %d ", ntohl (mp->vni));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_vxlan_tunnel_dump_t_print
  (vl_api_vxlan_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: vxlan_tunnel_dump ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_gre_add_del_tunnel_t_print
  (vl_api_gre_add_del_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: gre_add_del_tunnel ");

  s = format (s, "dst %U ", format_ip46_address,
	      (ip46_address_t *) & (mp->dst_address),
	      mp->is_ipv6 ? IP46_TYPE_IP6 : IP46_TYPE_IP4);

  s = format (s, "src %U ", format_ip46_address,
	      (ip46_address_t *) & (mp->src_address),
	      mp->is_ipv6 ? IP46_TYPE_IP6 : IP46_TYPE_IP4);

  if (mp->outer_fib_id)
    s = format (s, "outer-fib-id %d ", ntohl (mp->outer_fib_id));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_gre_tunnel_dump_t_print
  (vl_api_gre_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: gre_tunnel_dump ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_l2_fib_clear_table_t_print
  (vl_api_l2_fib_clear_table_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2_fib_clear_table ");

  FINISH;
}

static void *vl_api_l2_interface_efp_filter_t_print
  (vl_api_l2_interface_efp_filter_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2_interface_efp_filter ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  if (mp->enable_disable)
    s = format (s, "enable ");
  else
    s = format (s, "disable ");

  FINISH;
}

static void *vl_api_l2_interface_vlan_tag_rewrite_t_print
  (vl_api_l2_interface_vlan_tag_rewrite_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2_interface_vlan_tag_rewrite ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "vtr_op %d ", ntohl (mp->vtr_op));
  s = format (s, "push_dot1q %d ", ntohl (mp->push_dot1q));
  s = format (s, "tag1 %d ", ntohl (mp->tag1));
  s = format (s, "tag2 %d ", ntohl (mp->tag2));

  FINISH;
}

static void *vl_api_create_vhost_user_if_t_print
  (vl_api_create_vhost_user_if_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: create_vhost_user_if ");

  s = format (s, "socket %s ", mp->sock_filename);
  if (mp->is_server)
    s = format (s, "server ");
  if (mp->renumber)
    s = format (s, "renumber %d ", ntohl (mp->custom_dev_instance));

  FINISH;
}

static void *vl_api_modify_vhost_user_if_t_print
  (vl_api_modify_vhost_user_if_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: modify_vhost_user_if ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "socket %s ", mp->sock_filename);
  if (mp->is_server)
    s = format (s, "server ");
  if (mp->renumber)
    s = format (s, "renumber %d ", ntohl (mp->custom_dev_instance));

  FINISH;
}

static void *vl_api_delete_vhost_user_if_t_print
  (vl_api_delete_vhost_user_if_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: delete_vhost_user_if ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_sw_interface_vhost_user_dump_t_print
  (vl_api_sw_interface_vhost_user_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_vhost_user_dump ");

  FINISH;
}

static void *vl_api_sw_interface_dump_t_print
  (vl_api_sw_interface_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_dump ");

  if (mp->name_filter_valid)
    s = format (s, "name_filter %s ", mp->name_filter);
  else
    s = format (s, "all ");

  FINISH;
}

static void *vl_api_l2_fib_table_dump_t_print
  (vl_api_l2_fib_table_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: l2_fib_table_dump ");

  s = format (s, "bd_id %d ", ntohl (mp->bd_id));

  FINISH;
}

static void *vl_api_control_ping_t_print
  (vl_api_control_ping_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: control_ping ");

  FINISH;
}

static void *vl_api_want_interface_events_t_print
  (vl_api_want_interface_events_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: want_interface_events pid %d enable %d ",
	      ntohl (mp->pid), ntohl (mp->enable_disable));

  FINISH;
}

static void *vl_api_cli_request_t_print
  (vl_api_cli_request_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: cli_request ");

  FINISH;
}

static void *vl_api_cli_inband_t_print
  (vl_api_cli_inband_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: cli_inband ");

  FINISH;
}

static void *vl_api_memclnt_create_t_print
  (vl_api_memclnt_create_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: memclnt_create name %s ", mp->name);

  FINISH;
}

static void *vl_api_show_version_t_print
  (vl_api_show_version_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: show_version ");

  FINISH;
}

static void *vl_api_vxlan_gpe_add_del_tunnel_t_print
  (vl_api_vxlan_gpe_add_del_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: vxlan_gpe_add_del_tunnel ");

  s = format (s, "local %U ", format_ip46_address, &mp->local, mp->is_ipv6);

  s = format (s, "remote %U ", format_ip46_address, &mp->remote, mp->is_ipv6);

  s = format (s, "protocol %d ", ntohl (mp->protocol));

  s = format (s, "vni %d ", ntohl (mp->vni));

  if (mp->is_add == 0)
    s = format (s, "del ");

  if (mp->encap_vrf_id)
    s = format (s, "encap-vrf-id %d ", ntohl (mp->encap_vrf_id));

  if (mp->decap_vrf_id)
    s = format (s, "decap-vrf-id %d ", ntohl (mp->decap_vrf_id));

  FINISH;
}

static void *vl_api_vxlan_gpe_tunnel_dump_t_print
  (vl_api_vxlan_gpe_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: vxlan_gpe_tunnel_dump ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_interface_name_renumber_t_print
  (vl_api_interface_name_renumber_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: interface_renumber ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  s = format (s, "new_show_dev_instance %d ",
	      ntohl (mp->new_show_dev_instance));

  FINISH;
}

static void *vl_api_want_ip4_arp_events_t_print
  (vl_api_want_ip4_arp_events_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: want_ip4_arp_events ");
  s = format (s, "pid %d address %U ", mp->pid,
	      format_ip4_address, &mp->address);
  if (mp->enable_disable == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_want_ip6_nd_events_t_print
  (vl_api_want_ip6_nd_events_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: want_ip6_nd_events ");
  s = format (s, "pid %d address %U ", mp->pid,
	      format_ip6_address, mp->address);
  if (mp->enable_disable == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_input_acl_set_interface_t_print
  (vl_api_input_acl_set_interface_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: input_acl_set_interface ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "ip4-table %d ", ntohl (mp->ip4_table_index));
  s = format (s, "ip6-table %d ", ntohl (mp->ip6_table_index));
  s = format (s, "l2-table %d ", ntohl (mp->l2_table_index));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_ip_address_dump_t_print
  (vl_api_ip_address_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ip6_address_dump ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "is_ipv6 %d ", mp->is_ipv6 != 0);

  FINISH;
}

static void *
vl_api_ip_dump_t_print (vl_api_ip_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ip_dump ");
  s = format (s, "is_ipv6 %d ", mp->is_ipv6 != 0);

  FINISH;
}

static void *vl_api_cop_interface_enable_disable_t_print
  (vl_api_cop_interface_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: cop_interface_enable_disable ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  if (mp->enable_disable)
    s = format (s, "enable ");
  else
    s = format (s, "disable ");

  FINISH;
}

static void *vl_api_cop_whitelist_enable_disable_t_print
  (vl_api_cop_whitelist_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: cop_whitelist_enable_disable ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  s = format (s, "fib-id %d ", ntohl (mp->fib_id));
  if (mp->ip4)
    s = format (s, "ip4 ");
  if (mp->ip6)
    s = format (s, "ip6 ");
  if (mp->default_cop)
    s = format (s, "default ");

  FINISH;
}

static void *vl_api_af_packet_create_t_print
  (vl_api_af_packet_create_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: af_packet_create ");
  s = format (s, "host_if_name %s ", mp->host_if_name);
  if (mp->use_random_hw_addr)
    s = format (s, "hw_addr random ");
  else
    s = format (s, "hw_addr %U ", format_ethernet_address, mp->hw_addr);

  FINISH;
}

static void *vl_api_af_packet_delete_t_print
  (vl_api_af_packet_delete_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: af_packet_delete ");
  s = format (s, "host_if_name %s ", mp->host_if_name);

  FINISH;
}

static u8 *
format_policer_action (u8 * s, va_list * va)
{
  u32 action = va_arg (*va, u32);
  u32 dscp = va_arg (*va, u32);
  char *t = 0;

  if (action == SSE2_QOS_ACTION_DROP)
    s = format (s, "drop");
  else if (action == SSE2_QOS_ACTION_TRANSMIT)
    s = format (s, "transmit");
  else if (action == SSE2_QOS_ACTION_MARK_AND_TRANSMIT)
    {
      s = format (s, "mark-and-transmit ");
      switch (dscp)
	{
#define _(v,f,str) case VNET_DSCP_##f: t = str; break;
	  foreach_vnet_dscp
#undef _
	default:
	  break;
	}
      s = format (s, "%s", t);
    }

  return s;
}

static void *vl_api_policer_add_del_t_print
  (vl_api_policer_add_del_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: policer_add_del ");
  s = format (s, "name %s ", mp->name);
  s = format (s, "cir %d ", mp->cir);
  s = format (s, "eir %d ", mp->eir);
  s = format (s, "cb %d ", mp->cb);
  s = format (s, "eb %d ", mp->eb);

  switch (mp->rate_type)
    {
    case SSE2_QOS_RATE_KBPS:
      s = format (s, "rate_type kbps ");
      break;
    case SSE2_QOS_RATE_PPS:
      s = format (s, "rate_type pps ");
      break;
    default:
      break;
    }

  switch (mp->round_type)
    {
    case SSE2_QOS_ROUND_TO_CLOSEST:
      s = format (s, "round_type closest ");
      break;
    case SSE2_QOS_ROUND_TO_UP:
      s = format (s, "round_type up ");
      break;
    case SSE2_QOS_ROUND_TO_DOWN:
      s = format (s, "round_type down ");
      break;
    default:
      break;
    }

  switch (mp->type)
    {
    case SSE2_QOS_POLICER_TYPE_1R2C:
      s = format (s, "type 1r2c ");
      break;
    case SSE2_QOS_POLICER_TYPE_1R3C_RFC_2697:
      s = format (s, "type 1r3c ");
      break;
    case SSE2_QOS_POLICER_TYPE_2R3C_RFC_2698:
      s = format (s, "type 2r3c-2698 ");
      break;
    case SSE2_QOS_POLICER_TYPE_2R3C_RFC_4115:
      s = format (s, "type 2r3c-4115 ");
      break;
    case SSE2_QOS_POLICER_TYPE_2R3C_RFC_MEF5CF1:
      s = format (s, "type 2r3c-mef5cf1 ");
      break;
    default:
      break;
    }

  s = format (s, "conform_action %U ", format_policer_action,
	      mp->conform_action_type, mp->conform_dscp);
  s = format (s, "exceed_action %U ", format_policer_action,
	      mp->exceed_action_type, mp->exceed_dscp);
  s = format (s, "violate_action %U ", format_policer_action,
	      mp->violate_action_type, mp->violate_dscp);

  if (mp->color_aware)
    s = format (s, "color-aware ");
  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_policer_dump_t_print
  (vl_api_policer_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: policer_dump ");
  if (mp->match_name_valid)
    s = format (s, "name %s ", mp->match_name);

  FINISH;
}

static void *vl_api_policer_classify_set_interface_t_print
  (vl_api_policer_classify_set_interface_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: policer_classify_set_interface ");
  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));
  if (mp->ip4_table_index != ~0)
    s = format (s, "ip4-table %d ", ntohl (mp->ip4_table_index));
  if (mp->ip6_table_index != ~0)
    s = format (s, "ip6-table %d ", ntohl (mp->ip6_table_index));
  if (mp->l2_table_index != ~0)
    s = format (s, "l2-table %d ", ntohl (mp->l2_table_index));
  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_policer_classify_dump_t_print
  (vl_api_policer_classify_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: policer_classify_dump ");
  switch (mp->type)
    {
    case POLICER_CLASSIFY_TABLE_IP4:
      s = format (s, "type ip4 ");
      break;
    case POLICER_CLASSIFY_TABLE_IP6:
      s = format (s, "type ip6 ");
      break;
    case POLICER_CLASSIFY_TABLE_L2:
      s = format (s, "type l2 ");
      break;
    default:
      break;
    }

  FINISH;
}

static void *vl_api_sw_interface_clear_stats_t_print
  (vl_api_sw_interface_clear_stats_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: sw_interface_clear_stats ");
  if (mp->sw_if_index != ~0)
    s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_mpls_gre_tunnel_dump_t_print
  (vl_api_mpls_gre_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_gre_tunnel_dump ");

  s = format (s, "tunnel_index %d ", ntohl (mp->tunnel_index));

  FINISH;
}

static void *vl_api_mpls_eth_tunnel_dump_t_print
  (vl_api_mpls_eth_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_eth_tunnel_dump ");

  s = format (s, "tunnel_index %d ", ntohl (mp->tunnel_index));

  FINISH;
}

static void *vl_api_mpls_fib_encap_dump_t_print
  (vl_api_mpls_fib_encap_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_fib_encap_dump ");

  FINISH;
}

static void *vl_api_mpls_fib_decap_dump_t_print
  (vl_api_mpls_fib_decap_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: mpls_fib_decap_dump ");

  FINISH;
}

static void *vl_api_classify_table_ids_t_print
  (vl_api_classify_table_ids_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: classify_table_ids ");

  FINISH;
}

static void *vl_api_classify_table_by_interface_t_print
  (vl_api_classify_table_by_interface_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: classify_table_by_interface ");
  if (mp->sw_if_index != ~0)
    s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

static void *vl_api_classify_table_info_t_print
  (vl_api_classify_table_info_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: classify_table_info ");
  if (mp->table_id != ~0)
    s = format (s, "table_id %d ", ntohl (mp->table_id));

  FINISH;
}

static void *vl_api_classify_session_dump_t_print
  (vl_api_classify_session_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: classify_session_dump ");
  if (mp->table_id != ~0)
    s = format (s, "table_id %d ", ntohl (mp->table_id));

  FINISH;
}

static void *vl_api_ipfix_enable_t_print
  (vl_api_ipfix_enable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ipfix_enable ");

  s = format (s, "collector-address %U ", format_ip4_address,
	      (ip4_address_t *) mp->collector_address);
  s = format (s, "collector-port %d ", ntohs (mp->collector_port));
  s = format (s, "src-address %U ", format_ip4_address,
	      (ip4_address_t *) mp->src_address);
  s = format (s, "vrf-id %d ", ntohl (mp->vrf_id));
  s = format (s, "path-mtu %d ", ntohl (mp->path_mtu));
  s = format (s, "template-interval %d ", ntohl (mp->template_interval));

  FINISH;
}

static void *vl_api_ipfix_dump_t_print
  (vl_api_ipfix_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ipfix_dump ");

  FINISH;
}

static void *vl_api_get_next_index_t_print
  (vl_api_get_next_index_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: get_next_index ");
  s = format (s, "node-name %s ", mp->node_name);
  s = format (s, "next-node-name %s ", mp->next_name);

  FINISH;
}

static void *vl_api_pg_create_interface_t_print
  (vl_api_pg_create_interface_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: pg_create_interface ");
  s = format (0, "if_id %d", ntohl (mp->interface_id));

  FINISH;
}

static void *vl_api_pg_capture_t_print
  (vl_api_pg_capture_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: pg_capture ");
  s = format (0, "if_id %d ", ntohl (mp->interface_id));
  s = format (0, "pcap %s", mp->pcap_file_name);
  if (mp->count != ~0)
    s = format (s, "count %d ", ntohl (mp->count));
  if (!mp->is_enabled)
    s = format (s, "disable");

  FINISH;
}

static void *vl_api_pg_enable_disable_t_print
  (vl_api_pg_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: pg_enable_disable ");
  if (ntohl (mp->stream_name_length) > 0)
    s = format (s, "stream %s", mp->stream_name);
  if (!mp->is_enabled)
    s = format (s, "disable");

  FINISH;
}

static void *vl_api_ip_source_and_port_range_check_add_del_t_print
  (vl_api_ip_source_and_port_range_check_add_del_t * mp, void *handle)
{
  u8 *s;
  int i;

  s = format (0, "SCRIPT: ip_source_and_port_range_check_add_del ");
  if (mp->is_ipv6)
    s = format (s, "%U/%d ", format_ip6_address, mp->address,
		mp->mask_length);
  else
    s = format (s, "%U/%d ", format_ip4_address, mp->address,
		mp->mask_length);

  for (i = 0; i < mp->number_of_ranges; i++)
    {
      s = format (s, "range %d - %d ", mp->low_ports[i], mp->high_ports[i]);
    }

  s = format (s, "vrf %d ", ntohl (mp->vrf_id));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_ip_source_and_port_range_check_interface_add_del_t_print
  (vl_api_ip_source_and_port_range_check_interface_add_del_t * mp,
   void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ip_source_and_port_range_check_interface_add_del ");

  s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  if (mp->tcp_out_vrf_id != ~0)
    s = format (s, "tcp-out-vrf %d ", ntohl (mp->tcp_out_vrf_id));

  if (mp->udp_out_vrf_id != ~0)
    s = format (s, "udp-out-vrf %d ", ntohl (mp->udp_out_vrf_id));

  if (mp->tcp_in_vrf_id != ~0)
    s = format (s, "tcp-in-vrf %d ", ntohl (mp->tcp_in_vrf_id));

  if (mp->udp_in_vrf_id != ~0)
    s = format (s, "udp-in-vrf %d ", ntohl (mp->udp_in_vrf_id));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_lisp_enable_disable_t_print
  (vl_api_lisp_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_enable_disable %s",
	      mp->is_en ? "enable" : "disable");

  FINISH;
}

static void *vl_api_lisp_gpe_add_del_iface_t_print
  (vl_api_lisp_gpe_add_del_iface_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_gpe_add_del_iface ");

  s = format (s, "%s ", mp->is_add ? "up" : "down");
  s = format (s, "vni %d ", mp->vni);
  s = format (s, "%s %d ", mp->is_l2 ? "bd_id" : "table_id", mp->dp_table);

  FINISH;
}

static void *vl_api_lisp_pitr_set_locator_set_t_print
  (vl_api_lisp_pitr_set_locator_set_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_pitr_set_locator_set ");

  if (mp->is_add)
    s = format (s, "locator-set %s ", mp->ls_name);
  else
    s = format (s, "del");

  FINISH;
}

static u8 *
format_lisp_flat_eid (u8 * s, va_list * args)
{
  u32 type = va_arg (*args, u32);
  u8 *eid = va_arg (*args, u8 *);
  u32 eid_len = va_arg (*args, u32);

  switch (type)
    {
    case 0:
      return format (s, "%U/%d", format_ip4_address, eid, eid_len);
    case 1:
      return format (s, "%U/%d", format_ip6_address, eid, eid_len);
    case 3:
      return format (s, "%U", format_ethernet_address, eid);
    }
  return 0;
}

static u8 *
format_lisp_eid_vat (u8 * s, va_list * args)
{
  u32 type = va_arg (*args, u32);
  u8 *eid = va_arg (*args, u8 *);
  u32 eid_len = va_arg (*args, u32);
  u8 *seid = va_arg (*args, u8 *);
  u32 seid_len = va_arg (*args, u32);
  u32 is_src_dst = va_arg (*args, u32);

  if (is_src_dst)
    s = format (s, "%U|", format_lisp_flat_eid, type, seid, seid_len);

  s = format (s, "%U", format_lisp_flat_eid, type, eid, eid_len);

  return s;
}

/** Used for transferring locators via VPP API */
typedef CLIB_PACKED (struct
		     {
		     u8 is_ip4;
	     /**< is locator an IPv4 address */
		     u8 priority;
	       /**< locator priority */
		     u8 weight;
	       /**< locator weight */
		     u8 addr[16];
	       /**< IPv4/IPv6 address */
		     }) rloc_t;

static u8 *
format_rloc (u8 * s, va_list * args)
{
  rloc_t *rloc = va_arg (*args, rloc_t *);

  if (rloc->is_ip4)
    s = format (s, "%U ", format_ip4_address, rloc->addr);
  else
    s = format (s, "%U ", format_ip6_address, rloc->addr);

  s = format (s, "p %d w %d", rloc->priority, rloc->weight);

  return s;
}

static void *vl_api_lisp_add_del_remote_mapping_t_print
  (vl_api_lisp_add_del_remote_mapping_t * mp, void *handle)
{
  u8 *s;
  u32 i, rloc_num = 0;

  s = format (0, "SCRIPT: lisp_add_del_remote_mapping ");

  if (mp->del_all)
    s = format (s, "del-all ");

  s = format (s, "%s ", mp->is_add ? "add" : "del");
  s = format (s, "vni %d ", clib_net_to_host_u32 (mp->vni));

  s = format (s, "deid %U ", format_lisp_eid_vat,
	      mp->eid_type, mp->eid, mp->eid_len, mp->seid, mp->seid_len,
	      mp->is_src_dst);

  rloc_num = clib_net_to_host_u32 (mp->rloc_num);

  if (0 == rloc_num)
    s = format (s, "action %d", mp->action);
  else
    {
      rloc_t *rloc = (rloc_t *) mp->rlocs;
      for (i = 0; i < rloc_num; i++)
	s = format (s, "%U ", format_rloc, &rloc[i]);
    }

  FINISH;
}

static void *vl_api_lisp_add_del_adjacency_t_print
  (vl_api_lisp_add_del_adjacency_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_add_del_adjacency ");

  s = format (s, "%s ", mp->is_add ? "add" : "del");
  s = format (s, "vni %d ", clib_net_to_host_u32 (mp->vni));
  s = format (s, "deid %U seid %U ",
	      format_lisp_flat_eid, mp->eid_type, mp->deid, mp->deid_len,
	      format_lisp_flat_eid, mp->eid_type, mp->seid, mp->seid_len);

  FINISH;
}

static void *vl_api_lisp_add_del_map_request_itr_rlocs_t_print
  (vl_api_lisp_add_del_map_request_itr_rlocs_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_add_del_map_request_itr_rlocs ");

  if (mp->is_add)
    s = format (s, "%s", mp->locator_set_name);
  else
    s = format (s, "del");

  FINISH;
}

static void *vl_api_lisp_eid_table_add_del_map_t_print
  (vl_api_lisp_eid_table_add_del_map_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_eid_table_add_del_map ");

  if (!mp->is_add)
    s = format (s, "del ");

  s = format (s, "vni %d ", clib_net_to_host_u32 (mp->vni));
  s = format (s, "%s %d ",
	      mp->is_l2 ? "bd_index" : "vrf",
	      clib_net_to_host_u32 (mp->dp_table));
  FINISH;
}

static void *vl_api_lisp_add_del_local_eid_t_print
  (vl_api_lisp_add_del_local_eid_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_add_del_local_eid ");

  if (!mp->is_add)
    s = format (s, "del ");

  s = format (s, "vni %d ", clib_net_to_host_u32 (mp->vni));
  s = format (s, "eid %U ", format_lisp_flat_eid, mp->eid_type, mp->eid,
	      mp->prefix_len);
  s = format (s, "locator-set %s ", mp->locator_set_name);
  FINISH;
}

static void *vl_api_lisp_gpe_add_del_fwd_entry_t_print
  (vl_api_lisp_gpe_add_del_fwd_entry_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_gpe_add_del_fwd_entry TODO");

  FINISH;
}

static void *vl_api_lisp_add_del_map_resolver_t_print
  (vl_api_lisp_add_del_map_resolver_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_add_del_map_resolver ");

  if (!mp->is_add)
    s = format (s, "del ");

  if (mp->is_ipv6)
    s = format (s, "%U ", format_ip6_address, mp->ip_address);
  else
    s = format (s, "%U ", format_ip4_address, mp->ip_address);

  FINISH;
}

static void *vl_api_lisp_gpe_enable_disable_t_print
  (vl_api_lisp_gpe_enable_disable_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_gpe_enable_disable ");

  s = format (s, "%s ", mp->is_en ? "enable" : "disable");

  FINISH;
}

typedef CLIB_PACKED (struct
		     {
		     u32 sw_if_index;
		   /**< locator sw_if_index */
		     u8 priority;
	       /**< locator priority */
		     u8 weight;
	       /**< locator weight */
		     }) ls_locator_t;

static u8 *
format_locator (u8 * s, va_list * args)
{
  ls_locator_t *l = va_arg (*args, ls_locator_t *);

  return format (s, "sw_if_index %d p %d w %d",
		 l->sw_if_index, l->priority, l->weight);
}

static void *vl_api_lisp_add_del_locator_set_t_print
  (vl_api_lisp_add_del_locator_set_t * mp, void *handle)
{
  u8 *s;
  u32 loc_num = 0, i;
  ls_locator_t *locs;

  s = format (0, "SCRIPT: lisp_add_del_locator_set ");

  if (!mp->is_add)
    s = format (s, "del ");

  s = format (s, "locator-set %s ", mp->locator_set_name);

  loc_num = clib_net_to_host_u32 (mp->locator_num);
  locs = (ls_locator_t *) mp->locators;

  for (i = 0; i < loc_num; i++)
    s = format (s, "%U ", format_locator, &locs[i]);

  FINISH;
}

static void *vl_api_lisp_add_del_locator_t_print
  (vl_api_lisp_add_del_locator_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_add_del_locator ");

  if (!mp->is_add)
    s = format (s, "del ");

  s = format (s, "locator-set %s ", mp->locator_set_name);
  s = format (s, "sw_if_index %d ", mp->sw_if_index);
  s = format (s, "p %d w %d ", mp->priority, mp->weight);

  FINISH;
}

static void *vl_api_lisp_locator_set_dump_t_print
  (vl_api_lisp_locator_set_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_locator_set_dump ");

  /* not possible to reconstruct original VAT command */

  FINISH;
}

static void *vl_api_lisp_eid_table_dump_t_print
  (vl_api_lisp_eid_table_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_eid_table_dump ");

  if (mp->eid_set)
    {
      s = format (s, "vni %d ", clib_net_to_host_u32 (mp->vni));
      s = format (s, "eid %U ", format_lisp_flat_eid, mp->eid_type,
		  mp->eid, mp->prefix_length);
      switch (mp->filter)
	{
	case 1:
	  s = format (s, "local ");
	  break;
	case 2:
	  s = format (s, "remote ");
	  break;
	}
    }

  FINISH;
}

static void *vl_api_lisp_eid_table_map_dump_t_print
  (vl_api_lisp_eid_table_map_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: lisp_eid_table_map_dump ");

  if (mp->is_l2)
    s = format (s, "l2");
  else
    s = format (s, "l3");

  FINISH;
}

static void *vl_api_ipsec_gre_add_del_tunnel_t_print
  (vl_api_ipsec_gre_add_del_tunnel_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ipsec_gre_add_del_tunnel ");

  s = format (s, "dst %U ", format_ip4_address,
	      (ip4_address_t *) & (mp->dst_address));

  s = format (s, "src %U ", format_ip4_address,
	      (ip4_address_t *) & (mp->src_address));

  s = format (s, "local_sa %d ", ntohl (mp->local_sa_id));

  s = format (s, "remote_sa %d ", ntohl (mp->remote_sa_id));

  if (mp->is_add == 0)
    s = format (s, "del ");

  FINISH;
}

static void *vl_api_ipsec_gre_tunnel_dump_t_print
  (vl_api_ipsec_gre_tunnel_dump_t * mp, void *handle)
{
  u8 *s;

  s = format (0, "SCRIPT: ipsec_gre_tunnel_dump ");

  if (mp->sw_if_index != ~0)
    s = format (s, "sw_if_index %d ", ntohl (mp->sw_if_index));

  FINISH;
}

#define foreach_custom_print_no_arg_function                            \
_(lisp_eid_table_vni_dump)                                              \
_(lisp_map_resolver_dump)                                               \
_(lisp_gpe_tunnel_dump)

#define _(f)                                                            \
static void * vl_api_ ## f ## _t_print                                  \
  (vl_api_ ## f ## _t * mp, void * handle)                              \
{                                                                       \
  u8 * s;                                                               \
  s = format (0, "SCRIPT: " #f );                                       \
  FINISH;                                                               \
}
foreach_custom_print_no_arg_function
#undef _
#define foreach_custom_print_function                                   \
_(CREATE_LOOPBACK, create_loopback)                                     \
_(SW_INTERFACE_SET_FLAGS, sw_interface_set_flags)                       \
_(SW_INTERFACE_ADD_DEL_ADDRESS, sw_interface_add_del_address)           \
_(SW_INTERFACE_SET_TABLE, sw_interface_set_table)                       \
_(SW_INTERFACE_SET_VPATH, sw_interface_set_vpath)                       \
_(TAP_CONNECT, tap_connect)                                             \
_(TAP_MODIFY, tap_modify)                                               \
_(TAP_DELETE, tap_delete)                                               \
_(SW_INTERFACE_TAP_DUMP, sw_interface_tap_dump)                         \
_(IP_ADD_DEL_ROUTE, ip_add_del_route)                                   \
_(PROXY_ARP_ADD_DEL, proxy_arp_add_del)                                 \
_(PROXY_ARP_INTFC_ENABLE_DISABLE, proxy_arp_intfc_enable_disable)       \
_(MPLS_ADD_DEL_DECAP, mpls_add_del_decap)                               \
_(MPLS_ADD_DEL_ENCAP, mpls_add_del_encap)                               \
_(MPLS_GRE_ADD_DEL_TUNNEL, mpls_gre_add_del_tunnel)                     \
_(MPLS_ETHERNET_ADD_DEL_TUNNEL, mpls_ethernet_add_del_tunnel)		\
_(MPLS_ETHERNET_ADD_DEL_TUNNEL_2, mpls_ethernet_add_del_tunnel_2)	\
_(SW_INTERFACE_SET_UNNUMBERED, sw_interface_set_unnumbered)             \
_(IP_NEIGHBOR_ADD_DEL, ip_neighbor_add_del)                             \
_(RESET_VRF, reset_vrf)                                                 \
_(CREATE_VLAN_SUBIF, create_vlan_subif)                                 \
_(CREATE_SUBIF, create_subif)                                           \
_(OAM_ADD_DEL, oam_add_del)                                             \
_(RESET_FIB, reset_fib)                                                 \
_(DHCP_PROXY_CONFIG, dhcp_proxy_config)                                 \
_(DHCP_PROXY_SET_VSS, dhcp_proxy_set_vss)                               \
_(SET_IP_FLOW_HASH, set_ip_flow_hash)                                   \
_(SW_INTERFACE_IP6_SET_LINK_LOCAL_ADDRESS,                              \
  sw_interface_ip6_set_link_local_address)                              \
_(SW_INTERFACE_IP6ND_RA_PREFIX, sw_interface_ip6nd_ra_prefix)           \
_(SW_INTERFACE_IP6ND_RA_CONFIG, sw_interface_ip6nd_ra_config)           \
_(SET_ARP_NEIGHBOR_LIMIT, set_arp_neighbor_limit)                       \
_(L2_PATCH_ADD_DEL, l2_patch_add_del)                                   \
_(SR_TUNNEL_ADD_DEL, sr_tunnel_add_del)					\
_(SR_POLICY_ADD_DEL, sr_policy_add_del)					\
_(SR_MULTICAST_MAP_ADD_DEL, sr_multicast_map_add_del)                   \
_(SW_INTERFACE_SET_L2_XCONNECT, sw_interface_set_l2_xconnect)           \
_(L2FIB_ADD_DEL, l2fib_add_del)                                         \
_(L2_FLAGS, l2_flags)                                                   \
_(BRIDGE_FLAGS, bridge_flags)                                           \
_(CLASSIFY_ADD_DEL_TABLE, classify_add_del_table)			\
_(CLASSIFY_ADD_DEL_SESSION, classify_add_del_session)			\
_(SW_INTERFACE_SET_L2_BRIDGE, sw_interface_set_l2_bridge)		\
_(BRIDGE_DOMAIN_ADD_DEL, bridge_domain_add_del)                         \
_(BRIDGE_DOMAIN_DUMP, bridge_domain_dump)                               \
_(CLASSIFY_SET_INTERFACE_IP_TABLE, classify_set_interface_ip_table)	\
_(CLASSIFY_SET_INTERFACE_L2_TABLES, classify_set_interface_l2_tables)	\
_(ADD_NODE_NEXT, add_node_next)						\
_(DHCP_PROXY_CONFIG_2, dhcp_proxy_config_2)	                        \
_(DHCP_CLIENT_CONFIG, dhcp_client_config)	                        \
_(L2TPV3_CREATE_TUNNEL, l2tpv3_create_tunnel)                           \
_(L2TPV3_SET_TUNNEL_COOKIES, l2tpv3_set_tunnel_cookies)                 \
_(L2TPV3_INTERFACE_ENABLE_DISABLE, l2tpv3_interface_enable_disable)     \
_(L2TPV3_SET_LOOKUP_KEY, l2tpv3_set_lookup_key)                         \
_(SW_IF_L2TPV3_TUNNEL_DUMP, sw_if_l2tpv3_tunnel_dump)                   \
_(VXLAN_ADD_DEL_TUNNEL, vxlan_add_del_tunnel)                           \
_(VXLAN_TUNNEL_DUMP, vxlan_tunnel_dump)                                 \
_(GRE_ADD_DEL_TUNNEL, gre_add_del_tunnel)                               \
_(GRE_TUNNEL_DUMP, gre_tunnel_dump)                                     \
_(L2_FIB_CLEAR_TABLE, l2_fib_clear_table)                               \
_(L2_INTERFACE_EFP_FILTER, l2_interface_efp_filter)                     \
_(L2_INTERFACE_VLAN_TAG_REWRITE, l2_interface_vlan_tag_rewrite)         \
_(CREATE_VHOST_USER_IF, create_vhost_user_if)				\
_(MODIFY_VHOST_USER_IF, modify_vhost_user_if)				\
_(DELETE_VHOST_USER_IF, delete_vhost_user_if)				\
_(SW_INTERFACE_DUMP, sw_interface_dump)					\
_(CONTROL_PING, control_ping)						\
_(WANT_INTERFACE_EVENTS, want_interface_events)				\
_(CLI_REQUEST, cli_request)						\
_(CLI_INBAND, cli_inband)						\
_(MEMCLNT_CREATE, memclnt_create)					\
_(SW_INTERFACE_VHOST_USER_DUMP, sw_interface_vhost_user_dump)           \
_(SHOW_VERSION, show_version)                                           \
_(L2_FIB_TABLE_DUMP, l2_fib_table_dump)                                 \
_(VXLAN_GPE_ADD_DEL_TUNNEL, vxlan_gpe_add_del_tunnel) 			\
_(VXLAN_GPE_TUNNEL_DUMP, vxlan_gpe_tunnel_dump)                         \
_(INTERFACE_NAME_RENUMBER, interface_name_renumber)			\
_(WANT_IP4_ARP_EVENTS, want_ip4_arp_events)                             \
_(WANT_IP6_ND_EVENTS, want_ip6_nd_events)                               \
_(INPUT_ACL_SET_INTERFACE, input_acl_set_interface)                     \
_(IP_ADDRESS_DUMP, ip_address_dump)                                     \
_(IP_DUMP, ip_dump)                                                     \
_(DELETE_LOOPBACK, delete_loopback)                                     \
_(BD_IP_MAC_ADD_DEL, bd_ip_mac_add_del)					\
_(COP_INTERFACE_ENABLE_DISABLE, cop_interface_enable_disable) 		\
_(COP_WHITELIST_ENABLE_DISABLE, cop_whitelist_enable_disable)           \
_(AF_PACKET_CREATE, af_packet_create)					\
_(AF_PACKET_DELETE, af_packet_delete)					\
_(SW_INTERFACE_CLEAR_STATS, sw_interface_clear_stats)                   \
_(MPLS_GRE_TUNNEL_DUMP, mpls_gre_tunnel_dump)                           \
_(MPLS_ETH_TUNNEL_DUMP, mpls_eth_tunnel_dump)                           \
_(MPLS_FIB_ENCAP_DUMP, mpls_fib_encap_dump)                             \
_(MPLS_FIB_DECAP_DUMP, mpls_fib_decap_dump)                             \
_(CLASSIFY_TABLE_IDS,classify_table_ids)                                \
_(CLASSIFY_TABLE_BY_INTERFACE, classify_table_by_interface)             \
_(CLASSIFY_TABLE_INFO,classify_table_info)                              \
_(CLASSIFY_SESSION_DUMP,classify_session_dump)                          \
_(IPFIX_ENABLE,ipfix_enable)                                            \
_(IPFIX_DUMP,ipfix_dump)                                                \
_(GET_NEXT_INDEX, get_next_index)                                       \
_(PG_CREATE_INTERFACE,pg_create_interface)                              \
_(PG_CAPTURE, pg_capture)                                               \
_(PG_ENABLE_DISABLE, pg_enable_disable)                                 \
_(POLICER_ADD_DEL, policer_add_del)                                     \
_(POLICER_DUMP, policer_dump)                                           \
_(POLICER_CLASSIFY_SET_INTERFACE, policer_classify_set_interface)       \
_(POLICER_CLASSIFY_DUMP, policer_classify_dump)                         \
_(IP_SOURCE_AND_PORT_RANGE_CHECK_ADD_DEL,                               \
  ip_source_and_port_range_check_add_del)                               \
_(IP_SOURCE_AND_PORT_RANGE_CHECK_INTERFACE_ADD_DEL,                     \
  ip_source_and_port_range_check_interface_add_del)                     \
_(LISP_ENABLE_DISABLE, lisp_enable_disable)                             \
_(LISP_GPE_ENABLE_DISABLE, lisp_gpe_enable_disable)                     \
_(LISP_GPE_ADD_DEL_IFACE, lisp_gpe_add_del_iface)                       \
_(LISP_PITR_SET_LOCATOR_SET, lisp_pitr_set_locator_set)                 \
_(LISP_ADD_DEL_REMOTE_MAPPING, lisp_add_del_remote_mapping)             \
_(LISP_ADD_DEL_ADJACENCY, lisp_add_del_adjacency)                       \
_(LISP_ADD_DEL_MAP_REQUEST_ITR_RLOCS,                                   \
  lisp_add_del_map_request_itr_rlocs)                                   \
_(LISP_EID_TABLE_ADD_DEL_MAP, lisp_eid_table_add_del_map)               \
_(LISP_ADD_DEL_LOCAL_EID, lisp_add_del_local_eid)                       \
_(LISP_GPE_ADD_DEL_FWD_ENTRY, lisp_gpe_add_del_fwd_entry)               \
_(LISP_ADD_DEL_LOCATOR_SET, lisp_add_del_locator_set)                   \
_(LISP_ADD_DEL_MAP_RESOLVER, lisp_add_del_map_resolver)                 \
_(LISP_ADD_DEL_LOCATOR, lisp_add_del_locator)                           \
_(LISP_EID_TABLE_DUMP, lisp_eid_table_dump)                             \
_(LISP_EID_TABLE_MAP_DUMP, lisp_eid_table_map_dump)                     \
_(LISP_EID_TABLE_VNI_DUMP, lisp_eid_table_vni_dump)                     \
_(LISP_GPE_TUNNEL_DUMP, lisp_gpe_tunnel_dump)                           \
_(LISP_MAP_RESOLVER_DUMP, lisp_map_resolver_dump)                       \
_(LISP_LOCATOR_SET_DUMP, lisp_locator_set_dump)                         \
_(LISP_LOCATOR_SET_DUMP, lisp_locator_set_dump)                         \
_(IPSEC_GRE_ADD_DEL_TUNNEL, ipsec_gre_add_del_tunnel)                   \
_(IPSEC_GRE_TUNNEL_DUMP, ipsec_gre_tunnel_dump)                         \
_(DELETE_SUBIF, delete_subif)
  void
vl_msg_api_custom_dump_configure (api_main_t * am)
{
#define _(n,f) am->msg_print_handlers[VL_API_##n]       \
    = (void *) vl_api_##f##_t_print;
  foreach_custom_print_function;
#undef _
}

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
