/*===- tokenizer_api.h - LVX tokenizer plugin ABI (C, versioned) -----------===//
// S23-1: tokenizer = text <-> token ids for one model vocabulary. A
// tokenizer instance is created from a "tokenizer blob" (a deterministic
// serialization of the model's tokenizer data - see xir/tokenizer_codec.h
// for the reference codec; both sides compile that header, no C++ crosses
// this boundary).
//
//   const LVXTokenizerDescriptor *lvx_plugin_tokenizers(uint32_t *count);
//
// Ownership: ids/text returned by encode/decode/chat_template point into
// per-handle arena storage owned by the plugin; they stay valid until the
// next call on the same handle or destroy(). Text in is caller-owned.
//===----------------------------------------------------------------------===*/
#ifndef LVX_TOKENIZER_API_H
#define LVX_TOKENIZER_API_H

#include "lvx/plugin_api.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_TOKENIZER_ABI_VERSION 1u
#define LVX_TOKENIZER_QUERY_SYMBOL "lvx_plugin_tokenizers"

/* Tokenizer kind flags. */
enum {
  LVX_TOKENIZER_BPE = 1ull << 0,       /* byte-pair / GGUF tokenizer.ggml */
  LVX_TOKENIZER_SENTENCEPIECE = 1ull << 1,
  LVX_TOKENIZER_TIKTOKEN = 1ull << 2
};

typedef struct LVXTokenizerOpaque LVXTokenizerOpaque;
typedef LVXTokenizerOpaque *LVXTokenizerHandle;

typedef struct LVXTokenizerDescriptor {
  uint32_t abi_version;       /* LVX_TOKENIZER_ABI_VERSION */
  uint32_t struct_size;       /* sizeof(LVXTokenizerDescriptor) */
  const char *id;             /* unique, e.g. "bpe" */
  const char *name;
  uint64_t capabilities;      /* LVX_TOKENIZER_* flags */
  const char *format;         /* blob format id, e.g. "lvx-tokenizer-v1" */

  /* Creates a tokenizer over a decoded blob; err receives detail. */
  int (*create_from_blob)(const uint8_t *blob, size_t blob_size,
                          LVXTokenizerHandle *out, char *err, size_t errsz);
  void (*destroy)(LVXTokenizerHandle h);

  /* Arena-owned outputs (see header comment). LVX_OK on success. */
  int (*encode)(LVXTokenizerHandle h, const char *text, const int32_t **ids,
                size_t *count);
  int (*decode)(LVXTokenizerHandle h, const int32_t *ids, size_t count,
                const char **text);
  int (*token_count)(LVXTokenizerHandle h, const char *text, size_t *out);

  size_t (*vocab_size)(LVXTokenizerHandle h);
  const char *(*chat_template)(LVXTokenizerHandle h);
} LVXTokenizerDescriptor;

typedef const LVXTokenizerDescriptor *(*LVXTokenizerQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_TOKENIZER_API_H */
