/*===- capability_api.h - plugin capability channel (S23-7) ----------------===//
// Optional PLUGIN-LEVEL capability export. Kind-specific capabilities
// already live on each descriptor (LVX_BACKEND_*, LVX_RT_*, LVX_TOKENIZER_*,
// LVX_SAMPLER_*, LVX_TRAINER_*, LVX_IMPORTER_*, ...). This channel adds a
// dynamic, plugin-wide mask for abilities that are not tied to one
// descriptor kind (e.g. "torch present", "fake"), evaluated at load time:
//
//   uint64_t lvx_plugin_capabilities(void);
//
// Bits below 16 follow the LVXPluginCapability kind enum; bits >= 16 are
// free-form plugin abilities. New ability bits are additive; no frozen v1
// struct is touched (this is a new optional query symbol).
//===----------------------------------------------------------------------===*/
#ifndef LVX_CAPABILITY_API_H
#define LVX_CAPABILITY_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_CAPABILITY_ABI_VERSION 1u
#define LVX_CAPABILITY_QUERY_SYMBOL "lvx_plugin_capabilities"

typedef uint64_t (*LVXCapabilityFn)(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_CAPABILITY_API_H */
