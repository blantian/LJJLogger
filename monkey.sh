#!/usr/bin/env bash
################################################################################
# Monkey test script for com.mgtv.tv
# - 禁止物理按键事件
# - 支持 Ctrl-C 手动终止
# - 监控应用进程，一旦退出就结束 Monkey，防止中途被重新拉起
################################################################################

set -euo pipefail

PKG="com.mgtv.tv"      # 目标包名
EVENTS=30000           # Monkey 事件总数
THROTTLE=200           # 事件间隔 (ms)
SEED=$(date +%s)       # 随机种子

LOG_DIR="${HOME}/monkey_logs"
mkdir -p "${LOG_DIR}"
LOG_FILE="${LOG_DIR}/${PKG}_$(date +%F_%H-%M-%S)_seed${SEED}.log"

echo "====== Monkey start: $(date '+%F %T') ======"          | tee  "${LOG_FILE}"
echo "Package      : ${PKG}"                                 | tee -a "${LOG_FILE}"
echo "Events       : ${EVENTS}"                              | tee -a "${LOG_FILE}"
echo "Throttle     : ${THROTTLE} ms"                         | tee -a "${LOG_FILE}"
echo "Random seed  : ${SEED}"                                | tee -a "${LOG_FILE}"
echo "Log file     : ${LOG_FILE}"                            | tee -a "${LOG_FILE}"
echo "======================================================" | tee -a "${LOG_FILE}"

###############################################################################
# Ctrl-C 处理 —— 杀掉设备中正在运行的 Monkey
###############################################################################
function stop_monkey() {
  echo -e "\n[Trap] 正在终止设备中的 Monkey…" | tee -a "${LOG_FILE}"
  PIDS=$(adb shell "ps | grep -E 'com\.android\.commands\.monkey' | awk '{print \$2}'" | tr -d '\r')
  if [[ -n "${PIDS}" ]]; then
    for pid in ${PIDS}; do
      echo "[Trap] kill -9 ${pid}" | tee -a "${LOG_FILE}"
      adb shell "kill -9 ${pid}"
    done
  fi
  echo "[Trap] Monkey 已结束" | tee -a "${LOG_FILE}"
  exit 0
}
trap stop_monkey INT

###############################################################################
# 保证干净启动
###############################################################################
adb shell "am force-stop ${PKG}"

###############################################################################
# 启动 Monkey（后台运行），禁止物理按键，不忽略 Crash/ANR
###############################################################################
adb shell "monkey \
  -p ${PKG} \
  --monitor-native-crashes \
  --throttle ${THROTTLE} \
  --pct-touch 50 \
  --pct-motion 35 \
  --pct-appswitch 15 \
  --pct-nav 0 \
  --pct-syskeys 0 \
  -s ${SEED} \
  -v -v -v \
  ${EVENTS}" | tee -a "${LOG_FILE}" &
MONKEY_JOB_PID=$!   # 本地后台 job PID

###############################################################################
# 进程守护：检测 App 是否存活，若消失则终止 Monkey
###############################################################################
while true; do
  sleep 3
  # 判断后台 Monkey 是否已自然结束
  if ! kill -0 ${MONKEY_JOB_PID} 2>/dev/null; then
    break
  fi

  APP_PID=$(adb shell "pidof ${PKG}" | tr -d '\r')
  if [[ -z "${APP_PID}" ]]; then
    echo "[WatchDog] 检测到 ${PKG} 进程已退出，停止 Monkey" | tee -a "${LOG_FILE}"
    stop_monkey            # 会 exit 0
  fi
done

###############################################################################
# 正常收尾
###############################################################################
wait ${MONKEY_JOB_PID}
echo "====== Monkey finished: $(date '+%F %T') ======"       | tee -a "${LOG_FILE}"
