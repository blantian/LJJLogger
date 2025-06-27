# AGENTS.md  ──MGLogger/mglogger 模块
# 本文件对当前目录及子目录有效；若有子模块需特殊规则，可在该子模块再放一份 AGENTS.md 覆盖
# 本模块为 MGLogger 的核心模块，包含 C/C++ 代码和日志框架的主要逻辑。是基于美团开源的日志框架。
# 本模块主要用于日志的缓存、加密和压缩等功能。缓存用mmap，压缩用glib，日志加密用mbedtls。

## 📂 Project Structure
- `:mglogger` (核心模块)
---
## 模块目录结构
- `src/main/cpp` （C/C++ 代码）
- `src/main/cpp/external` （mbedtls C++ 库）
- `src/main/cpp/jni` （JNI 代码）
- `src/main/cpp/mglogger` （日志框架缓存、加密、压缩主要逻辑代码）
- `src/main/java` （Java/Kotlin 代码）
- `src/main/java/com/mgtv/logger/kt` （kotlin 重构模块代码）
- `src/main/java/com/mgtv/logger/` （业务层java代码，待重构代码）

---
## 📜 Code Style (Kotlin)
- `explicit-api=strict`
- Coroutine 默认调度器：`Dispatchers.Default`
- 不允许直接用 `GlobalScope`
- 所有流式数据用 `StateFlow`/`SharedFlow`；禁用 `LiveData`
- 禁用 `Thread`/`ExecutorService`，统一使用 **Kotlin 协程**

---
## 🗑️ Forbidden APIs
ban:
- android.util.Log.d
- java.lang.Runtime.exec
- kotlinx.coroutines.GlobalScope
- org.apache.http (Deprecated)

---
## 🏁 Done
Codex 完成全部步骤、确保 *所有 run 块* 成功后方可推送/提交。若遇到无法通过的脚本，请在 PR 中 @代码 Owner 说明原因并请求豁免。