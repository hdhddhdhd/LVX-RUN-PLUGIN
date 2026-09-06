/*===- sampler_api.h - LVX sampler plugin ABI (C, versioned) ---------------===//
// S23-2: sampler = next-token selection from logits. Parameters cross the
// ABI as a frozen POD struct; logits/ids cross as caller-owned pointers;
// the token comes back by value. History (for repetition penalties) is
// passed per call by the caller (the runtime owns it).
//
//   const LVXSamplerDescriptor *lvx_plugin_samplers(uint32_t *count);
//
// Determinism: a sampler created with seed S draws the exact same token
// sequence as the in-process core xir::Sampler seeded with S (single
// shared implementation compiled into the plugin). set_seed() reseeds.
//===----------------------------------------------------------------------===*/
#ifndef LVX_SAMPLER_API_H
#define LVX_SAMPLER_API_H

#include "lvx/plugin_api.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_SAMPLER_ABI_VERSION 1u
#define LVX_SAMPLER_QUERY_SYMBOL "lvx_plugin_samplers"

/* Capability bits: which knobs the sampler implements. */
enum {
  LVX_SAMPLER_TEMPERATURE = 1ull << 0,
  LVX_SAMPLER_TOP_K = 1ull << 1,
  LVX_SAMPLER_TOP_P = 1ull << 2,
  LVX_SAMPLER_MIN_P = 1ull << 3,
  LVX_SAMPLER_REPETITION = 1ull << 4,
  LVX_SAMPLER_GREEDY = 1ull << 5
};

/* Frozen parameter POD (see golden tests for the layout). */
typedef struct LVXSamplerParams {
  uint32_t struct_size;        /* sizeof(LVXSamplerParams) */
  uint32_t flags;              /* reserved, must be 0 */
  float temperature;           /* 0 = greedy (top-k/top-p ignored) */
  float top_p;                 /* <= 0 or >= 1 disables */
  float min_p;                 /* 0 disables */
  float repetition_penalty;    /* 1.0 disables; > 1 penalizes repeats */
  int32_t top_k;               /* <= 0 disables */
  uint32_t repeat_last_n;      /* history window for the penalty */
  uint64_t seed;
  float presence_penalty;      /* -logit * presence per seen token */
  float frequency_penalty;     /* -logit * frequency * count */
} LVXSamplerParams;

typedef struct LVXSamplerOpaque LVXSamplerOpaque;
typedef LVXSamplerOpaque *LVXSamplerHandle;

typedef struct LVXSamplerDescriptor {
  uint32_t abi_version;      /* LVX_SAMPLER_ABI_VERSION */
  uint32_t struct_size;      /* sizeof(LVXSamplerDescriptor) */
  const char *id;            /* e.g. "default" */
  const char *name;
  uint64_t capabilities;     /* LVX_SAMPLER_* bits */

  int (*create)(const LVXSamplerParams *params, LVXSamplerHandle *out,
                char *err, size_t errsz);
  void (*destroy)(LVXSamplerHandle h);
  /* Samples one token; history holds the recent ids for repetition
     penalties (may be NULL/0). Token comes back by value. */
  int (*sample)(LVXSamplerHandle h, const float *logits, size_t vocab,
                const int32_t *history, size_t history_len, int32_t *token);
  /* Deterministic argmax (no RNG, ignores config). */
  int (*greedy)(LVXSamplerHandle h, const float *logits, size_t vocab,
                int32_t *token);
  /* Reseeds the RNG deterministically. */
  int (*set_seed)(LVXSamplerHandle h, uint64_t seed);
} LVXSamplerDescriptor;

typedef const LVXSamplerDescriptor *(*LVXSamplerQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_SAMPLER_API_H */
