/*===- trainer_api.h - LVX trainer plugin ABI (C, versioned) ---------------===//
// S23-3: trainers adapt a model to data (tune). Training is fully separate
// from the Runtime ABI (§37). One tune() call runs the whole adaptation
// and produces a NEW LVX file; the input model is only ever read (mmap),
// the output is written atomically (temp file + rename) by the trainer.
//
//   const LVXTrainerDescriptor *lvx_plugin_trainers(uint32_t *count);
//===----------------------------------------------------------------------===*/
#ifndef LVX_TRAINER_API_H
#define LVX_TRAINER_API_H

#include "lvx/plugin_api.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVX_TRAINER_ABI_VERSION 1u
#define LVX_TRAINER_QUERY_SYMBOL "lvx_plugin_trainers"

/* Trainer kinds. */
enum {
  LVX_TRAINER_SIMPLE = 1ull << 0,  /* deterministic low-resource tune */
  LVX_TRAINER_LORA = 1ull << 1,
  LVX_TRAINER_PYTORCH = 1ull << 2  /* external torch, plugin-owned */
};

/* probe(): can this trainer adapt `model_path`? */
typedef struct LVXTrainerProbe {
  int confidence;            /* 0 = no; 100 = certain */
  const char *kind;          /* static string, e.g. "simple" */
  const char *reason;
} LVXTrainerProbe;

/* Everything one tune() run needs (definition reserved by ABI v1; see
   import_context.h for the same pattern). */
struct LVXTuneContext {
  uint32_t struct_size;      /* sizeof(LVXTuneContext) */
  const char *model_path;    /* input .lvx (read-only) */
  const char *data_path;     /* training text file */
  const char *output_path;   /* atomic output .lvx */
  char *err;                 /* optional detail buffer */
  size_t errsz;
  uint64_t *out_tensors_written; /* optional stats */
  uint32_t flags;            /* reserved */
};

typedef struct LVXTrainerDescriptor {
  uint32_t abi_version;      /* LVX_TRAINER_ABI_VERSION */
  uint32_t struct_size;      /* sizeof(LVXTrainerDescriptor) */
  const char *id;            /* e.g. "simple" */
  const char *name;
  uint64_t capabilities;     /* LVX_TRAINER_* flags */

  int (*probe)(const char *model_path, LVXTrainerProbe *out);
  int (*tune)(struct LVXTuneContext *ctx);
} LVXTrainerDescriptor;

typedef const LVXTrainerDescriptor *(*LVXTrainerQueryFn)(uint32_t *count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVX_TRAINER_API_H */
