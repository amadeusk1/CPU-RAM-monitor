import psutil
import serial
import time
import subprocess

# Change COM7 to your Arduino port
arduino = serial.Serial("COM5", 9600, timeout=1)
time.sleep(2)

def get_wifi_name():
    try:
        result = subprocess.check_output(
            ["netsh", "wlan", "show", "interfaces"],
            encoding="utf-8",
            errors="ignore"
        )

        for line in result.splitlines():
            line = line.strip()
            if line.startswith("SSID") and "BSSID" not in line:
                return line.split(":", 1)[1].strip()

        return "No WiFi"
    except:
        return "WiFi Error"

def get_ping():
    try:
        result = subprocess.check_output(
            ["ping", "8.8.8.8", "-n", "1"],
            encoding="utf-8",
            errors="ignore"
        )

        for line in result.splitlines():
            if "time=" in line:
                ping_part = line.split("time=")[1]
                ping = ping_part.split("ms")[0].strip()
                return ping + "ms"

        return "No ping"
    except:
        return "Ping error"

while True:
    cpu = psutil.cpu_percent()
    ram = psutil.virtual_memory().percent
    wifi = get_wifi_name()
    ping = get_ping()

    message = f"CPU:{cpu:.0f}%,RAM:{ram:.0f}%,WIFI:{wifi},PING:{ping}\n"
    arduino.write(message.encode())

    print(message.strip())
    time.sleep(1)