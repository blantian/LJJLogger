# AGENTS.md  ── Android Project MGLogger
# 本文件对当前目录及子目录有效；若有子模块需特殊规则，可在该子模块再放一份 AGENTS.md 覆盖

---
## ☕️ JDK & Android SDK
- JDK 1.8
- Android Gradle Plugin 4.1.3 (必须是 4.1.x 系列，因为要适配低版本设备)
- compileSdk = 33, minSdk = 14
- Android NDK 16.1.4479499（由 `mglogger/build.gradle → ndkVersion` 指定）

## 📦 Gradle & Dependencies
- Gradle 6.5 (不能更改)
- Kotlin 1.4.32 (不能升级)
- Kotlin Coroutines 1.4.3 (不能升级)
- koin 2.1.6 (不能升级)

## 📂 Project Structure
- `:app`
- `:mglogger` (核心模块)

---
## 🧩 Module Map
- `:app` 主应用模块，包含 UI 和业务逻辑
- `:mglogger` 核心业务模块，包含cpp代码和核心逻辑。
> 子模块若有特定测试或编译脚本，在各自目录新建 `AGENTS.md` 追加覆盖

---
## 🏷️ Versioning & Git
- 采用 GitHub Flow（feat/bugfix/hotfix 分支 → PR → main）
- 使用 Conventional Commits；提交信息模板：`<type>(<scope>): <description>`
    - type ∈ { feat, fix, refactor, docs, chore, test }
- PR 描述需包含：
    1. 变更概要
    2. 关联 Issue/任务单

---
## 📦 Dependabot / Gradle Update
# 不自动升级到 *preview* / *RC* 版本
# 不能升级 依赖的三方库
# 不能升级 Kotlin 版本
# 不能升级 Android Gradle Plugin 版本
# 不能升级 Gradle 版本
# 不能升级 NDK 版本
# 不能升级 JDK 版本
# 不能升级 Android SDK 版本
denylist:
- 'com.android.tools.build:gradle:*preview*'
- 'org.jetbrains.kotlin:kotlin-gradle-plugin:*-M*'


---
## 🌐 Network & Sandbox
- 默认在离线沙箱运行；禁止执行外网 curl/wget/npm install 等命令
- 允许访问内部 Maven 代理 `https://maven.priv.example.com`
- 若需联网请加 `#needs-internet` 标记并说明理由

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