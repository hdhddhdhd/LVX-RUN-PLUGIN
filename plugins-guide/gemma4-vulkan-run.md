# gemma4-vulkan-run — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：gemma4-vulkan-run 1.0.0，runtime id `gemma4-vulkan`。
**接管条件**：architecture=`gemma4` 的 .lvx，且 Vulkan 可用。
**调用**：
```sh
lvx run gemma4-e4b-full.lvx -p "你好" --chat -n 64
lvx chat gemma4-e4b-full.lvx
lvx bench gemma4-e4b-full.lvx
lvx runtime probe gemma4-e4b-full.lvx
```
**给插件开发者**：这是"全 GPU 管线 runtime"（recordToken/prefill/decode 循环 +
GPUModelLoader 常驻 VRAM 权重）的参照实现，qwen35-vulkan-run 的混合 v1 复用了它
的 gemv/缓冲模式。


## English

**Package**: gemma4-vulkan-run 1.0.0, runtime id `gemma4-vulkan`.
**Ownership**: architecture=`gemma4` .lvx models with Vulkan available.
**Invocation**:
```sh
lvx run gemma4-e4b-full.lvx -p "hi" --chat -n 64
lvx chat gemma4-e4b-full.lvx
lvx bench gemma4-e4b-full.lvx
lvx runtime probe gemma4-e4b-full.lvx
```
**For plugin developers**: reference implementation of a full-GPU runtime
(recordToken/prefill/decode loop + VRAM-resident weights via GPUModelLoader);
qwen35-vulkan-run's mixed-v1 reuses its gemv/buffer patterns.

