# qwen35-cpu-run — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：qwen35-cpu-run 1.0.0，runtime id `qwen35-cpu`。
**接管条件**：architecture=`qwen35`（GDN 混合：24 递归层 + 8 全注意力）的 .lvx；
无 Vulkan 设备时是 qwen35 模型的默认 runtime（probe 92）。
**调用**：
```sh
lvx run  qwen35q4.lvx -p "你好" -n 64
lvx chat qwen35q4.lvx
lvx bench qwen35q4.lvx
lvx runtime probe qwen35q4.lvx      # 确认被 qwen35-cpu 接管
```
上下文容量 kvCap=8192。
**给插件开发者**：本 runtime 是"整模型 CPU 推理"的参照实现；数值门以同权重
CPU-Q4 logits 为基准。GPU 版见 qwen35-vulkan-run。


## English

**Package**: qwen35-cpu-run 1.0.0, runtime id `qwen35-cpu`.
**Ownership**: .lvx models with architecture=`qwen35` (GDN hybrid: 24 recurrent +
8 full-attention layers); it is the default qwen35 runtime when no Vulkan device
exists (probe 92).
**Invocation**:
```sh
lvx run  qwen35q4.lvx -p "Hello" -n 64
lvx chat qwen35q4.lvx
lvx bench qwen35q4.lvx
lvx runtime probe qwen35q4.lvx      # confirms qwen35-cpu owns the model
```
Context capacity: kvCap=8192.
**For plugin developers**: reference implementation of whole-model CPU inference;
numerics gate against CPU-Q4 logits of the same weights. GPU counterpart:
qwen35-vulkan-run.

