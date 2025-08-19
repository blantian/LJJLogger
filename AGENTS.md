# AGENTS.md  ── Android Project MGLogger
# 本文件对当前目录及子目录有效；若有子模块需特殊规则，可在该子模块再放一份 AGENTS.md 覆盖

---
## ☕️ JDK & Android SDK & NDK
- JDK 1.8
- Android Gradle Plugin 4.1.3 (可以适当升级，但跨度不能太大，因为要适配低版本Android 设备 API(14-33))
- compileSdk = 33, minSdk = 14
- C++ Version 11（由 `mglogger/build.gradle → cppFlags` 指定）
- CMake 3.10.2（由 `mglogger/build.gradle → cmakeVersion` 指定）
- CMakeLists.txt 需要包含 `set(CMAKE_CXX_STANDARD 11)` 来指定 C++ 版本
- CMakeLists.txt 需要包含 `set(CMAKE_CXX_STANDARD_REQUIRED ON)` 来确保 C++ 版本要求

## 📦 Gradle & Dependencies
- Gradle 6.5 (可以升级，但是跨度不能太大，因为要适配低版本Android 设备 API(14-28))

## 📂 Project Structure
- `:app`
- `:ljjlogger` (核心模块)

---
## 🧩 Module Map
- `:app` 主应用模块，包含 UI 和业务逻辑
- `:ljjlogger` 核心业务模块，包含cpp代码和核心逻辑。
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
# 不能升级 NDK 版本
# 不能升级 JDK 版本
# 可升级 Android Gradle Plugin 版本（适当升级，不能跨度太大，需要适配 Android 低版本设备）
---
## 🗑️ Forbidden APIs
ban:
- android.util.Log.d
- java.lang.Runtime.exec

## 🏁 Done
Codex 完成全部步骤、确保 *所有 run 块* 成功后方可推送/提交。若遇到无法通过的脚本，请在 PR 中 @代码 Owner 说明原因并请求豁免。