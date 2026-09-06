/*===- execution_api.h - LVX execution plugin ABI (C, versioned) ----------===//
// S22-1: execution = buffer/tensor lifecycle + transfer + sync on top of a
// chosen backend device. No kernel dispatch in v1 (vendor execution models
// differ; runtimes keep their own dispatch internally - see arch note).
//
//   const LVXExecutionDescriptor *lvx_plugin_executions(uint32_t *count);
//
// POD/C only; buffers are opaque handles owned by the plugin; upload/
// download use caller-owned host memory.
//===----------------------------------------------------------------------===*/
#ifndef LVX_EXECUTION_API_H
#define LVX_EXECUTION_API_H

#include "lvx/plugin_api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_EXECUTION_ABI_VERSION 1u
#define LVX_EXECUTION_QUERY_SYMBOL "lvx_plugin_executions"

typedef struct LVXExecOpaque LVXExecOpaque;
typedef LVXExecOpaque *LVXExecHandle;        /* execution instance */
typedef struct LVXBufferOpaque LVXBufferOpaque;
typedef LVXBufferOpaque *LVXBufferHandle;    /* device buffer */

typedef struct LVXExecCreateInfo {
  uint32_t struct_size;      /* sizeof(LVXExecCreateInfo) */
  const char *backend_id;    /* e.g. "cpu"/"vulkan" (informational) */
  uint32_t device_index;
  uint64_t capabilities;     /* requested subset (informational) */
} LVXExecCreateInfo;

typedef struct LVXExecutionDescriptor {
  uint32_t abi_version;      /* LVX_EXECUTION_ABI_VERSION */
  uint32_t struct_size;      /* sizeof(LVXExecutionDescriptor) */
  const char *id;            /* unique, e.g. "cpu-exec" */
  const char *name;
  uint64_t capabilities;     /* LVX_BACKEND_* style bits */

  int (*probe)(const LVXHostAPI *host);
  int (*create)(const LVXExecCreateInfo *info, LVXExecHandle *out,
                char *err, size_t errsz);
  void (*destroy)(LVXExecHandle h);

  int (*allocate_buffer)(LVXExecHandle h, uint64_t bytes,
                         LVXBufferHandle *out, char *err, size_t errsz);
  void (*free_buffer)(LVXExecHandle h, LVXBufferHandle b);
  int (*upload)(LVXExecHandle h, LVXBufferHandle b, const void *src,
                uint64_t size, uint64_t offset);
  int (*download)(LVXExecHandle h, LVXBufferHandle b, void *dst,
                  uint64_t size, uint64_t offset);
  int (*sync)(LVXExecHandle h); /* wait for pending transfers */
} LVXExecutionDescriptor;

typedef const LVXExecutionDescriptor *(*LVXExecQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_EXECUTION_API_H */
