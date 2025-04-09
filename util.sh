#!/bin/bash

PROCESS_NAME="Frontier"
LOG_FILE=~/FrontierLog.txt
CMD="/home/wbjin/Frontier/build/Frontier -n 50000000 -b 50 -l /home/wbjin/Frontier/seedList.txt -f 50000 -c 50000 -s /home/wbjin/Frontier/frontier_save.txt --recover"
CHECK_INTERVAL=60  # in seconds
MAX_UNCHANGED=5    # number of iterations before restart

# Color codes
CYAN_BOLD="\033[1;36m"
YELLOW="\033[0;33m"
RESET="\033[0m"

echo -e "${CYAN_BOLD}🔍 Monitoring process '$PROCESS_NAME' every $CHECK_INTERVAL seconds...${RESET}"

unchanged_count=0
prev_size=$(stat -c%s "$LOG_FILE" 2>/dev/null || echo 0)

while true; do
    if pgrep -x "$PROCESS_NAME" > /dev/null; then
        current_size=$(stat -c%s "$LOG_FILE" 2>/dev/null || echo 0)

        if [ "$current_size" -eq "$prev_size" ]; then
            ((unchanged_count++))
        else
            unchanged_count=0
        fi

        prev_size=$current_size

        echo "$(date): ✅ Process is running. Log size: $current_size bytes (unchanged $unchanged_count times)"

        if [ "$unchanged_count" -ge "$MAX_UNCHANGED" ]; then
            echo "$(date): ⚠️ Log size hasn't changed in $MAX_UNCHANGED checks. Restarting '$PROCESS_NAME'..."
            pkill -x "$PROCESS_NAME"
            nohup $CMD > "$LOG_FILE" 2>&1 &
            unchanged_count=0
        fi
    else
        echo "$(date): ❌ Process is NOT running. Restarting '$PROCESS_NAME'..."
        nohup $CMD > "$LOG_FILE" 2>&1 &
        unchanged_count=0
    fi

    sleep "$CHECK_INTERVAL"
done
