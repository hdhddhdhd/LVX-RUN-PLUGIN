//===- demo_hello_plugin.c - minimal LVX plugin (source dist) ------------===//
// Build: gcc -shared -fPIC -O2 -I<lvx-include> demo_hello_plugin.c -o liblvx_demo_hello.so
#include "lvx/plugin_api.h"
#include <string.h>

static const LVXPluginDescriptor kDesc = {
    LVX_PLUGIN_ABI_VERSION, 1, "demo-hello",
    "Demo hello plugin distributed via the lvx-plugin repo (test)", NULL, 0};

extern const LVXPluginDescriptor *lvx_plugin_init(const LVXHostAPI *host) {
  (void)host;
  return &kDesc;
}
