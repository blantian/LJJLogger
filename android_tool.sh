#!/usr/bin/env bash
#
# android_tool.sh
# 用法：android_tool.sh <command> [options]
#
# command:
#   screenshot    截图并拉取到本地  ./android_tool.sh screenshot -s <serial> -d <save_dir>
#   log           抓取 AV_MGLOGGER 日志 ./android_tool.sh log start -s <serial> -o <output_file> -t <tag>
#   install       构建 / 安装 / 启动 APK  ./android_tool.sh install -s debug -i put
#

set -euo pipefail

DEFAULT_SERIAL="${ANDROID_SERIAL:-$(adb devices | awk 'NR==2 {print $1}')}"
NOW="$(date +%Y%m%d_%H%M%S)"
PID_FILE="/tmp/android_log.pid"

#--------------------------------------
# 通用：打印帮助
#--------------------------------------
usage() {
cat <<EOF
Usage: $0 <command> [options]

Commands:
  screenshot   adb 截图并拉取到本地
  log          过滤 AV_MGLOGGER 日志到文件
  install      构建 / 安装 / 启动 APP
  playercore   构建 playercore 模块 AAR 并复制到指定目录

运行 $0 <command> -h 查看子命令帮助
EOF
exit 1
}

#--------------------------------------
# screenshot 子命令
#--------------------------------------
cmd_screenshot() {
    # 默认值
    local serial="$DEFAULT_SERIAL"
    local save_dir="$HOME/Desktop"

    # 解析参数
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -s|--serial) serial="$2"; shift 2;;
            -d|--dir)    save_dir="$2"; shift 2;;
            -h|--help) cat <<EOF
Usage: $0 screenshot [-s SERIAL] [-d DIR]

  -s SERIAL   设备序列号或 IP:port，默认 ${DEFAULT_SERIAL}
  -d DIR      本地保存目录，默认 ~/Desktop
EOF
            return ;;
            *) echo "未知参数 $1"; return 1;;
        esac
    done

    local remote="/sdcard/screenshot.png"
    local local_path="${save_dir}/screenshot_${NOW}.png"

    mkdir -p "$save_dir"

    echo "📸 设备($serial) 开始截图..."
    adb -s "$serial" shell screencap -p "$remote"

    echo "⬇️  拉取到本地: $local_path"
    adb -s "$serial" pull "$remote" "$local_path"

    echo "🧹 删除设备端临时文件"
    adb -s "$serial" shell rm "$remote" || true

    echo "✅ 完成: $local_path"
}

#--------------------------------------
# log 子命令
#--------------------------------------
cmd_log_start() {
    local serial="$DEFAULT_SERIAL"
    local outfile="$HOME/Desktop/mgtv_log_${NOW}.txt"
    local tag="AV_MGLOGGER"

    # 解析参数
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -s|--serial) serial="$2"; shift 2;;
            -o|--output) outfile="$2"; shift 2;;
            -t|--tag)    tag="$2"; shift 2;;
            -h|--help)
