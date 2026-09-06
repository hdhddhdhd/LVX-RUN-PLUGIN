/*===- plugin_api.h - LVX plugin ABI (C, versioned) ------------------------===//
// S20: the only long-term software extension boundary of LVX. Pure C ABI:
// fixed-width types, no STL, no exceptions across the boundary, so plugins
// may be built with different compilers/libstdc++ than the core.
//
// Lifecycle: dlopen -> dlsym(lvx_plugin_init) -> init(host) -> descriptor.
// The core validates abi_version == LVX_PLUGIN_ABI_VERSION; anything else
// is rejected with a warning and the plugin stays unloaded.
//===----------------------------------------------------------------------===*/
#ifndef LVX_PLUGIN_API_H
#define LVX_PLUGIN_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_PLUGIN_ABI_VERSION 1u
#define LVX_PLUGIN_INIT_SYMBOL "lvx_plugin_init"

/* Plugin capability bitmask (S20-B). Plugins declare what they provide;
   the core only uses this for listing, never for trust decisions. */
typedef enum LVXPluginCapability {
  LVX_PLUGIN_IMPORTER = 1u << 0,
  LVX_PLUGIN_EXPORTER = 1u << 1,
  LVX_PLUGIN_RUNTIME = 1u << 2,
  LVX_PLUGIN_TRAINER = 1u << 3,
  LVX_PLUGIN_COMMAND = 1u << 4,
  LVX_PLUGIN_FORMAT = 1u << 5,
  LVX_PLUGIN_TOKENIZER = 1u << 6,
  LVX_PLUGIN_SAMPLER = 1u << 7,
  LVX_PLUGIN_DEVICE = 1u << 8
} LVXPluginCapability;

/* Plugin error / status codes (never throw across the ABI). */
enum {
  LVX_OK = 0,
  LVX_ERROR = 1,
  LVX_INVALID_ARGUMENT = 2,
  LVX_UNSUPPORTED = 3,
  LVX_NOT_FOUND = 4,
  LVX_ABI_MISMATCH = 5,
  LVX_IO_ERROR = 6,
  LVX_CORRUPT = 7
};

/* Host services handed to every plugin at init time. All pointers must stay
   valid for the whole plugin lifetime. */
typedef struct LVXHostAPI {
  uint32_t abi_version;          /* LVX_PLUGIN_ABI_VERSION */
  void (*log)(int level, const char *message);
  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
} LVXHostAPI;

/* One registered command. execute() returns an LVX_* status code; errors are
   reported through the host log or the plugin's own stderr. */
typedef struct LVXCommandDescriptor {
  const char *name;        /* command name, e.g. "hello" (no spaces) */
  const char *description; /* one line for lvx help */
  int (*execute)(void *ctx, int argc, const char **argv);
} LVXCommandDescriptor;

/* Returned by lvx_plugin_init. Static storage inside the plugin. */
typedef struct LVXPluginDescriptor {
  uint32_t abi_version;        /* must equal LVX_PLUGIN_ABI_VERSION */
  uint32_t plugin_version;     /* plugin's own version */
  const char *name;            /* e.g. "hello" */
  const char *description;     /* one line */
  const LVXCommandDescriptor *commands;
  uint32_t command_count;
} LVXPluginDescriptor; /* ABI v1 struct: do NOT append fields */

/* Every plugin exports exactly one of these. */
typedef const LVXPluginDescriptor *(*LVXPluginInitFn)(const LVXHostAPI *host);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_PLUGIN_API_H */
