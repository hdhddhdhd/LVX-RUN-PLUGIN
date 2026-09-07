# openai-api-server — 官方插件调用 / Official Plugin Call

## 中文

**包信息**：openai-api-server 1.1.0，命令入口 `api`。
**调用**：
```sh
lvx install openai-api-server
lvx api model.lvx                         # 后台服务（默认端口 1200，被占自动+1）
lvx api a.lvx b.lvx -c 8000/5000 -s "简短回答"   # 多模型：值按模型顺序一一对应
lvx api ls                                # 查看运行中的服务与 pid/port
lvx api rm <id>                           # 停止服务
```
日志在 `~/.lvx/api-<id>.log`（含每模型 "loaded (ctx N, system prompt set)"）。
**HTTP 端点**：`GET /v1/models`、`POST /v1/chat/completions`（OpenAI 格式：
model/messages/temperature/top_k/top_p/max_tokens）。
**通用参数（多模型按序）**：`-c/-context`（值 `8000/5000` 或单值全用；按模型元
数据上限截断）、`-s/-system`（默认 system 消息；请求自带 system 角色时不叠加）、
`-kv/-v`（llama 引擎仅 f16，q8_0/q4_0 警告忽略）、`-g/-gpu/-cpu`（警告忽略——
api 模型走 CPU llama 引擎，无层拆分）。
**给插件开发者**：当前仅服务 llama-family .lvx（LlamaRuntime 引擎）。若你的插件
要起自己的 HTTP 服务，可直接 fork 该命令模式（fork+daemon+state json）。


## English

**Package**: openai-api-server 1.1.0, command entry `api`.
**Invocation**:
```sh
lvx install openai-api-server
lvx api model.lvx                         # background server (default port 1200, auto-increments)
lvx api a.lvx b.lvx -c 8000/5000 -s "Be brief"    # multi-model: values map in order
lvx api ls                                # list running servers (pid/port)
lvx api rm <id>                           # stop one
```
Logs live in `~/.lvx/api-<id>.log` (per model: "loaded (ctx N, system prompt set)").
**HTTP endpoints**: `GET /v1/models`, `POST /v1/chat/completions` (OpenAI format:
model/messages/temperature/top_k/top_p/max_tokens).
**Common options (ordered per model)**: `-c/-context` (`8000/5000` or a single
value for all; capped by model metadata), `-s/-system` (default system message; not
duplicated when the request already has a system role), `-kv/-v` (llama engine is
f16 only; q8_0/q4_0 warn and ignore), `-g/-gpu/-cpu` (warn and ignore — api models
run on the CPU llama engine without layer split).
**For plugin developers**: only llama-family .lvx is served today (LlamaRuntime
engine). To run your own HTTP service, fork this command pattern
(fork+daemon+state json).

