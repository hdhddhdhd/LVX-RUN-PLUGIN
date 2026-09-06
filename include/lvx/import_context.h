/*===- import_context.h - LVX importer import context (S22-4) --------------===//
// First concrete definition of the LVXImportContext type that importer
// ABI v1 left opaque ("defined by the convert layer in a later stage").
// This is a NEW header, not an edit of the frozen importer_api.h: the
// descriptor structs and query symbols of ABI v1 are untouched.
//
// The core convert router fills one of these and hands it to
// descriptor->import(path, ctx). Plugins read dst_path and write their
// output there; error text goes into err/errsz (when non-null).
//===----------------------------------------------------------------------===*/
#ifndef LVX_IMPORT_CONTEXT_H
#define LVX_IMPORT_CONTEXT_H

#include "lvx/importer_api.h" /* LVXImportContext tag forward + LVX_* codes */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct LVXImportContext {
  uint32_t struct_size;     /* sizeof(LVXImportContext) */
  const char *dst_path;     /* required: output .lvx path */
  char *err;                /* optional: plugin writes failure text here */
  size_t errsz;             /* capacity of err */
  uint64_t *out_tensor_count; /* optional: filled on success */
  uint64_t *out_bytes;        /* optional: output file size on success */
  uint32_t flags;             /* reserved, must be 0 */
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_IMPORT_CONTEXT_H */
