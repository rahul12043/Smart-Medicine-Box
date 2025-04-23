#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT22

RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change address if needed

// ===== DEFAULT ALARM SETTINGS =====
const int DEFAULT_ALARM_HOUR = 14;     // Example: 2 PM
const int DEFAULT_ALARM_MINUTE = 28;
// ==================================

int alarmHour = -1;
int alarmMinute = -1;
bool alarmActive = false;
bool alarmTriggeredToday = false;  // To ensure alarm rings only once

const int BUZZER = 3;
const int ALARM_LED = 8;
const int LDR_PIN = 7;
const int IR_SENSOR_PIN = 4;
const int TRIGGER_PIN = 12;
const int ECHO_PIN = 13;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.init();
  lcd.backlight();
  lcd.print("System Ready");

  dht.begin();
  pinMode(BUZZER, OUTPUT);
  pinMode(ALARM_LED, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(ALARM_LED, LOW);

  if (DEFAULT_ALARM_HOUR >= 0 && DEFAULT_ALARM_HOUR < 24 &&
      DEFAULT_ALARM_MINUTE >= 0 && DEFAULT_ALARM_MINUTE < 60) {
    alarmHour = DEFAULT_ALARM_HOUR;
    alarmMinute = DEFAULT_ALARM_MINUTE;
    alarmActive = true;

    lcd.clear();
    lcd.print("Default Alarm Set");
    lcd.setCursor(0, 1);
    lcd.print(formatTime(alarmHour, alarmMinute));
    delay(2000);
  }

  Serial.println("System Initialized - Ready");
}

void loop() {
  handleSerialCommands();

  DateTime now = rtc.now();
  int currentHour = now.hour();
  int currentMinute = now.minute();

  // Reset alarm trigger at midnight
  if (currentHour == 0 && currentMinute == 0) {
    alarmTriggeredToday = false;
  }

  // Trigger alarm if time matches and hasn't already triggered today
  if (alarmActive &&
      alarmHour == currentHour &&
      alarmMinute == currentMinute &&
      !alarmTriggeredToday) {
    triggerAlarm();
    alarmTriggeredToday = true;
  }

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String ldrStatus = digitalRead(LDR_PIN) ? "Box Open" : "Box Closed";
  String irStatus = (digitalRead(IR_SENSOR_PIN) == LOW) ? "Medicine Taken!" : "Medicine Not Taken";
  long distance = getUltrasonicDistance();

  updateLCD(currentHour, currentMinute);

  // JSON output
  Serial.print("{\"time\":\"");
  Serial.print(formatTime(currentHour, currentMinute));
  Serial.print("\",\"temperature\":");
  Serial.print(temperature);
  Serial.print(",\"humidity\":");
  Serial.print(humidity);
  Serial.print(",\"ldr\":\"");
  Serial.print(ldrStatus);
  Serial.print("\",\"ir\":\"");
  Serial.print(irStatus);
  Serial.print("\",\"distance\":");
  Serial.print(distance);
  Serial.print(",\"alarm\":\"");
  Serial.print((alarmHour >= 0) ? formatTime(alarmHour, alarmMinute) : "Not set");
  Serial.println("\"}");

  delay(2000);
}

void updateLCD(int hour, int minute) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(formatTime(hour, minute));
  lcd.setCursor(0, 1);
  if (alarmHour >= 0) {
    lcd.print("Alarm: ");
    lcd.print(formatTime(alarmHour, alarmMinute));
  } else {
    lcd.print("No alarm set");
  }
}

String formatTime(int h, int m) {
  char buf[6];
  sprintf(buf, "%02d:%02d", h, m);
  return String(buf);
}

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SET_ALARM:")) {
      setAlarm(command);
    } else if (command == "CLEAR_ALARM") {
      clearAlarm();
    }
  }
}

void setAlarm(String command) {
  // Format: SET_ALARM:HH:MM
  int colon1 = command.indexOf(':');
  int colon2 = command.indexOf(':', colon1 + 1);

  alarmHour = command.substring(colon1 + 1, colon2).toInt();
  alarmMinute = command.substring(colon2 + 1).toInt();

  if (alarmHour >= 0 && alarmHour < 24 &&
      alarmMinute >= 0 && alarmMinute < 60) {
    alarmActive = true;
    alarmTriggeredToday = false;

    Serial.print("Alarm set to ");
    Serial.print(alarmHour);
    Serial.print(":");
    Serial.println(alarmMinute);

    lcd.clear();
    lcd.print("Alarm set to");
    lcd.setCursor(0, 1);
    lcd.print(formatTime(alarmHour, alarmMinute));
    delay(2000);
  } else {
    Serial.println("Invalid time format");
  }
}

void clearAlarm() {
  alarmHour = -1;
  alarmMinute = -1;
  alarmActive = false;
  alarmTriggeredToday = false;
  digitalWrite(ALARM_LED, LOW);
  Serial.println("Alarm cleared");

  lcd.clear();
  lcd.print("Alarm cleared");
  delay(2000);
}

void triggerAlarm() {
  Serial.println("ALARM_TRIGGERED");
  digitalWrite(ALARM_LED, HIGH);

  lcd.clear();
  lcd.print("ALARM!");
  lcd.setCursor(0, 1);
  lcd.print("TAKE MEDICINE");

  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 1000, 300);
    delay(500);
  }

  digitalWrite(ALARM_LED, LOW);
}

long getUltrasonicDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.0344 / 2;
}
