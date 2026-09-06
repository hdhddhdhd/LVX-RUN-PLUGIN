/*===- runtime_api_v2.h - LVX runtime plugin ABI v2 (extension) -----------===//
// S21-6 part 2: ABI v1 stays frozen. Runtimes that also support
// chat-style sessions (context kept across turns, sampling options,
// reset/stop, stats) export an OPTIONAL v2 descriptor through its own
// query symbol. v2 descriptors duplicate the v1 callback set so a v2-only
// loader needs no v1 struct knowledge.
//
//   const LVXRuntimeDescriptorV2 *lvx_plugin_runtimes_v2(uint32_t *count);
//
// Session state lives inside the plugin; chat_turn() keeps conversation
// context until reset().
//===----------------------------------------------------------------------===*/
#ifndef LVX_RUNTIME_API_V2_H
#define LVX_RUNTIME_API_V2_H

#include "lvx/plugin_api.h"
#include "lvx/runtime_api.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_RUNTIME2_ABI_VERSION 3u
#define LVX_RUNTIME2_QUERY_SYMBOL "lvx_plugin_runtimes_v2"

/* v2 capability bits. */
enum {
  LVX_RT2_CHAT = 1ull << 0,       /* multi-turn chat_turn() */
  LVX_RT2_SAMPLING = 1ull << 1,   /* configure() temperature/top-k/top-p */
  LVX_RT2_RESET = 1ull << 2,      /* reset() */
  LVX_RT2_STATS = 1ull << 3       /* stats() */
};

typedef struct LVXBenchParams {
  uint32_t struct_size;      /* sizeof(LVXBenchParams) */
  int32_t tokens;            /* measured tokens (<=0: default 100) */
  int32_t warmup;            /* warmup tokens (<=0: default 10) */
  int32_t max_new;           /* cap for the run (<=0: 256) */
  uint32_t flags;            /* reserved, 0 */
} LVXBenchParams;

typedef struct LVXBenchResult {
  uint32_t struct_size;      /* sizeof(LVXBenchResult) */
  double ms_per_token;
  double tokens_per_sec;
  uint64_t tokens;
} LVXBenchResult;

typedef struct LVXSessionOptions {
  uint32_t struct_size;      /* sizeof(LVXSessionOptions) */
  float temperature;         /* <0: leave unchanged */
  int32_t top_k;             /* <0: leave unchanged */
  float top_p;               /* <0: leave unchanged */
  uint64_t seed;
  int32_t max_tokens;        /* <=0: leave unchanged */
  int32_t stop_token;        /* <0: leave unchanged */
  uint32_t flags;            /* reserved, 0 */
} LVXSessionOptions;

typedef struct LVXRuntimeStats {
  uint32_t struct_size;      /* sizeof(LVXRuntimeStats) */
  uint64_t tokens;
  double generation_ms;
  double last_step_ms;
} LVXRuntimeStats;

typedef struct LVXRuntimeDescriptorV2 {
  uint32_t abi_version;      /* LVX_RUNTIME2_ABI_VERSION */
  uint32_t struct_size;      /* sizeof(LVXRuntimeDescriptorV2) */
  const char *id;            /* same id as the v1 descriptor */
  const char *name;
  uint64_t caps_v2;          /* LVX_RT2_* */

  /* v1-equivalent callbacks (same semantics as runtime_api.h v1). */
  int (*probe)(const char *path, LVXRuntimeProbe *out);
  int (*create)(const char *path, const char *backend_hint,
                LVXRuntimeHandle **out, char *err, size_t errsz);
  void (*destroy)(LVXRuntimeHandle *h);
  int (*generate)(LVXRuntimeHandle *h, const char *prompt, char *out,
                  size_t outsz);

  /* Raw completion with sampling options (run path). Falls back to v1
     generate() semantics when opts->temperature < 0 (greedy). */
  int (*generate_ex)(LVXRuntimeHandle *h, const char *prompt,
                     const LVXSessionOptions *opts, char *out, size_t outsz);
  /* Per-token timing benchmark (decode loop inside the runtime; mean
     stats returned; no registry/plugin lookup in the loop). */
  int (*bench)(LVXRuntimeHandle *h, const LVXBenchParams *params,
               LVXBenchResult *out);
  /* v2 session callbacks. */
  int (*configure)(LVXRuntimeHandle *h, const LVXSessionOptions *opts);
  /* Multi-turn chat: appends the user turn, generates until stop/eos and
     returns the assistant text; context persists across calls. */
  int (*chat_turn)(LVXRuntimeHandle *h, const char *user, char *out,
                   size_t outsz);
  int (*reset)(LVXRuntimeHandle *h);
  int (*stats)(LVXRuntimeHandle *h, LVXRuntimeStats *out);
} LVXRuntimeDescriptorV2;

typedef const LVXRuntimeDescriptorV2 *(*LVXRuntimeV2QueryFn)(
    uint32_t *count);
/* ── optional v2 streaming extension (S2x) ─────────────────────────────────
   A runtime that can stream per-token chat output exports its OWN query
   symbol (same `id` as its v2 descriptor). The core chat UI prefers this;
   blocking chat_turn() remains the universal fallback. */
#define LVX_RUNTIME2S_ABI_VERSION 1u
#define LVX_RUNTIME2S_QUERY_SYMBOL "lvx_plugin_runtimes_v2s"

/* Delivers one decoded text piece (may be partial UTF-8 token text). */
typedef void (*LVXChatTokenFn)(const char *piece, uint32_t len, void *user);

typedef struct LVXRuntimeDescriptorV2S {
  uint32_t abi_version;      /* LVX_RUNTIME2S_ABI_VERSION */
  uint32_t struct_size;      /* sizeof(LVXRuntimeDescriptorV2S) */
  const char *id;            /* same id as the v2 descriptor */
  /* Streaming multi-turn chat: semantics identical to chat_turn(), but each
     decoded piece is pushed through onToken as it is produced. When `stop`
     is non-null the runtime polls it between tokens and aborts cleanly
     (returns LVX_OK with whatever was emitted so far). */
  int (*chat_turn_stream)(LVXRuntimeHandle *h, const char *user,
                          LVXChatTokenFn onToken, void *ud,
                          volatile int *stop);
} LVXRuntimeDescriptorV2S;

typedef const LVXRuntimeDescriptorV2S *(*LVXRuntimeV2SQueryFn)(
    uint32_t *count);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_RUNTIME_API_V2_H */
