#!/bin/sh

# Path to the hwmon0 temperature input file
temp_file='/sys/class/hwmon/hwmon0/temp1_input'

# Check if the temperature file exists
if [[ -f "$temp_file" ]]; then
    raw=$(cat "$temp_file")
    temp_c=$(echo "scale=1; $raw / 1000" | bc)

    # Display the temperature
    echo "Temperature: ${temp_c} °C"
else
    echo "Temperature sensor not found at /sys/class/hwmon/hwmon0/temp1_input"
fi
