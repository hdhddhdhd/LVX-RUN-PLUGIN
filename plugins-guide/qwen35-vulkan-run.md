# qwen35-vulkan-run — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：qwen35-vulkan-run 1.0.1，runtime id `qwen35-vulkan`。
**接管条件**：Vulkan 设备可用时 probe 95，自动压过 CPU 包接管 architecture=qwen35
的模型；无设备时 confidence 0（CPU 包接管）。
**架构（混合 v1）**：权重全量上传 VRAM（~2.6GB@Q4_K）；Q4_K/Q5_K/Q6_K 行主 gemv 与
lm-head 在 GPU，解码骨架在 CPU。数值门：与 CPU-Q4 逐 token logits worst maxdiff
0.0001、top1 10/10（阈值 0.05 内）。GPU 初始化失败自动纯 CPU 运行并在 stderr 提示。
**调用（S28 通用参数全支持）**：
```sh
lvx ls devices                        # 看 gpu/cpu 编号
lvx run qwen35q4.lvx -p "..." -c 4096             # 上下文上限（≤kvCap 8192）
lvx run qwen35q4.lvx -p "..." -g 0 20 -cpu 0 all  # 前 20 层 GPU，其余 CPU
lvx run qwen35q4.lvx -p "..." -g 0 8 -cpu 0 all   # 实测：8 gpu / 24 cpu 层
lvx chat qwen35q4.lvx -s "你是简洁的助手"          # chat 首轮 system 消息
lvx bench qwen35q4.lvx                # RX 5700 XT + Q4_K 参考 ~3.77 tok/s（≈13× CPU）
```
`-kv q8_0/q4_0` 会警告 "kv stays f32; ignored"（当前 kv 为 f32）。
**给插件开发者**：逐层拆分语义 = 按权重名 `blk.N.` 解析层号 + 层计划表；CPU 层走
反量化 gemv。该插件不接受"半层"拆分（按整层）。


## English

**Package**: qwen35-vulkan-run 1.0.1, runtime id `qwen35-vulkan`.
**Ownership**: with a Vulkan device, probe 95 — it out-scores the CPU package for
architecture=qwen35 models; without a device confidence drops to 0 (CPU takes over).
**Architecture (mixed-v1)**: weights uploaded to VRAM (~2.6GB at Q4_K); Q4_K/Q5_K/
Q6_K row-major gemvs and the lm-head run on GPU, the decoder skeleton on CPU.
Numerics gate vs CPU-Q4: worst logits maxdiff 0.0001, top1 10/10. GPU init failure
falls back to pure CPU with a stderr note.
**Invocation (all S28 options)**:
```sh
lvx ls devices                        # device indices first
lvx run qwen35q4.lvx -p "..." -c 4096             # context cap (<= kvCap 8192)
lvx run qwen35q4.lvx -p "..." -g 0 20 -cpu 0 all  # first 20 layers GPU, rest CPU
lvx run qwen35q4.lvx -p "..." -g 0 8 -cpu 0 all   # measured: 8 gpu / 24 cpu
lvx chat qwen35q4.lvx -s "Be concise"             # system message on first turn
lvx bench qwen35q4.lvx                # RX 5700 XT + Q4_K ref ~3.77 tok/s (~13x CPU)
```
`-kv q8_0/q4_0` warns "kv stays f32; ignored" (kv is f32 today).
**For plugin developers**: per-layer split parses the layer id from tensor names
`blk.N.` against a plan; CPU layers use dequantized gemvs. Split granularity is a
whole layer, never half a layer.