cat <<EOF
Usage: $0 log start [options]

  -s SERIAL   设备序列号，默认 $DEFAULT_SERIAL
  -o FILE     保存文件，默认 ~/Desktop/mgtv_log_<time>.txt
  -t TAGS     Tag 过滤(逗号分隔)，all/* 为全量，默认 AV_MGLOGGER
EOF
            return;;
            *) echo "未知参数 $1"; return 1;;
        esac
    done

    if [[ -f "$PID_FILE" ]] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null; then
        echo "⚠️ 已有日志会话在运行(PID $(cat "$PID_FILE"))，请先执行 '$0 log stop'"
        exit 1
    fi

    echo "🧹 清空 logcat 缓冲区"; adb -s "$serial" logcat -c

    echo "🚩 后台抓取日志 -> $outfile"
    echo "PID 将记录在 $PID_FILE"

    (
        if [[ "$tag" == "all" || "$tag" == "*" ]]; then
            adb -s "$serial" logcat -v threadtime
        else
            IFS=',' read -ra TAGS <<< "$tag"
            adb -s "$serial" logcat -v threadtime $(printf -- '-s %s ' "${TAGS[@]}")
        fi
    ) | tee "$outfile" &

    echo $! > "$PID_FILE"
    disown
    echo "✅ 开始抓取，使用 '$0 log stop' 终止"
}

cmd_log_stop() {
    if [[ ! -f "$PID_FILE" ]]; then
        echo "❌ 未找到 PID 文件，可能未在后台抓日志"
        exit 1
    fi
    local pid="$(cat "$PID_FILE")"
    if kill -0 "$pid" 2>/dev/null; then
        kill "$pid"
        echo "🛑 已停止抓日志 (PID $pid)"
    else
        echo "⚠️ 进程 $pid 不存在"
    fi
    rm -f "$PID_FILE"
}


#--------------------------------------
# install 子命令
#--------------------------------------
cmd_install() {
    local serial="$DEFAULT_SERIAL"
    local build_type="debug"   # debug / release
    local custom_path=""
    local flavor=""
    local pkg="com.mgtv.mglogger"
    local activity="com.mgtv.mglogger.MainActivity"
    local install_method="install"  # install / put

    # 解析参数
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -s|--serial)     serial="$2"; shift 2;;
            -p|--path)       custom_path="$2"; shift 2;;
            -b|--build-type) build_type="$(echo "$2" | tr 'A-Z' 'a-z')"; shift 2;;
            -f|--flavor)     flavor="$2"; shift 2;;
            -i|--install)    install_method="$(echo "$2" | tr 'A-Z' 'a-z')"; shift 2;;
            --pkg)           pkg="$2"; shift 2;;
            --activity)      activity="$2"; shift 2;;
            -h|--help) cat <<EOF
Usage: $0 install [options]

  -s SERIAL        设备序列号，默认 ${DEFAULT_SERIAL}
  -b TYPE          构建类型 debug/release，默认 debug
  -f FLAVOR        Product Flavor (可选)
  -p PATH          使用已有 APK，跳过本地构建
  -i METHOD        安装方式 install/put，默认 install
  --pkg NAME       包名 (默认 $pkg)
  --activity CLS   主 Activity (默认 $activity)

说明：执行 install 前会先构建并投放 playercore AAR 到 /Users/skyblue/imgo/code/MGLogger/app/libs
EOF
            return ;;
            *) echo "未知参数 $1"; return 1;;
        esac
    done

    # 验证安装方式
    if [[ "$install_method" != "install" && "$install_method" != "put" && "$install_method" != "push" ]]; then
        echo "❌ 安装方式必须是 install, put 或 push"
        exit 1
    fi

    echo "========================================"
    [[ -z "$custom_path" ]] && echo "构建类型: ${flavor}${build_type}" \
                             || echo "使用自定义 APK: $custom_path"
    echo "设备: $serial"
    echo "安装方式: $install_method"
    echo "========================================"

    # ---------- 预构建并投放 playercore AAR ----------
    local LIB_DIR="/Users/skyblue/imgo/code/MGLogger/app/libs"
    local AAR_NAME="mglogger_1.0.0.aar"

    echo "📦 预构建 playercore → $LIB_DIR ($AAR_NAME)"
    mkdir -p "$LIB_DIR"
    # 清理旧 AAR，避免重复打包导致依赖冲突
    rm -f "$LIB_DIR"/mglogger-*.aar

    # 直接调用本脚本内的 cmd_playercore
    cmd_playercore -b "$build_type" -p "$LIB_DIR" -n "$AAR_NAME"

    # ---------- 构建 APK ----------
    if [[ -z "$custom_path" ]]; then
        echo "🧹 清理构建残留..."
        rm -rf app/build/outputs/apk/
        ./gradlew clean

        echo "🔨 开始构建 ${flavor}${build_type}..."
        # 构造 gradle 任务名称，处理首字母大写
        local gradle_task="assemble"
        if [[ -n "$flavor" ]]; then
            gradle_task="${gradle_task}$(echo "${flavor:0:1}" | tr 'a-z' 'A-Z')${flavor:1}"
        fi
        gradle_task="${gradle_task}$(echo "${build_type:0:1}" | tr 'a-z' 'A-Z')${build_type:1}"
        ./gradlew "$gradle_task"

        custom_path="$(find app/build/outputs/apk/ -name "*${build_type}.apk" | head -n1)"
        [[ -z "$custom_path" ]] && { echo "❌ 未找到生成的 APK"; exit 1; }
        echo "✅ 构建成功: $custom_path"
    else
        [[ ! -f "$custom_path" ]] && { echo "❌ 文件不存在: $custom_path"; exit 1; }
    fi

    # ---------- 安装到设备 ----------
    if [[ "$install_method" == "install" ]]; then
        echo "🚮 卸载旧包 (如存在)..."
        adb -s "$serial" uninstall "$pkg" || true

        echo "⬆️  安装 APK (adb install)..."
        adb -s "$serial" install -r "$custom_path"
    else
        echo "🚮 清理旧版本应用文件..."
        adb -s "$serial" shell "find /data/app/ -name '*${build_type}*' -exec rm -rf {} \;" || true

        echo "⬆️  推送 APK 到设备 (adb push)..."
        adb -s "$serial" push "$custom_path" /data/app/
        echo "⏱️  等待安装完成..."
        sleep 5
    fi

    echo "🚀 启动应用..."
    adb -s "$serial" shell am start -n "$pkg/$activity"
    echo "✅ 安装并启动完成!"
}


#--------------------------------------
# playercore 子命令
#--------------------------------------
cmd_playercore() {
    local build_type="debug"   # debug / release
    local target_dir="$HOME/Desktop"
    local aar_name="mglogger_1.0.0.aar"
    local run_install=false

    # 解析参数
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -b|--build-type) build_type="$(echo "$2" | tr 'A-Z' 'a-z')"; shift 2;;
            -p|--path)       target_dir="$2"; shift 2;;
            -n|--name)       aar_name="$2"; shift 2;;
            --install)       run_install=true; shift;;
            -h|--help) cat <<EOF
Usage: $0 playercore [options]

  -b TYPE          构建类型 debug/release，默认 debug
  -p PATH          目标目录，默认 ~/Desktop
  -n NAME          AAR文件名，默认 mglogger_1.0.0.aar
  --install        构建完成后执行安装脚本

示例:
  $0 playercore -b release -p /user/lib
  $0 playercore -b debug -p ./libs -n my-playercore.aar
  $0 playercore -b debug --install
EOF
            return ;;
            *) echo "未知参数 $1"; return 1;;
        esac
    done

    # 验证构建类型
    if [[ "$build_type" != "debug" && "$build_type" != "release" ]]; then
        echo "❌ 构建类型必须是 debug 或 release"
        exit 1
    fi

    echo "========================================"
    echo "构建 playercore 模块"
    echo "构建类型: $build_type"
    echo "目标目录: $target_dir"
    echo "文件名: $aar_name"
    echo "执行安装: $run_install"
    echo "========================================"

    # 创建目标目录
    mkdir -p "$target_dir"

    # 清理之前的构建
    echo "🧹 清理 mglogger 模块构建残留..."
    rm -rf mglogger/build/outputs/aar/

    # 构建 AAR
    echo "🔨 开始构建 mglogger ${build_type}..."
    if [[ "$build_type" == "debug" ]]; then
        ./gradlew :mglogger:assembleDebug
        source_aar="mglogger/build/outputs/aar/mglogger-debug.aar"
    else
        ./gradlew :mglogger:assembleRelease
        source_aar="mglogger/build/outputs/aar/mglogger-release.aar"
    fi

    # 检查AAR文件是否生成成功
    if [[ ! -f "$source_aar" ]]; then
        echo "❌ AAR文件构建失败，未找到: $source_aar"
        exit 1
    fi

    # 复制并重命名AAR文件
    target_path="$target_dir/$aar_name"
    echo "📦 复制AAR文件: $source_aar -> $target_path"
    cp "$source_aar" "$target_path"

    # 验证复制结果
    if [[ -f "$target_path" ]]; then
        local file_size=$(du -h "$target_path" | cut -f1)
        echo "✅ 构建完成!"
        echo "📄 文件路径: $target_path"
        echo "📊 文件大小: $file_size"
        # 显示AAR内容概览
        echo "📋 AAR内容概览:"
        unzip -l "$target_path" | grep -E '\.(so|jar|xml)$' | head -10
        echo "   ..."
        # 如果指定了 --install 参数，执行外部安装脚本
        if [[ "$run_install" == true ]]; then
            local install_script="/Users/lantianbao/imgtv/code/sp/IPTV-SpecialZone-APP-Portal/install.sh"
            echo ""
            echo "⏱️  等待 5 秒后执行安装脚本..."
            sleep 5
            echo "🚀 执行安装脚本: $install_script"
            if [[ -f "$install_script" ]]; then
                echo "📍 切换到脚本目录并执行..."
                (
                    cd "$(dirname "$install_script")"
                    bash "$(basename "$install_script")" Debug
                )
                echo "✅ 安装脚本执行完成!"
            else
                echo "❌ 安装脚本不存在: $install_script"
                exit 1
            fi
        fi
    else
        echo "❌ 复制失败"
        exit 1
    fi
}

#====================================================================
# 主入口
#====================================================================
[[ $# -lt 1 ]] && usage

case "$1" in
  screenshot) shift; cmd_screenshot "$@";;
  log)
       case "${2-}" in
         start) shift 2; cmd_log_start "$@";;
         stop)  shift 2; cmd_log_stop  "$@";;
         *)     echo "用法: $0 log {start|stop}"; exit 1;;
       esac;;
  install) shift; cmd_install "$@";;
  playercore) shift; cmd_playercore "$@";;
  -h|--help) usage;;
  *) echo "未知命令: $1"; usage;;
esac
