# llama-run — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：llama-run 1.0.1，runtime id `llama-cpu`。
**接管条件**：llama/qwen2/mistral 家族架构的 .lvx（KQuant 权重，llama.cpp 行布局）。
**调用**：
```sh
lvx run tinyllama.lvx -p "hi" -n 32
lvx chat tinyllama.lvx
lvx bench tinyllama.lvx
```
模型 chat 模板跟随 tokenizer 数据；UTF-8 字节级 token decode 已修复（中文不乱码）。
**给插件开发者**：
- openai-api-server 内部加载的就是 llama-family .lvx（同一 LlamaRuntime 引擎），
  因此 API 服务器目前只服务 llama 家族模型。
- 上下文上限来自模型元数据（context_length）；新的 `loadCtx` 可在加载时把上限
  调低（不高于模型自带上限）。


## English

**Package**: llama-run 1.0.1, runtime id `llama-cpu`.
**Ownership**: llama/qwen2/mistral-family .lvx models (KQuant weights, llama.cpp
row layout).
**Invocation**:
```sh
lvx run tinyllama.lvx -p "hi" -n 32
lvx chat tinyllama.lvx
lvx bench tinyllama.lvx
```
Chat template follows the tokenizer data; UTF-8 byte-level token decoding is fixed
(Chinese text does not garble).
**For plugin developers**:
- openai-api-server loads llama-family .lvx internally (the same LlamaRuntime
  engine), so the API server currently serves llama-family models only.
- The context limit comes from model metadata (context_length); `loadCtx` may lower
  it at load time (never above the model's own limit).

