/*===- runtime_api.h - LVX runtime plugin ABI (C, versioned) --------------===//
// S21-1: runtimes (llama-cpu, gemma-vulkan, community runtimes, ...) are
// plugins. The core only discovers/validates/lists them and routes `lvx
// run/chat` to the plugin selected by probe(). Token loops run through
// function pointers cached at session creation - never through registries.
//
//   const LVXRuntimeDescriptor *lvx_plugin_runtimes(uint32_t *count);
//
// POD/C only; sessions are opaque handles owned by the plugin.
//===----------------------------------------------------------------------===*/
#ifndef LVX_RUNTIME_API_H
#define LVX_RUNTIME_API_H

#include "lvx/plugin_api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_RUNTIME_ABI_VERSION 1u
#define LVX_RUNTIME_QUERY_SYMBOL "lvx_plugin_runtimes"

/* Runtime capability bits (informational; probe() is authoritative). */
enum {
  LVX_RT_CPU = 1u << 0,
  LVX_RT_VULKAN = 1u << 1,
  LVX_RT_ROCM = 1u << 2,
  LVX_RT_CUDA = 1u << 3,
  LVX_RT_FP16 = 1u << 4,
  LVX_RT_BF16 = 1u << 5,
  LVX_RT_QUANT = 1u << 6, /* Q4_K/Q6_K style quantized weights */
  LVX_RT_KV_CACHE = 1u << 7,
  LVX_RT_STREAMING = 1u << 8,
  LVX_RT_LORA = 1u << 9,
  LVX_RT_TRAINING = 1u << 10
};

/* Session handle (opaque, plugin-allocated). */
typedef struct LVXRuntimeHandle LVXRuntimeHandle;

/* probe(): does this runtime understand `path`? */
typedef struct LVXRuntimeProbe {
  int confidence;            /* 0 = no, 100 = certain */
  const char *architecture;  /* e.g. "gemma4"/"llama" */
  const char *backend;       /* e.g. "vulkan"/"cpu" */
  const char *reason;
} LVXRuntimeProbe;

typedef struct LVXRuntimeDescriptor {
  uint32_t abi_version;      /* LVX_RUNTIME_ABI_VERSION */
  const char *id;            /* unique, e.g. "llama-cpu" */
  const char *name;          /* display name */
  uint64_t capabilities;     /* LVX_RT_* bitmask */

  int (*probe)(const char *path, LVXRuntimeProbe *out);

  /* Create one session for `path` (model load happens here; this is the
     only place plugin discovery is allowed - never inside a token loop).
     err/errsz receive a human-readable message on failure. */
  int (*create)(const char *path, const char *backend_hint,
                LVXRuntimeHandle **out, char *err, size_t errsz);
  void (*destroy)(LVXRuntimeHandle *h);

  /* Minimal blocking generate: prompt -> text into out[0..outsz). ABI v1
     baseline; streaming/cancel arrive with a later ABI version. */
  int (*generate)(LVXRuntimeHandle *h, const char *prompt, char *out,
                  size_t outsz);
} LVXRuntimeDescriptor;

typedef const LVXRuntimeDescriptor *(*LVXRuntimeQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_RUNTIME_API_H */
