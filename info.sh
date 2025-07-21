#!/usr/bin/env bash
#
# monitor_android_proc.sh <packageName | pid> [intervalSec]
#
set -e

########################################
# 0. 处理参数
########################################
TARGET=$1
INTERVAL=${2:-2}

[ -z "$TARGET" ] && { echo "用法: $0 <packageName|pid> [intervalSec]"; exit 1; }

########################################
# 1. 解析 PID
########################################
if [[ "$TARGET" =~ ^[0-9]+$ ]]; then
  PID=$TARGET
else
  PID=$(adb shell "pidof $TARGET" 2>/dev/null | tr -d '\r')
  [ -z "$PID" ] && PID=$(adb shell "ps | grep -m 1 $TARGET" | awk '{print $2}' | tr -d '\r')
fi
[ -z "$PID" ] && { echo "未找到进程 $TARGET"; exit 2; }

########################################
# 2. 注册 Ctrl+C 处理器
########################################
trap 'echo -e "\n收到 Ctrl+C，监控结束，Bye~"; exit 0' SIGINT

########################################
# 3. 打印表头
########################################
echo -e "监控进程 PID=$PID (interval=${INTERVAL}s)\nPress Ctrl+C to stop.\n"
printf "%-8s %-10s %-12s %-12s\n" "TIME" "CPU(%)" "RSS(MB)" "PSS(MB)"
echo "---------------------------------------------------------"

########################################
# 4. 采样循环
########################################
while true; do
  NOW=$(date +%H:%M:%S)

  # ---- CPU (%)
  CPU=$(adb shell "dumpsys cpuinfo | grep -w $PID" | awk '{print $1}' | tr -d '%')
  [ -z "$CPU" ] && CPU="0.0"

  # ---- MEM (kB → MB)
  read RSS PSS <<<$(adb shell "dumpsys meminfo $PID | awk '/TOTAL/{printf \"%.1f %.1f\", \$2/1024, \$3/1024}'")

  printf "%-8s %-10s %-12s %-12s\n" "$NOW" "$CPU" "$RSS" "$PSS"
  sleep "$INTERVAL"
done
