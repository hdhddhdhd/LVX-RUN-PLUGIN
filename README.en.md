# LVX-RUN-PLUGIN — LVX Plugin Development Tutorial & Test Plugin Repository

> [中文](README.md) | **English**

This repository plays two roles: a **plugin development tutorial** (this README) and an **lvx-plugin test plugin** (topic: `lvx-plugin`, automatically collected by `lvx update`).

- ABI header snapshot: `include/lvx/*.h`
- Compilable example: `demo_hello_plugin.c` (repo root) + prebuilt `lvx-plugin/liblvx_demo_hello.so`
- The repository itself is a live example of the official source mechanism.

Also see: **[official plugin call guides (EN/中文)](plugins-guide/README.md)** — how every official plugin is invoked.

---

# LVX Plugin Development Tutorial

## 1. Concepts

A plugin is a `liblvx_*.so` shared library exporting several **extern "C"** query symbols. The loader (PluginManager) discovers capabilities by symbol:

| Capability | Query symbol | Header |
|---|---|---|
| Plugin itself (required) | `lvx_plugin_init` | plugin_api.h |
| Runtime v1 (decoding) | `lvx_plugin_runtimes` | runtime_api.h |
| Runtime v2 (session/chat) | `lvx_plugin_runtimes_v2` | runtime_api_v2.h |
| Streaming chat extension | `lvx_plugin_runtimes_v2s` | runtime_api_v2.h |
| Importer | `lvx_plugin_importers` | importer_api.h |
| Others (backend/execution/tokenizer/sampler/trainer/format…) | see the matching lvx/*.h | |

ABI principle: **only extern-C POD crosses the boundary**; structs are frozen, optional capabilities are added via extra query symbols.

## 2. Minimal plugin (see demo_hello_plugin.c)

```c
#include "lvx/plugin_api.h"

static const LVXPluginDescriptor kDesc = {
    LVX_PLUGIN_ABI_VERSION, 1, "my-plugin",
    "My first plugin", NULL, 0};

extern const LVXPluginDescriptor *lvx_plugin_init(const LVXHostAPI *host) {
  (void)host;
  return &kDesc;
}
```

Compile (the file must be named `liblvx_*.so` to be loaded):

```sh
gcc -shared -fPIC -O2 -I include demo_hello_plugin.c -o liblvx_my_plugin.so
```

### Making a real runtime (that runs models)

A runtime must export the v1 (and, if useful, v2) descriptor. v1 minimum:

```c
int probe(const char *path, LVXRuntimeProbe *out);      // 0..100 score to claim a model
int create(const char *path, const char *backend_hint,
           LVXRuntimeHandle **out, char *err, size_t errsz); // load the model, create a session
void destroy(LVXRuntimeHandle *h);
int generate(LVXRuntimeHandle *h, const char *prompt, char *out, size_t outsz);
```

v2 adds session/sampling/multi-turn: `generate_ex / bench / configure / chat_turn / reset / stats` (optional v2s provides per-token streaming). Field layout must strictly follow `include/lvx/runtime_api*.h` — do not invent your own.

> **S28 session common-options extension**: `LVXSessionOptions` gained trailing fields `ctx_len` (context cap),
> `kv_cache_type` (0=f16/1=q8_0/2=q4_0) and `system_prompt` (UTF-8; valid for the duration of the call — the
> runtime must copy it). New fields must be read behind a `struct_size >= sizeof(LVXSessionOptions)` gate: an
> older core sends the old struct_size, and a new runtime should stay compatible (read only the base fields),
> while older runtimes naturally ignore the tail because of their own old sizeof. Create-time options
> (ctx/kv/layer plan/GPU device index) are carried by the core as a `backend_hint` suffix:
> `vulkan;ctx=8192;kv=q8_0;plan=g0:20,c0:all` (`g0:20` = 20 layers on GPU 0 from the start, `c0:all` = the rest
> on CPU 0; runtimes that do not understand the `;` suffix simply treat the whole hint as a backend name).

## 3. Package manifest (manifest.json)

The default branch must contain a `lvx-plugin/` directory with `manifest.json`:

```json
{
  "name": "my-plugin",
  "version": "1.0.0",
  "desc": "describe here",
  "dependencies": [],
  "files": [
    {"file": "liblvx_my_plugin.so",
     "sha256": "<sha256 of the file>",
     "size": 12345}
  ]
}
```

- `file`: the file name after install; must start with `liblvx_`/`lvx_` and end with `.so`; `/` and `..` are forbidden
- `sha256`/`size`: verified at install time — always generate them with `sha256sum`
- `dependencies`: may depend on other packages, e.g. `[{"name":"qwen35-run","version":">=1.0.0"}]`; versions support bare, `>=`, `>`, `==`
- A repository may also act as a multi-package index: wrap entries as `{"packages":[ {...as above} ]}`

The official (static) source puts its manifest at the repository root; GitHub plugin repos put it at `lvx-plugin/manifest.json` with the files in the same directory.

## 4. Publishing & installing

1. Push your code to the default branch (main recommended)
2. Repository settings → Topics → add **`lvx-plugin`** (the only condition for auto-collection)
3. Users get it automatically:

```sh
lvx update                 # refresh the package list (your repo appears)
lvx install my-plugin      # install by package name
lvx install demo           # file-name / substring matching also works
lvx plugin ls              # view installed packages & versions (same as `lvx ls plugins`)
lvx ls devices             # view CPU/GPU device indices (for the -g/-cpu options)
lvx plugins                # view loaded (running) plugins
```

## 5. Testing tips

- Local load test: `LVX_PLUGIN_PATH=/path/to/dir lvx plugins` (only `liblvx_*`/`lvx_* .so` files are scanned)
- Manifest check: after changing a file, `lvx install` should report a sha256 mismatch (verifies checksums work)
- Runtime numerics: gate against llama.cpp with per-token logits correlation ≥0.99 on the same input before releasing (see the qwen35-run delivery standard)
- Version bumps: change `version`, then re-run `lvx update && lvx install <name>`

## 6. This repository is the tutorial example

`lvx-demo-hello` v0.2.0 was published with exactly this flow: source at the repo root, manifest in `lvx-plugin/`, topic applied. Fork it to get started.

---

# Official fine-tune plugin: tune_vulkan (quick start)

Main repo release page (README): https://github.com/hdhddhdhd/LVX-RUN

```sh
lvx install tune_vulkan
lvx tune --trainer tune_vulkan model.lvx data.txt tuned.lvx
```

- `data.txt` supports three forms: plain text (one sequence per line), multi-line dialogue blocks (blank-line separated, formatted to the model chat template) and token-id lines (legacy)
- Trains on a **discrete GPU** by default (skips iGPUs; falls back to CPU without one); `-cpu` forces CPU
- rank-8 LoRA over q/k/v/o + ffn gate/up/down projections, BF16 merged write-back; the output works directly with `lvx run`/`lvx chat`
- Package management: `lvx remove <pkg>` to uninstall, `lvx upgrade [pkg]` to upgrade, `lvx plugin ls [-a]` to inspect (-a expands files)

## Single-file repo

This repository distributes as **one .lvx in the root**: `plugins.lvx` is the
only machine-facing artifact — plugin binaries, version registry (manifest
JSON) are all sealed inside (LVX Universal Container, `kind=repo`).

- Install directly:
  ```
  lvx install plugins.lvx
  lvx install /path/to/plugins.lvx
  ```
- GitHub topic sources (`github-topic=lvx-plugin`) try the root
  `plugins.lvx` container first; legacy `lvx-plugin/manifest.json` is the
  fallback channel.
- Inspect: `lvx type plugins.lvx` (kind + section table).
- Repack after editing `plugins.spec`: `lvx_pack plugins.spec plugins.lvx`.

## Store metadata

The repository keeps a **store layer** separate from the install layer for a
future software store:

- Installation only reads the root `plugins.lvx` (single-file container).
- **`store.json`** — store-facing metadata per app/plugin: version, title,
  summary & long description, author, tags, `package` (the .lvx), `media`
  references, release notes.
- **`files/`** — showcase media: icon (`icon.png`), screenshots
  (`screenshots/*.png`), promo video (`demo.mp4`). Display only.

To add an app: add an `apps[]` entry + `files/` assets; repack the install
container with `lvx_pack plugins.spec plugins.lvx`.
