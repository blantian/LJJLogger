#!/usr/bin/env bash
#
# 用法: ./info.sh <packageName | pid> [intervalSec]
#

set -e

TARGET=$1
INTERVAL=${2:-2}

if [ -z "$TARGET" ]; then
  echo "用法: $0 <packageName|pid> [intervalSec]"
  exit 1
fi

# ========= 获取 PID =========
get_pid() {
  local target=$1
  if [[ "$target" =~ ^[0-9]+$ ]]; then
    echo "$target"
  else
    adb shell "pidof $target" 2>/dev/null | tr -d '\r' || \
    adb shell "ps | grep -w $target" | awk '{print $2}' | tr -d '\r'
  fi
}

PID=$(get_pid "$TARGET")
if [ -z "$PID" ]; then
  echo "❌ 未找到进程 $TARGET"
  exit 1
fi

echo "🎯 监控进程 PID=$PID（$TARGET），每 ${INTERVAL}s 采样"

# ========= 准备 CSV =========
TIME_TAG=$(date +%Y%m%d_%H%M%S)
CSV_FILE="monitor_${TARGET//./_}_${TIME_TAG}.csv"
echo "📂 数据将保存到: $CSV_FILE"

# CSV 文件表头
echo "TIME,CPU(a%),CPU(p%),RSS(MB),PSS(MB),JavaHeap(MB),NativeHeap(MB)" > "$CSV_FILE"

# 终端表头
printf "%-10s | %-9s %-9s %-8s %-8s %-13s %-13s\n" "TIME" "CPU(a%)" "CPU(p%)" "RSS" "PSS" "JavaHeap" "NativeHeap"
echo "---------------------------------------------------------------------------------------------"

trap 'echo -e "\n🛑 监控结束，日志保存在 $CSV_FILE"; exit 0' SIGINT

# ========= 主采样循环 =========
while true; do
  NOW=$(date +%H:%M:%S)

  # ---- 1. 设备总体 CPU 使用率 ----
  read cpu user nice system idle iowait irq softirq steal < <(adb shell cat /proc/stat | grep "^cpu " | awk '{print $1, $2, $3, $4, $5, $6, $7, $8, $9}')
  total1=$((user + nice + system + idle + iowait + irq + softirq + steal))
  idle1=$((idle + iowait))

  sleep 0.5  # 必须有延迟才能计算变化

  read cpu user nice system idle iowait irq softirq steal < <(adb shell cat /proc/stat | grep "^cpu " | awk '{print $1, $2, $3, $4, $5, $6, $7, $8, $9}')
  total2=$((user + nice + system + idle + iowait + irq + softirq + steal))
  idle2=$((idle + iowait))

  total_diff=$((total2 - total1))
  idle_diff=$((idle2 - idle1))

  if [ "$total_diff" -eq 0 ]; then
    TOTAL_CPU="0.0"
  else
    usage=$(echo "scale=1; (100 * ($total_diff - $idle_diff)) / $total_diff" | bc)
    TOTAL_CPU=${usage:-0.0}
  fi

  # ---- 2. 当前进程 CPU 使用率 ----
  CPU=$(adb shell dumpsys cpuinfo | awk -v pid="$PID" '$0 ~ pid"/" {print $1}' | tr -d '%' | head -n 1)
  CPU=${CPU:-0.0}

  # ---- 3. 内存信息 ----
  MEMINFO=$(adb shell dumpsys meminfo $PID)

  RSS_KB=$(echo "$MEMINFO" | awk '/TOTAL/ {print $2}' | head -n 1)
  PSS_KB=$(echo "$MEMINFO" | awk '/TOTAL/ {print $3}' | head -n 1)
  JAVA_KB=$(echo "$MEMINFO" | awk '/Java Heap/ {print $3}' | head -n 1)
  NATIVE_KB=$(echo "$MEMINFO" | awk '/Native Heap/ {print $3}' | head -n 1)

  RSS=$(echo "scale=1; ${RSS_KB:-0}/1024" | bc)
  PSS=$(echo "scale=1; ${PSS_KB:-0}/1024" | bc)
  JAVA=$(echo "scale=1; ${JAVA_KB:-0}/1024" | bc)
  NATIVE=$(echo "scale=1; ${NATIVE_KB:-0}/1024" | bc)

  # ---- 4. 打印到终端 ----
  printf "%-8s | %-10s %-7s %-8s %-8s %-12s %-12s\n" "$NOW" "$TOTAL_CPU" "$CPU" "$RSS" "$PSS" "$JAVA" "$NATIVE"

  # ---- 5. 写入 CSV ----
  echo "$NOW,$TOTAL_CPU,$CPU,$RSS,$PSS,$JAVA,$NATIVE" >> "$CSV_FILE"

  sleep "$INTERVAL"
done

