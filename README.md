# 🏠 ESP32 Home Appliance Scheduler

## 📌 Overview
Control and schedule **home appliances** using ESP32 + Relay + RTC.  
Local web dashboard allows **setting ON/OFF schedules** for each day of the week.

---

## 🛠️ Hardware Required
- ESP32 Dev Board  
- 2 Relays (or more if needed)  
- RTC Module (DS3231)  
- Jumper wires, breadboard  

---

## 🔌 Wiring
- Relay 1 → GPIO19  
- Relay 2 → GPIO18  
- RTC → I2C (SDA=GPIO21, SCL=GPIO22)  

---

## ▶️ Usage
1. Upload the sketch to ESP32.  
2. Connect to Wi-Fi AP `ESP32-AP` (password: `12345678`).  
3. Open browser → go to `192.168.4.1`.  
4. Set schedule for each device (day, hour, minute, state).  
5. ESP32 automatically toggles devices based on schedule.

---

## 📂 Repo Structure
