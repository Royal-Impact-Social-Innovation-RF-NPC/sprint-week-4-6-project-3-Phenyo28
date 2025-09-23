# 🩺RBN-Health-Monitoring-Device-project-3-PD
---
This project is an IoT-enabled healthcare monitoring device designed to measure and record vital signs, specifically pulse rate and body temperature, in real time. Using sensors connected to an ESP32 microcontroller, the system collects health data and transmits it over Wi-Fi to a mobile application, where it is displayed for easy monitoring. The device provides a portable and affordable solution for continuous health tracking, making it useful for patients, caregivers, and healthcare providers. By enabling timely detection of irregularities and supporting remote access, the project contributes to improved preventive healthcare and has potential for future expansion with additional sensors and advanced features.

## Who is project is designed for?
- Patients who need continuous monitoring of vital signs.
- Caregivers and family members responsible for health supervision.
- Healthcare providers who want remote access to patient data for timely interventions.
---


## Repository Contents
| File / Folder                 | Description                                       |
| ----------------------------- | ------------------------------------------------- |
| `Picture`                     |Folder of the project building images             |
| `BOM - PulseWave .xlsx`       | Bill of materials                                 |
| `README.md`                   |Update README.md                                   |
| `Schematic.png`               | Circuit schematic (image version)                 |
| `RP-Diale-PulseWave.ino`      | Arduino source code for running the soil detector |
|`PULSEWAVE PERFOMANCE FLOW`    | Perfomance workflow of the project                |
 

---
## Features

- Sends health data wirelessly via Wi-Fi
- Measures heart rate (BPM) in real time
- Tracks body temperature with digital sensors
-  Shows live readings directly on the device
- Enables remote access to health data through a mobile app and the device itself
- Designed for everyday use in healthcare or home settings
---
## Components Used
| Component Name               | Purpose in Project                                            | Quantity |
| ---------------------------- | ------------------------------------------------------------- | -------- |
| `ESP32`                        | Main microcontroller; collects sensor data & controls outputs | 1        |
| `9 V Battery`                  | Powers the system                                             | 1        |
| `Pulse Sensor`                 | Measures the user’s heart rate                                | 1        |
| `IR Thermometer Sensor`        | Measures body temperature                                     | 1        |
| `Breadboard`                   | Measures body temperature                         | 1        |
| `USB A-Male to Micro USB Male` | Powers or programs the ESP32 via USB                          | 1        |
| `LCD Screen`                   | Displays real-time sensor readings                            | 1        |
| `On/Off Switch`                | Turns the device on or off                                    | 1        |

---

## Software Development
-Arduino IDE (with ESP32 board support)

-Libraries:
- WiFi.h
- HTTPClient.h
- PulseSensorPlayground.h
- DFRobot_MLX90614.h
- Wire.h
- LiquidCrystal_I2C.h
<img width="1920" height="179" alt="Screenshot 2025-09-17 004305" src="https://github.com/user-attachments/assets/c213e6c2-e469-4ed0-b403-7a2f0503e60d" />

## The Code

## How the Code Works

1. Initialization (setup)
- Connects to Wi-Fi
- Initializes Pulse Sensor, Temperature Sensor, and LCD display
- Starts a web server on the ESP32
<img width="1914" height="936" alt="Screenshot 2025-09-17 004145" src="https://github.com/user-attachments/assets/e0c5c711-175a-4efa-8fc4-dcda3a6a4e21" />

2. Loop Operations (continuous monitoring)
- Reads pulse sensor raw values and detects heartbeats
- Calculates BPM (Beats Per Minute)
- Reads object temperature (body) and ambient temperature
- Prints results to the Serial Monitor for debugging
- Updates the LCD with current BPM & body temperature
- Sends data every 15 seconds to ThingSpeak (API call via HTTP)
- Hosts a web dashboard showing real-time BPM, temperature, signal strength, and health state
<img width="1887" height="651" alt="Screenshot 2025-09-17 004033" src="https://github.com/user-attachments/assets/108b4ea9-9269-47de-84f0-e2b7df35d1ac" />

3. Health Logic & Thresholds
- Pulse thresholds:
- `< 60 BPM` → Low (bradycardia risk)
- `60–100 BPM` → Normal
- `> 100 BPM` → High (tachycardia risk)
- Temperature thresholds:
- `< 36°C` → Low (hypothermia risk)
- `36–37.5°C` → Normal
- `> 37.5°C` → High (fever/infection risk)
- The system explains the possible causes of abnormal readings (e.g., stress, infection, dehydration).
<img width="1920" height="498" alt="Screenshot 2025-09-17 003225" src="https://github.com/user-attachments/assets/50b82596-d918-4637-8063-f8659ab65dc8" />

