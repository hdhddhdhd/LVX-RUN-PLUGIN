# hello / fake-importer / fake-runtime / fake-trainer（参考/测试包） — 官方插件调用 / Official Plugin Call

## 中文

**包信息**（官方源各 1.0.0）：
- `hello`：最小参考插件（注册后 `lvx plugins` 可见）
- `fake-importer`：importer id `fakegguf`（扩展名 fakegguf）
- `fake-runtime`：runtime id `fakecpu`（测试）
- `fake-trainer`：fake trainers（`lvx trainer list` 可见）

**用途**：机制测试与学习。manifest 结构与安装/校验流程与真实插件完全一致，
适合作为"hello world"对照；不要在生产模型上依赖它们。
**调用**：`lvx install hello` 后 `lvx plugins` 观察注册；fake 系列在对应
`lvx <kind> list` 中出现（fakegguf 只认 `.fakegguf` 后缀，fakecpu probe 不认领
真实模型）。


## English

**Packages** (each 1.0.0 on the official source):
- `hello`: minimal reference plugin (appears in `lvx plugins` after install)
- `fake-importer`: importer id `fakegguf` (extension fakegguf)
- `fake-runtime`: runtime id `fakecpu` (test)
- `fake-trainer`: fake trainers (visible in `lvx trainer list`)

**Purpose**: mechanism tests and learning. Their manifest/install/checksum flow is
identical to real plugins — a good hello-world reference; do not depend on them for
production models.
**Invocation**: `lvx install hello` then observe `lvx plugins`; the fake series
appears in the matching `lvx <kind> list` (fakegguf only claims `.fakegguf` files,
fakecpu never claims real models).

