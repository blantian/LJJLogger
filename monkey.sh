#!/usr/bin/env bash
################################################################################
# Monkey test script for com.mgtv.tv  —— 取消物理按键 + Ctrl-C 优雅停止
################################################################################

set -e            # 任一命令出错立即退出，可按需去掉
PKG="com.mgtv.tv"
EVENTS=30000
THROTTLE=800
SEED=$(date +%s)

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
# 1. 定义 Ctrl-C 处理函数
###############################################################################
function stop_monkey() {
  echo -e "\n[Trap] Ctrl-C 捕获，正在终止设备中的 Monkey 进程…" | tee -a "${LOG_FILE}"

  # 查找所有 Monkey 进程 PID（兼容 Android 7/8+）
  PIDS=$(adb shell "ps | grep -E 'com\.android\.commands\.monkey' | awk '{print \$2}'" | tr -d '\r')

  if [[ -z "${PIDS}" ]]; then
    echo "[Trap] 未找到 Monkey 进程，可能已结束" | tee -a "${LOG_FILE}"
  else
    for pid in ${PIDS}; do
      echo "[Trap] kill -9 ${pid}" | tee -a "${LOG_FILE}"
      adb shell "kill -9 ${pid}"
    done
  fi

  echo "[Trap] Monkey 测试已手动中断 ✅" | tee -a "${LOG_FILE}"
  exit 0
}
#!/usr/bin/env bash
################################################################################
# Monkey test script for com.mgtv.tv  —— 取消物理按键 + Ctrl-C 优雅停止
################################################################################

set -e            # 任一命令出错立即退出，可按需去掉
PKG="com.mgtv.tv"
EVENTS=30000
THROTTLE=200
SEED=$(date +%s)

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
# 1. 定义 Ctrl-C 处理函数
###############################################################################
function stop_monkey() {
  echo -e "\n[Trap] Ctrl-C 捕获，正在终止设备中的 Monkey 进程…" | tee -a "${LOG_FILE}"

  # 查找所有 Monkey 进程 PID（兼容 Android 7/8+）
  PIDS=$(adb shell "ps | grep -E 'com\.android\.commands\.monkey' | awk '{print \$2}'" | tr -d '\r')

  if [[ -z "${PIDS}" ]]; then
    echo "[Trap] 未找到 Monkey 进程，可能已结束" | tee -a "${LOG_FILE}"
  else
    for pid in ${PIDS}; do
      echo "[Trap] kill -9 ${pid}" | tee -a "${LOG_FILE}"
      adb shell "kill -9 ${pid}"
    done
  fi

  echo "[Trap] Monkey 测试已手动中断 ✅" | tee -a "${LOG_FILE}"
  exit 0
}

# 捕获 SIGINT (Ctrl-C)
trap stop_monkey INT

###############################################################################
# 2. 确保干净启动
###############################################################################
adb shell "am force-stop ${PKG}"

###############################################################################
# 3. 启动 Monkey（无物理按键事件）
###############################################################################
adb shell "monkey \
  -p ${PKG} \
  --ignore-crashes \
  --ignore-timeouts \
  --ignore-security-exceptions \
  --monitor-native-crashes \
  --throttle ${THROTTLE} \
  --pct-touch 50 \
  --pct-motion 35 \
  --pct-appswitch 15 \
  --pct-nav 0 \
  --pct-syskeys 0 \
  -s ${SEED} \
  -v -v -v \
  ${EVENTS}" | tee -a "${LOG_FILE}"

###############################################################################
# 4. 正常结束
###############################################################################
echo "====== Monkey finished: $(date '+%F %T') ======"       | tee -a "${LOG_FILE}"

# 捕获 SIGINT (Ctrl-C)
trap stop_monkey INT

###############################################################################
# 2. 确保干净启动
###############################################################################
adb shell "am force-stop ${PKG}"

###############################################################################
# 3. 启动 Monkey（无物理按键事件）
###############################################################################
adb shell "monkey \
  -p ${PKG} \
  --ignore-crashes \
  --ignore-timeouts \
  --ignore-security-exceptions \
  --monitor-native-crashes \
  --throttle ${THROTTLE} \
  --pct-touch 50 \
  --pct-motion 35 \
  --pct-appswitch 15 \
  --pct-nav 0 \
  --pct-syskeys 0 \
  -s ${SEED} \
  -v -v -v \
  ${EVENTS}" | tee -a "${LOG_FILE}"

###############################################################################
# 4. 正常结束
###############################################################################
echo "====== Monkey finished: $(date '+%F %T') ======"       | tee -a "${LOG_FILE}"
