
# Copyright (c) <current-year> <your-organization>
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CFLAGS = -Wall -I@TOOLKIT_INCLUDE@ @DPDK@

lib_LTLIBRARIES = snat_plugin.la snat_test_plugin.la

snat_plugin_la_SOURCES = snat/snat.c		\
        snat/in2out.c				\
        snat/out2in.c				\
	snat/snat_plugin.api.h 

snat_plugin_la_LDFLAGS = -module

BUILT_SOURCES = snat/snat.api.h

SUFFIXES = .api.h .api

%.api.h: %.api
	mkdir -p `dirname $@` ; \
	$(CC) $(CPPFLAGS) -E -P -C -x c $^ \
	| vppapigen --input - --output $@ --show-name $@

nobase_include_HEADERS =			\
  snat/snat_all_api_h.h			\
  snat/snat_msg_enum.h			\
  snat/snat.api.h

snat_test_plugin_la_SOURCES = \
  snat/snat_test.c snat/snat_plugin.api.h
snat_test_plugin_la_LDFLAGS = -module

if WITH_PLUGIN_TOOLKIT
install-data-hook:
	mkdir /usr/lib/vpp_plugins || true
	mkdir /usr/lib/vpp_api_test_plugins || true
	cp -L $(prefix)/lib/snat_plugin.so /usr/lib/vpp_plugins
	cp -L $(prefix)/lib/snat_test_plugin.so \
		/usr/lib/vpp_api_test_plugins
	rm -f $(prefix)/lib/snat_plugin.*
	rm -f $(prefix)/lib/snat_test_plugin.*
endif