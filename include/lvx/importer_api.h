/*===- importer_api.h - LVX importer plugin ABI (C, versioned) ------------===//
// S20-B: importers turn external model sources into the unified ModelIR the
// LVX writer consumes. Importers are OPTIONAL capabilities of plugins: a
// plugin may export zero or more LVXImporterDescriptor entries through the
// optional query symbol lvx_plugin_importers().
//
//   const LVXImporterDescriptor *lvx_plugin_importers(uint32_t *count);
//
// A plugin that does not export that symbol simply has no importers; the
// hello plugin is unaffected. Everything here is POD/C; no STL, no C++
// exceptions across the boundary.
//===----------------------------------------------------------------------===*/
#ifndef LVX_IMPORTER_API_H
#define LVX_IMPORTER_API_H

#include "lvx/plugin_api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_IMPORTER_ABI_VERSION 1u
#define LVX_IMPORTER_QUERY_SYMBOL "lvx_plugin_importers"

/* Importer kinds (registry listing + routing hints). */
enum {
  LVX_IMPORTER_WEIGHTS = 1u << 0, /* bare weight file (gguf/safetensors) */
  LVX_IMPORTER_DIRECTORY = 1u << 1, /* HF-style model directory */
  LVX_IMPORTER_CHECKPOINT = 1u << 2, /* torch checkpoint etc. */
  LVX_IMPORTER_GRAPH = 1u << 3,   /* ONNX-style graph model */
};

/* probe(): how well this importer understands `path`. */
typedef struct LVXProbeResult {
  int confidence;          /* 0 = not mine; 100 = certain */
  const char *format;      /* static string, e.g. "gguf" */
  const char *architecture; /* e.g. "llama" (empty "" when unknown) */
  const char *reason;      /* human-readable one-liner */
} LVXProbeResult;

/* inspect(): summary of what import would produce (S20-B: minimal). */
typedef struct LVXImportInfo {
  const char *format;
  const char *architecture;
  const char *tensor_count_hint; /* static numeric string or "" */
} LVXImportInfo;

/* Opaque import context; defined by the convert layer in a later stage. */
typedef struct LVXImportContext LVXImportContext;

typedef struct LVXImporterDescriptor {
  uint32_t abi_version;    /* LVX_IMPORTER_ABI_VERSION */
  const char *id;          /* unique id, e.g. "gguf" */
  const char *name;        /* display name */
  uint64_t capabilities;   /* LVX_IMPORTER_* flags */
  const char *extensions;  /* comma-separated hint list, e.g. "gguf" */

  int (*probe)(const char *path, LVXProbeResult *out);
  int (*inspect)(const char *path, LVXImportInfo *info);
  int (*import)(const char *path, LVXImportContext *ctx);
} LVXImporterDescriptor;

/* Optional plugin export: returns a static array of descriptors. */
typedef const LVXImporterDescriptor *(*LVXImporterQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_IMPORTER_API_H */
