/*===- backend_api.h - LVX backend plugin ABI (C, versioned) -------------===//
// S21-4: compute backends (vulkan/cuda/rocm/cpu) are plugins. ABI v1 covers
// DISCOVERY + CAPABILITY + DEVICE ENUMERATION + CREATE/DESTROY only - no
// kernel/execution calls: execution models differ per vendor and belong to
// later execution-context ABIs. All vendor types (Vk*, CU*, HIP*) stay
// inside plugins.
//
//   const LVXBackendDescriptor *lvx_plugin_backends(uint32_t *count);
//
// POD/C only; caller-owned buffers where noted; plugin-owned opaque handles.
//===----------------------------------------------------------------------===*/
#ifndef LVX_BACKEND_API_H
#define LVX_BACKEND_API_H

#include "lvx/plugin_api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_BACKEND_ABI_VERSION 1u
#define LVX_BACKEND_QUERY_SYMBOL "lvx_plugin_backends"

/* Backend capability flags (declared capabilities must be real). */
enum {
  LVX_BACKEND_FAKE = 1ull << 0,   /* test/dummy backend (no compute) */
  LVX_BACKEND_CPU = 1ull << 1,
  LVX_BACKEND_VULKAN = 1ull << 2,
  LVX_BACKEND_CUDA = 1ull << 3,
  LVX_BACKEND_ROCM = 1ull << 4
};

typedef struct LVXBackendOpaque LVXBackendOpaque;
typedef LVXBackendOpaque *LVXBackendHandle;

/* One device as seen through the backend (no vendor types leak here). */
typedef struct LVXBackendDeviceInfo {
  uint32_t struct_size;        /* sizeof(LVXBackendDeviceInfo) */
  uint32_t device_index;
  const char *id;              /* plugin-owned static string */
  const char *name;
  uint64_t vendor_id;
  uint64_t device_id;
  uint64_t memory_bytes;
  uint32_t api_major;
  uint32_t api_minor;
  uint64_t capabilities;
} LVXBackendDeviceInfo;

/* Caller-allocated list: first call with items=NULL fills count; second
   call with items/capacity fills the entries (capacity >= count). */
typedef struct LVXBackendDeviceList {
  uint32_t count;
  uint32_t capacity;
  LVXBackendDeviceInfo *items;
} LVXBackendDeviceList;

typedef struct LVXBackendCreateInfo {
  uint32_t struct_size;        /* sizeof(LVXBackendCreateInfo) */
  uint32_t device_index;       /* index from enumerate_devices */
  uint64_t capabilities;       /* requested subset (informational) */
} LVXBackendCreateInfo;

typedef struct LVXBackendDescriptor {
  uint32_t abi_version;        /* LVX_BACKEND_ABI_VERSION */
  uint32_t struct_size;        /* sizeof(LVXBackendDescriptor) */
  const char *id;              /* unique, e.g. "vulkan-fake" */
  const char *name;
  const char *version;         /* backend implementation version string */
  uint64_t capabilities;       /* LVX_BACKEND_* bitmask */

  /* Backend availability (driver present etc.). host may be NULL for
     backends that do not need host services. */
  int (*probe)(const LVXHostAPI *host);
  int (*enumerate_devices)(LVXBackendDeviceList *out);
  int (*create)(const LVXBackendCreateInfo *info, LVXBackendHandle *out,
                char *err, size_t err_size);
  void (*destroy)(LVXBackendHandle backend);
  int (*get_device_info)(LVXBackendHandle backend,
                         LVXBackendDeviceInfo *out);
} LVXBackendDescriptor;

typedef const LVXBackendDescriptor *(*LVXBackendQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_BACKEND_API_H */
