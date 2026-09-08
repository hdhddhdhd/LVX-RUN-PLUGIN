# lvx-ai-dev (基础开发包 / base dev kit) — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：lvx-ai-dev 1.0.0，15 个文件（`lvx ls plugins -a` 可查）。安装：
`lvx update` 后 `lvx install lvx-ai-dev`（`lvx init` 命令已移除），是导入/执行/
分词/采样/训练等的基础能力。

**注册 id（实测 `lvx * list`）**：
- importers：`gguf`（.gguf）、`hf`（目录）、`onnx`、`pytorch`（bin/pt/pth）、`safetensors`
- executions：`cpu-exec` / `vulkan-exec`；backend：`vulkan-fake`
- tokenizer：`bpe`（格式 lvx-tokenizer-v1）；sampler：`default`
- trainers：`lora` / `pytorch` / `simple`；格式服务：liblvx_lvx.so（lvx2，`lvx info` 使用）

**调用方式**：
```sh
lvx convert model.gguf model.lvx        # 按扩展名自动 probe 导入器（lvx convert 查看）
lvx importer list | info <id> | probe <路径>
lvx execution list | info <id>
lvx backend list | devices vulkan-fake
lvx tokenizer list | info bpe
lvx sampler list | info default
lvx trainer list | info <id>
lvx info model.lvx                      # 走 lvx2 格式服务
```

**给插件开发者**：
- 模型运行时无需自己解析 GGUF/HF——上游转好 `.lvx` 后你的 runtime 只面对
  LVXLoader（mmap 张量视图）。
- `liblvx_vulkan_engine_edit.so` 随 base 安装、无独立 CLI；其能力由引擎侧使用，
  不要臆测调用方式。
- 想注册自己的导入器/执行器：照 importer/execution API 头写插件即可，probe 打分
  决定接管。


## English

**Package**: lvx-ai-dev 1.0.0, 15 files (see `lvx ls plugins -a`). Install with
`lvx update` then `lvx install lvx-ai-dev` (`lvx init` was removed); it is the
base capability set.

**Registry ids (measured via `lvx * list`)**:
- importers: `gguf` (.gguf), `hf` (directory), `onnx`, `pytorch` (bin/pt/pth), `safetensors`
- executions: `cpu-exec` / `vulkan-exec`; backend: `vulkan-fake`
- tokenizer: `bpe` (format lvx-tokenizer-v1); sampler: `default`
- trainers: `lora` / `pytorch` / `simple`; format service: liblvx_lvx.so (lvx2, used by `lvx info`)

**Invocation**:
```sh
lvx convert model.gguf model.lvx        # importer auto-probed by extension
lvx importer list | info <id> | probe <path>
lvx execution list | info <id>
lvx backend list | devices vulkan-fake
lvx tokenizer list | info bpe
lvx sampler list | info default
lvx trainer list | info <id>
lvx info model.lvx                      # via the lvx2 format service
```

**For plugin developers**:
- Runtimes never parse GGUF/HF themselves — consume upstream `.lvx` through
  LVXLoader (mmap tensor views).
- `liblvx_vulkan_engine_edit.so` ships with base and has no standalone CLI; it is
  used by engines — do not assume a call surface.
- To register your own importer/execution: implement the matching API header;
  the probe score decides ownership.

