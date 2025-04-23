# Smart Medicine Box

An **Arduino-based Smart Medicine Box** that reminds users to take their medicine on time and ensures sensitive medicines are stored safely. It features alarm functionality and environmental monitoring, along with real-time data logging through Wi-Fi.

## Hardware Components

| Component            | Description                                    |
|---------------------|------------------------------------------------|
| IR Sensor           | Detects if the medicine has been taken         |
| Ultrasonic Sensor   | Monitors the stock level of medicines          |
| RTC Module          | Real-time clock for alarm scheduling           |
| Buzzer              | Provides alert sound for alarms                |
| LEDs                | Visual indication for different alerts         |
| I2C LCD Display     | Displays messages and status                   |
| DHT22 Sensor        | Monitors temperature and humidity              |
| LDR Sensor          | Monitors light exposure inside the box         |
| Arduino UNO         | Main controller for hardware interaction       |
| NodeMCU (ESP8266)   | Wi-Fi module for logging data online           |

## Features

- **Alarm Functionality**  
  Set alarms for medicine intake using the RTC module. The buzzer and LEDs are triggered to alert the user.

- **Light Monitoring**  
  The LDR sensor checks for light exposure inside the box to protect light-sensitive medicines.

- **Medicine Intake Detection**  
  The IR sensor detects whether the medicine has been taken.

- **Stock Level Monitoring**  
  An ultrasonic sensor is used to estimate the number of medicines left in the box and detect low-stock conditions.

- **Serial Communication**  
  Arduino Uno communicates with NodeMCU via serial for data sharing.

- **Data Logging via Wi-Fi**  
  NodeMCU sends data (like temperature, humidity, medicine status, and stock level) to a Flask server for remote access and monitoring.

## Folder Structure

```
Smart-Medicine-Box/
├── arduino_code/          # Code for Arduino UNO (sensors, control logic)
├── nodemcu_code/          # Code for NodeMCU (Wi-Fi, serial communication)
├── flask_server/          # Backend Flask server for data logging
│   ├── app.py             # Main Flask application
│   └── templates/
│       └── index.html     # Web interface template
├── static/                # Frontend static files
│   ├── style.css          # Styling for web interface
│   └── script.js          # Client-side JavaScript
```

## Getting Started

1. Clone the repository and set up the required libraries.
2. Upload the respective code to the Arduino UNO and NodeMCU.
3. Connect all sensors and components according to the circuit diagram.
4. Run the Flask server locally or on a cloud platform.
5. Start monitoring and managing your medicine box smartly.
