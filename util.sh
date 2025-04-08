#!/bin/bash

PROCESS_NAME="Frontier"

if [ -z "$PROCESS_NAME" ]; then
    echo "Usage: $0 <process_name>"
    exit 1
fi

# Color codes
CYAN_BOLD="\033[1;36m"
YELLOW="\033[0;33m"
RESET="\033[0m"

echo -e "🔍 Monitoring process '$PROCESS_NAME' every 60 seconds..."

while true; do

    # Check if process is running
    if pgrep -x "$PROCESS_NAME" > /dev/null; then
        echo "$(date): ✅ Process '$PROCESS_NAME' is running."
    else
        echo "$(date): ❌ Process '$PROCESS_NAME' is NOT running."
        echo "🔄 Restarting '$PROCESS_NAME'"

        nohup /home/wbjin/Frontier/build/Frontier -n 500000000 -b 50 -l /home/wbjin/Frontier/seedList.txt -f 20000 -c 30000 -s /home/wbjin/Frontier/frontier_save.txt --recover > ~/FrontierLog.txt 2>&1 &
    fi

    sleep 60
done

