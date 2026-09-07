# Official LVX Plugin Call Guides / 官方 LVX 插件调用指南

> Language: the main repos switch between [中文](README.md) / [English](../README.en.md).
> 本目录为插件开发者提供"官方插件如何被调用"的双语参考，防止重复造轮子或误调。
>
> English: This directory documents how every official plugin is invoked, so
> plugin developers can reuse or integrate them correctly.

## 中文

官方源（`http://47.79.21.231:8080`，12 个包）插件没有跨插件的导出 C API —— 宿主
统一管理 registry（runtime/importer/execution/tokenizer/sampler/trainer/backend）。
"调用官方插件"的三种正确姿势：

1. **用户/脚本级调用**：官方 CLI 命令（`lvx run/chat/bench/tune/api/convert` 等）由
   宿主自动路由到对应插件；你自己的工具用同款命令即可（见各篇）。
2. **注册表协作**：在 `lvx <kind> list|info <id>` 中查看官方插件注册的 id；你的插件
   通过 manifest `dependencies` 声明依赖官方包，保证用户环境已装。
3. **能力边界**：官方 runtime 按模型架构 probe 自动接管（`lvx runtime probe model.lvx`
   可查）；无 Vulkan 设备时 GPU runtime 自动降级（或让出给 CPU runtime）。

| 包 | 注册 id / 命令入口 | 说明(中文) | Description (EN) |
|---|---|---|---|
| lvx-base | importers gguf/hf/onnx/pytorch/safetensors · executions cpu-exec/vulkan-exec · tokenizer bpe · sampler default · trainers lora/pytorch/simple | 基础组件：转换/执行/格式 | base components: convert/execution/format |
| qwen35-cpu-run | runtime `qwen35-cpu` | Qwen3.5 混合架构 CPU 运行 | Qwen3.5 hybrid CPU runtime |
| qwen35-vulkan-run | runtime `qwen35-vulkan` | Qwen3.5 Vulkan 混合运行（S28 全参数） | Qwen3.5 hybrid on Vulkan (full S28) |
| llama-run | runtime `llama-cpu` | Llama/Qwen2/Mistral CPU 运行 | Llama-family CPU runtime |
| gemma4-vulkan-run | runtime `gemma4-vulkan` | Gemma4 GPU 运行 | Gemma4 Vulkan runtime |
| pytouch-run | runtime `pytorch-run` | 占位（开发中） | placeholder (WIP) |
| tune_vulkan | trainer `tune_vulkan` · `lvx tune --trainer tune_vulkan` | Vulkan LoRA 微调 | Vulkan LoRA fine-tune |
| openai-api-server | command `api` · `lvx api` | OpenAI 兼容 API 服务器 | OpenAI-compatible API server |
| hello / fake-* | 各 registry 测试条目 | 参考/测试插件 | reference/test plugins |

文档细读：[lvx-base](lvx-base.md) · [qwen35-cpu-run](qwen35-cpu-run.md) ·
[qwen35-vulkan-run](qwen35-vulkan-run.md) · [llama-run](llama-run.md) ·
[gemma4-vulkan-run](gemma4-vulkan-run.md) · [pytouch-run](pytouch-run.md) ·
[tune_vulkan](tune_vulkan.md) · [openai-api-server](openai-api-server.md) ·
[hello/fake](hello-fake.md)

所有版本号以官方源 manifest 为准（清单可能随发布更新）。命令细节以
`lvx <命令> --help` 为最终权威。

## English

The official source (`http://47.79.21.231:8080`, 12 packages) plugins expose **no
cross-plugin C API** — the host owns every registry
(runtime/importer/execution/tokenizer/sampler/trainer/backend). There are three
correct ways to "call an official plugin":

1. **User/script level**: host CLI commands (`lvx run/chat/bench/tune/api/convert`, ...)
   route automatically to the owning plugin; reuse the same commands in your tools.
2. **Registry cooperation**: inspect ids with `lvx <kind> list|info <id>`; declare the
   official package in your manifest `dependencies` so user environments have it.
3. **Capability boundaries**: official runtimes claim models by architecture probe
   (`lvx runtime probe model.lvx`); without a Vulkan device GPU runtimes degrade or
   yield to CPU runtimes.

| Package | Registry id / command | 说明(中文) | Description (EN) |
|---|---|---|---|
| lvx-base | importers gguf/hf/onnx/pytorch/safetensors · executions cpu-exec/vulkan-exec · tokenizer bpe · sampler default · trainers lora/pytorch/simple | 基础组件：转换/执行/格式 | base components |
| qwen35-cpu-run | runtime `qwen35-cpu` | Qwen3.5 CPU | Qwen3.5 hybrid CPU runtime |
| qwen35-vulkan-run | runtime `qwen35-vulkan` | Qwen3.5 Vulkan 混合 | Qwen3.5 hybrid on Vulkan |
| llama-run | runtime `llama-cpu` | Llama 家族 CPU | Llama-family CPU runtime |
| gemma4-vulkan-run | runtime `gemma4-vulkan` | Gemma4 GPU | Gemma4 Vulkan runtime |
| pytouch-run | runtime `pytorch-run` | 占位 | placeholder (WIP) |
| tune_vulkan | trainer `tune_vulkan` | Vulkan LoRA 微调 | Vulkan LoRA fine-tune |
| openai-api-server | command `api` | OpenAI API 服务器 | OpenAI-compatible server |
| hello / fake-* | test entries | 参考/测试 | reference/test |

Per-package guides: [lvx-base](lvx-base.md) · [qwen35-cpu-run](qwen35-cpu-run.md) ·
[qwen35-vulkan-run](qwen35-vulkan-run.md) · [llama-run](llama-run.md) ·
[gemma4-vulkan-run](gemma4-vulkan-run.md) · [pytouch-run](pytouch-run.md) ·
[tune_vulkan](tune_vulkan.md) · [openai-api-server](openai-api-server.md) ·
[hello/fake](hello-fake.md)

Versions track the official manifest; `lvx <command> --help` is the final authority
for command details.
