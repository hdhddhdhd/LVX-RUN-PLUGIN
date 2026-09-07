# tune_vulkan — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：tune_vulkan 1.1.2，trainer id `tune_vulkan`（`lvx trainer list` 可见）。
**调用**：
```sh
lvx install tune_vulkan
lvx tune --trainer tune_vulkan model.lvx data.txt tuned.lvx
lvx tune --trainer tune_vulkan -cpu model.lvx data.txt tuned.lvx   # 强制 CPU
lvx run tuned.lvx --chat            # 直接用微调产物对话
```
**数据格式（data.txt，自动识别）**：
- 每行一条纯文本序列
- 空行分隔的多行对话块（格式对齐模型 chat 模板）
- 每行空格分隔 token-id（旧格式，兼容）
**行为**：默认在离散 GPU 上训练（自动避开核显；无独显自动回退 CPU，且 stderr
提示后端）；rank-8 LoRA × q/k/v/o/gate/up/down，BF16 合并写回 154 张量。
**给插件开发者**：trainer 插件在 `lvx tune` 路由下工作；你的 CLI 若想复用训练产物
（.lvx），直接把它当普通模型交给 runtime 即可。


## English

**Package**: tune_vulkan 1.1.2, trainer id `tune_vulkan` (visible via
`lvx trainer list`).
**Invocation**:
```sh
lvx install tune_vulkan
lvx tune --trainer tune_vulkan model.lvx data.txt tuned.lvx
lvx tune --trainer tune_vulkan -cpu model.lvx data.txt tuned.lvx   # force CPU
lvx run tuned.lvx --chat            # chat with the tuned output directly
```
**Data formats (data.txt, auto-detected)**:
- one plain-text sequence per line
- dialogue blocks separated by blank lines (aligned to the model chat template)
- token-id lines separated by spaces (legacy, still supported)
**Behaviour**: trains on a discrete GPU by default (skips iGPUs; falls back to CPU
without one, with a stderr note); rank-8 LoRA over q/k/v/o/gate/up/down, BF16
merged write-back of 154 tensors.
**For plugin developers**: trainer plugins work under `lvx tune` routing; treat a
tuned .lvx as an ordinary model for runtimes.

