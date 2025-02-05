# dam-gates-controlling-system
An IoT-based automated dam gate control system using ESP8266, Blynk, and Telegram integration.
# IoT-Based Dam Gate Control System 🌊🚦

This project automates dam gate operations using **ESP8266**, **Blynk**, and **Telegram API**. It monitors water levels and controls servo motors to open/close gates based on predefined thresholds. It also sends real-time alerts via Telegram and provides remote control through the Blynk app.
## 🚀 Features
✔ **Automated Gate Control** – Gates open/close based on water level.  
✔ **Remote Monitoring** – Water levels sent to Telegram and Blynk.  
✔ **Manual Control** – Users can control gates via the Blynk app.  
✔ **Flood Alerts** – Sends notifications when the water level is too high.  
✔ **Malfunction Detection** – Identifies faulty sensors or gate issues.  

## 🔧 Components Required
- **ESP8266 (NodeMCU)**
- **Water Level Sensor**
- **Servo Motors (x2)**
- **Buzzer**
- **Power Supply (5V)**
- **Jumper Wires**

## 🏗 How It Works
1. **Water Level Measurement** – The sensor reads the water level and sends data to ESP8266.
2. **Threshold-Based Actions**:
   - If **water level < 40%** → Keep gates **closed**.
   - If **water level ≥ 40%** → Open **Gate 1**.
   - If **water level ≥ 60%** → Open **both gates**.
3. **Real-Time Updates** – Water levels are displayed in the **Blynk App** & **Telegram Bot**.
4. **Manual Override** – Users can manually open/close gates using the **Blynk App**.
5. **Alerts & Notifications** – If water level exceeds safe limits, an alert is sent via **Telegram**.

    ## 📱 Blynk App Setup
1. **Install Blynk App** (Android/iOS).
2. **Create a New Project** → Select **ESP8266**.
3. **Add Widgets**:
   - **Gauge (V0)** – Water Level (%)
   - **Button (V1, V2, V3)** – Gate Controls
   - **LED (V4)** – Water Level Alert Indicator
4. **Enter Blynk Auth Token** (from email) in config.h.

    ## 💬 Telegram Bot Setup
1. Open Telegram and search for **BotFather**.
2. Type `/newbot` and follow instructions to create a bot.
3. Copy the **Bot Token** and update it in `config.h`:
   ```cpp
   #define BOTtoken "YOUR_TELEGRAM_BOT_TOKEN"
   
### **Blynk App**
- Live water level gauge updates.
- Buttons to manually control gates.
- LED turns **ON** if water level exceeds the threshold.

  ## 🛠 Troubleshooting
1. **ESP8266 Not Connecting to WiFi?**
   - Double-check **SSID and Password** in `config.h`.
   - Ensure your router supports **2.4GHz WiFi** (ESP8266 doesn’t support 5GHz).
  
2. **Blynk App Not Updating?**
   - Verify the **Blynk Auth Token** is correct.
   - Ensure **ESP8266 is online** (check Serial Monitor).

3. **Telegram Bot Not Sending Messages?**
   - Check if the **bot token** and **chat ID** are correct.
   - Make sure you’ve started a chat with your bot.




