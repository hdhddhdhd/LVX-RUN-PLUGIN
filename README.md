# LVX-RUN-PLUGIN — LVX 插件开发教程 & 测试插件仓库

> **中文** | [English](README.en.md)
>
> 官方插件调用指南（双语）：[plugins-guide/](plugins-guide/README.md)

本仓库双重身份：**插件开发教程**（本 README）与 **lvx-plugin 测试插件**（topic: `lvx-plugin`，会被 `lvx update` 自动收录）。

- ABI 头文件快照：`include/lvx/*.h`
- 可编译示例：`demo_hello_plugin.c`（根目录）+ 预编译产物 `lvx-plugin/liblvx_demo_hello.so`
- 本仓库自身就是"能被官方源机制收录"的活例子。

---

# LVX 插件开发教程

## 1. 概念

插件 = 一个 `liblvx_*.so` 动态库，导出若干 **extern "C"** 查询符号。加载方（PluginManager）按符号发现能力：

| 能力 | 查询符号 | 头文件 |
|---|---|---|
| 插件本体（必须） | `lvx_plugin_init` | plugin_api.h |
| 运行时 v1（解码） | `lvx_plugin_runtimes` | runtime_api.h |
| 运行时 v2（会话/chat） | `lvx_plugin_runtimes_v2` | runtime_api_v2.h |
| 流式 chat 扩展 | `lvx_plugin_runtimes_v2s` | runtime_api_v2.h |
| 导入器 | `lvx_plugin_importers` | importer_api.h |
| 其它（backend/execution/tokenizer/sampler/trainer/format…） | 见对应 lvx/*.h | |

ABI 原则：**只跨 extern-C POD**；结构体冻结、可选能力用额外查询符号扩展。

## 2. 最小插件（参考 demo_hello_plugin.c）

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

编译（文件名必须 `liblvx_*.so` 才会被加载）：

```sh
gcc -shared -fPIC -O2 -I include demo_hello_plugin.c -o liblvx_my_plugin.so
```

### 做一个真正的 runtime（能跑模型）

runtime 需要导出 v1（必要时 v2）描述符。v1 最少要实现：

```c
int probe(const char *path, LVXRuntimeProbe *out);      // 0..100 打分认领模型
int create(const char *path, const char *backend_hint,
           LVXRuntimeHandle **out, char *err, size_t errsz); // 加载模型建会话
void destroy(LVXRuntimeHandle *h);
int generate(LVXRuntimeHandle *h, const char *prompt, char *out, size_t outsz);
```

v2 增加会话/采样/多轮：`generate_ex / bench / configure / chat_turn / reset / stats`（可选的 v2s 提供逐 token 流式）。字段布局严格参照 `include/lvx/runtime_api*.h`，不要臆造。

> **S28 会话通用选项扩展**：`LVXSessionOptions` 尾部新增 `ctx_len`（上下文上限）、
> `kv_cache_type`（0=f16/1=q8_0/2=q4_0）、`system_prompt`（UTF-8，调用期有效、runtime 需拷贝）。
> 读取新字段必须用 `struct_size >= sizeof(LVXSessionOptions)` 门控；旧核心发来旧
> struct_size 时新 runtime 应兼容（只读基础字段），旧 runtime 因自身旧 sizeof 自动
> 忽略尾段。create 期选项（ctx/kv/层计划/GPU 设备号）由核心挂在 `backend_hint` 后缀：
> `vulkan;ctx=8192;kv=q8_0;plan=g0:20,c0:all`（plan 中 `g0:20`=gpu0 前 20 层、`c0:all`=cpu0 剩余全部；
> 不认识 `;` 后缀的 runtime 把整个 hint 当后端名忽略即可）。

## 3. 软件包清单（manifest.json）

仓库默认分支需含目录 `lvx-plugin/`，其中 `manifest.json`：

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

- `file`：安装后落盘的文件名，需以 `liblvx_`/`lvx_` 开头且以 `.so` 结尾，禁止 `/` 与 `..`
- `sha256`/`size`：安装时校验，务必与文件一致（`sha256sum` 生成）
- `dependencies`：可依赖其它包 `[{"name":"qwen35-run","version":">=1.0.0"}]`，版本支持裸版本、`>=`、`>`、`==`
- 一个仓库也可作为多包索引：外层放 `{"packages":[ {...如上}, {...} ]}`

官方源（静态仓库）的 manifest 直接放仓库根；GitHub 插件仓库放 `lvx-plugin/manifest.json`，文件与 manifest 同目录。

## 4. 发布与安装

1. 代码推送到默认分支（推荐 main）
2. 仓库设置 → Topics → 添加 **`lvx-plugin`**（被自动收录的唯一条件）
3. 用户侧自动生效：

```sh
lvx update                 # 刷新包列表（能看到你的仓库）
lvx install my-plugin      # 按包名安装
lvx install demo           # 也支持文件名/子串
lvx plugin ls              # 查看已安装包与版本（lvx ls plugins 同义）
lvx ls devices             # 查看 CPU/GPU 设备编号（供 -g/-cpu 参数）
lvx plugins                # 查看已加载(运行中)插件
```

## 5. 测试建议

- 本地加载测试：`LVX_PLUGIN_PATH=/path/to/dir lvx plugins`（只扫 `liblvx_*`/`lvx_* .so`）
- manifest 校验：改文件后 `lvx install` 应报 sha256 mismatch（说明校验生效）
- runtime 数值验证：与 llama.cpp 同输入逐 token logits 相关 ≥0.99 再发布（参考 qwen35-run 的交付标准）
- 版本迭代：改 `version` 后重新 `lvx update && lvx install <name>`

## 6. 本仓库即教程样例

`lvx-demo-hello` v0.2.0 就是按上述流程发布的最小插件：源码在根目录、manifest 在 `lvx-plugin/`、topic 已打上。把它 fork 修改即可起步。


---

# 官方微调插件：tune_vulkan（快速上手）

主仓库发布页（README）：https://github.com/hdhddhdhd/LVX-RUN

```sh
lvx install tune_vulkan
lvx tune --trainer tune_vulkan model.lvx data.txt tuned.lvx
```

- `data.txt` 支持三种形态：纯文本（每行一条序列）、多行对话块（空行分隔，格式对齐模型 chat 模板）、token-id 行（旧格式）
- 默认使用**独立显卡**训练（自动避开核显；无独显回退 CPU），`-cpu` 强制 CPU
- rank-8 LoRA 覆盖 q/k/v/o + ffn gate/up/down，BF16 合并写回；产物直接 `lvx run`/`lvx chat`
- 包管理：`lvx remove <包名>` 卸载、`lvx upgrade [包名]` 升级、`lvx plugin ls [-a]` 查看（-a 展开文件）

## 单文件仓库（single-file repo）

本仓库按"根目录一个 .lvx"分发：**`plugins.lvx`** 是唯一机器分发物——
插件二进制、版本注册表（manifest JSON）全部封装在内（LVX Universal
Container，`kind=repo`）。

- 直接安装：
  ```
  lvx install plugins.lvx
  lvx install /path/to/plugins.lvx
  ```
- GitHub topic 源（`github-topic=lvx-plugin`）会自动优先抓取仓库根的
  `plugins.lvx` 并按容器安装；找不到时回退旧的 `lvx-plugin/manifest.json`
  格式。
- 查看内容：`lvx type plugins.lvx`（显示 kind 与 section 表）。
- 重新打包：改 `plugins.spec` 后 `lvx_pack plugins.spec plugins.lvx`
  （`lvx_pack` 与 `lvx` 同仓库构建）。
