/*===- format_api.h - LVX format service plugin ABI (C, versioned) --------===//
// S20-C: the LVX file format (header/sections/tensor table/CRC validation)
// is a plugin capability. Core routes `lvx lvx <subcmd> <file>` to this
// plugin; future consumers (convert/run) will reach the format through the
// same optional query symbol instead of parsing files themselves.
//
//   const LVXFormatDescriptor *lvx_plugin_format(uint32_t *count);
//
// POD/C only; handles are opaque; strings returned by the service point
// into the mmap region owned by the handle (valid until close()).
//===----------------------------------------------------------------------===*/
#ifndef LVX_FORMAT_API_H
#define LVX_FORMAT_API_H

#include "lvx/plugin_api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_FORMAT_ABI_VERSION 1u
#define LVX_FORMAT_QUERY_SYMBOL "lvx_plugin_format"

typedef struct LVXFormatHandle LVXFormatHandle;

/* Static file facts (LVX v2 header fields). */
typedef struct LVXFormatInfo {
  char magic[4];
  uint32_t version;
  uint32_t section_count;
  uint32_t tensor_count;
  uint64_t file_size;
} LVXFormatInfo;

typedef struct LVXSectionInfo {
  uint32_t type;
  uint64_t offset; /* file offset */
  uint64_t size;
} LVXSectionInfo;

typedef struct LVXTensorInfo {
  const char *name; /* into the mmap (valid until close) */
  uint16_t dtype;
  uint16_t ndim;
  uint64_t shape[4];
  uint64_t offset; /* relative to WEIGHT_DATA start */
  uint64_t size;
} LVXTensorInfo;

/* One format service implementation (usually exactly one per plugin). */
typedef struct LVXFormatDescriptor {
  uint32_t abi_version; /* LVX_FORMAT_ABI_VERSION */
  const char *id;       /* e.g. "lvx2" */
  const char *name;

  /* Opens for read (mmap). Returns LVX_OK + *out on success. */
  int (*open)(const char *path, LVXFormatHandle **out);
  void (*close)(LVXFormatHandle *h);

  /* Header facts. */
  int (*info)(LVXFormatHandle *h, LVXFormatInfo *out);
  /* section_count/type/offset/size; idx < info().section_count. */
  int (*section)(LVXFormatHandle *h, uint32_t idx, LVXSectionInfo *out);
  /* Tensor table entries; idx < info().tensor_count. */
  int (*tensor)(LVXFormatHandle *h, uint32_t idx, LVXTensorInfo *out);
  /* Full structural check incl. header+section-table CRC. */
  int (*validate)(const char *path);
} LVXFormatDescriptor;

/* Optional plugin export. */
typedef const LVXFormatDescriptor *(*LVXFormatQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_FORMAT_API_H */
