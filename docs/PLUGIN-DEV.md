# LVX 插件开发教程

本仓库同时承担两个角色：**lvx-plugin 测试插件**（topic: `lvx-plugin`，供 `lvx update` 自动收录）与 **插件开发文档**。

- 头文件快照：`include/lvx/*.h`（ABI 定义，与 LVX 构建树一致）
- 可编译示例：`demo_hello_plugin.c`（仓库根目录）+ 预编译产物 `lvx-plugin/liblvx_demo_hello.so`
- 本仓库自身就是"能被官方源机制收录"的活例子。

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

1. 代码推送到默认分支（main/master 均可，推荐 main）
2. 仓库设置 → Topics → 添加 **`lvx-plugin`**（这是被自动收录的唯一条件）
3. 用户侧自动生效：

```sh
lvx update                 # 刷新包列表（能看到你的仓库）
lvx install my-plugin      # 按包名安装
lvx install demo           # 也支持文件名/子串
lvx plugin ls              # 查看已安装包与版本
lvx plugins                # 查看已加载(运行中)插件
```

## 5. 测试建议

- 本地加载测试：`LVX_PLUGIN_PATH=/path/to/dir lvx plugins`（只扫 `liblvx_*`/`lvx_* .so`）
- manifest 校验：改文件后 `lvx install` 应报 sha256 mismatch（说明校验生效）
- runtime 数值验证：与 llama.cpp 同输入逐 token logits 相关 ≥0.99 再发布（参考 qwen35-run 的交付标准）
- 版本迭代：改 `version` 后重新 `lvx update && lvx install <name>`

## 6. 本仓库即教程样例

`lvx-demo-hello` v0.2.0 就是按上述流程发布的最小插件：源码在根目录、manifest 在 `lvx-plugin/`、topic 已打上。把它 fork 修改即可起步。