4. Web Dashboard
- Auto-refreshes every 2 seconds
- Displays:
- Heartbeat animation ♥ when pulse is detected
- BPM, temperature, and signal strength
- Current health state messages (normal, high, low)
- ThingSpeak connection status & next update timer
<img width="1917" height="571" alt="Screenshot 2025-09-17 003530" src="https://github.com/user-attachments/assets/b4cdd4f0-cf83-481f-8f26-ed3a11dd12fa" />

## Perfomance workflow of the project
<img width="1239" height="797" alt="Screenshot 2025-09-18 143142" src="https://github.com/user-attachments/assets/5dda3534-b8d9-4cf6-8d31-7dc5b5e4f8d4" />

---

## Schematic Design. 
I created  the schematic design of this project using draw.io. I started by mapping out all the components, including the ESP32 microcontroller, pulse sensor, temperature sensor, switch, LCD, and a 9v battery, then arranged them to make connections. This schematic helped plan the physical wiring, guided the coding process, and ensured that the system could reliably collect and transmit data.

<img width="1117" height="620" alt="Screenshot 2025-09-06 192206" src="https://github.com/user-attachments/assets/9bfbd855-af8f-4252-8542-5281151fc399" />


## App Development
I used the MIT App inventor to create my app. The pulse rate and body temperature readings from the ESP32 are displayed on the LCD screen of the device and in the mobile app for real-time monitoring. The same data is also sent to ThingSpeak, where it is visualized in real-time graphs, allowing users to track trends over time and monitor their health data effectively both locally and via the cloud.
<img width="869" height="719" alt="Screenshot 2025-09-17 004610" src="https://github.com/user-attachments/assets/98beadfa-babb-4867-aff1-e113ee5d82c7" />

- 	The first screen is the home screen
- 	Second screen is about how the app works and the benefits of using the app
- 	The third and fourth screens show the temperature and pulse readings.
- 	I added buttons so the user can navigate between the screens on the app. I used block code logic to handle the transitions

---

## How the ThingSpeak Readings Work
The ESP32 collects pulse and temperature values from the sensors and connects to Wi-Fi. These readings are then sent to ThingSpeak, an IoT analytics platform, using its API. Each data value is uploaded to a specific channel field (for example, Field 1 = Pulse Rate, Field 2 = Signal Strength, Field 3 = Object Temperature, Field 4 = Ambient Temperature). ThingSpeak stores the data in the cloud and displays it in real-time through interactive graphs and charts. This allows continuous monitoring of trends over time, remote access to health data, and easy integration with data analysis or alert systems.
<img width="1298" height="806" alt="Screenshot 2025-09-09 091009" src="https://github.com/user-attachments/assets/3277fdbe-5d2a-492c-a488-6d55d139bcd5" />

---
## The Casing
I created the casing on OnShape. This casing will provide protection for the electronics, gave it a cleaner look, and made the device portable.
<img width="1034" height="523" alt="image" src="https://github.com/user-attachments/assets/31d777d9-655b-4aff-bd46-81977aefa1e5" />

<img width="854" height="590" alt="Screenshot 2025-09-17 084955" src="https://github.com/user-attachments/assets/807e8e22-88e9-4eca-8c2c-e5ed4410b671" />


---
## Process of working on the Project

## Soldering:
![IMG-20250901-WA0003](https://github.com/user-attachments/assets/e612b328-15d6-4712-b2cf-38692746efca)

---
## Component Testing:
---
![IMG-20250905-WA0006](https://github.com/user-attachments/assets/38d5bf0b-ecc3-4149-ab6e-30b127555615) ![IMG-20250905-WA0009](https://github.com/user-attachments/assets/ad44780d-0bfd-4865-8fd2-258017accac9)
---

## Challenges I faced when building this project
During this project, I faced several challenges. The LCD screen was not working properly, it would turn on and off (and not work for a long time). Some components were overheating and some would not work when I connected them to my breadboard. The serial monitor failed to recognize certain components sometimes, to solve this I used an I2C scanner to correctly identify and connect the devices, which helped stabilize the system. The readings of my components were displaying slow. My ESP32 microcontroller burnt and was not responsive, this delayed the process as I had to source a replacement device before I could continue testing and integrating the components.

---

## Conclusion

This project gave me hands-on experience with IoT, embedded systems, and hardware designs. I learnt how to integrate multiple sensors with the ESP32, and create a web dashboard for real-time health monitoring. Despite challenges like overheating components and hardware failures, I gained valuable troubleshooting skills and a deeper understanding of electronics and prototyping.

## Acknowledgements

Special thanks to my workshop instructors, my peers (especially Lesane for helping me troubleshoot), and the online community resources that guided me throughout this journey.

## Future Improvements

In the future, I plan to:
- Improve the stability of the ESP32 setup and explore better power management.
- Add more sensors to expand health monitoring features.
- Develop a more user-friendly mobile app for live tracking.
- Explore 3D printing for more compact and durable casings.
