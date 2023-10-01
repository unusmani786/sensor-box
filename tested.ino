#include <Wire.h>
#include <LiquidCrystal_I2C.h> -
#include <MFRC522.h>

#define BUTTON_PIN 10 // The button is connected to digital pin 2
#define TOUCH_SENSOR_PIN 6
#define PIR_SENSOR_PIN 8
#define IR_SENSOR_PIN 9
#define ULTRASONIC_TRIGGER_PIN 2
#define ULTRASONIC_ECHO_PIN 3
#define BUZZER_PIN 4  

// Define RFID RC522 pins  
#define SS_PIN 53
#define RST_PIN 5

// Define LED pins for sensor status
#define LED_TOUCH 15
#define LED_PIR 18
#define LED_IR 17
#define LED_ULTRASONIC 16
#define LED_RFID 14

// Create objects for the sensors and RFID RC522
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the I2C address if needed
MFRC522 mfrc522(SS_PIN , RST_PIN);

bool sensorsEngaged[] = {false, false, false, false, false}; // Tracks which sensors are engaged
int currentSensor = -1; // Currently engaged sensor (-1 means none)
int buzzerState = LOW;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
//  pinMode(BUZZER_PIN, OUTPUT);
  
  pinMode(LED_TOUCH, OUTPUT);
  pinMode(LED_PIR, OUTPUT);
  pinMode(LED_IR, OUTPUT);
  pinMode(LED_ULTRASONIC, OUTPUT);
  pinMode(LED_RFID, OUTPUT);
  lcd.begin();
  lcd.backlight();
  lcd.print("SENSOR KIT ");
  lcd.setCursor(0, 1);
  lcd.print("MADE BY UNAIB");
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init(); // Initialize RFID RC522
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    // Button is pressed, cycle through the sensors
    int nextSensor = getNextSensor();
    engageSensor(nextSensor);
    delay(500); // Debounce delay
  }

  if (currentSensor >= 0) {
    // Check if the current sensor is active
    switch (currentSensor) {
      case 0: // Touch sensor
        handleTouchSensor();
       
        break;
      case 1: // PIR sensor
        handlePIRSensor();
        break;
      case 2: // IR sensor
        handleIRSensor();
        break;
      case 3: // Ultrasonic sensor
        handleUltrasonicSensor();
        break;
      case 4: // RFID RC522 sensor
        handleRFIDSensor();
        break;
    }
  }
}

int getNextSensor() {
  int nextSensor = currentSensor;
  do {
    nextSensor = (nextSensor + 1) % 5; // Wrap around to the first sensor if necessary
  } while (sensorsEngaged[nextSensor]);
  return nextSensor;
}

void engageSensor(int sensor) {
  if (currentSensor != sensor) {
    // Deactivate the previous sensor
    deactivateSensor(currentSensor);
    
    // Activate the new sensor
    currentSensor = sensor;
    sensorsEngaged[currentSensor] = true;
    
    // Display sensor name on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (currentSensor) {
      case 0:
        lcd.print("Touch Sensor");
        break;
      case 1:
        lcd.print("PIR Sensor");
        break;
      case 2:
        lcd.print("IR Sensor");
        break;
      case 3:
        lcd.print("Ultrasonic < 10 cm");
        break;
      case 4:
        lcd.print("RFID Sensor");
        break;
    }
    lcd.setCursor(0, 1);
    lcd.print("Engaged");
    
    // Start buzzer
    //tone(BUZZER_PIN, 1000);
    //buzzerState = HIGH;

    // Turn on the corresponding LED for sensor status
    digitalWrite(getSensorLED(currentSensor), HIGH);
  }
}

void deactivateSensor(int sensor) {
  if (sensor >= 0) {
    sensorsEngaged[sensor] = false;
    //noTone(BUZZER_PIN);
    //buzzerState = LOW;

    // Turn off the LED for the deactivated sensor
    digitalWrite(getSensorLED(sensor), LOW);
  }
}

void handleTouchSensor() {
  int touchState = digitalRead(TOUCH_SENSOR_PIN);
  Serial.print("Touch Sensor: ");
  Serial.println(touchState);
  // Your code to handle the Touch sensor data goes here
  if(touchState == 1)
  {
     tone(BUZZER_PIN, 1000);
    buzzerState = HIGH;
  
    }
else 
{
  noTone(BUZZER_PIN);
      buzzerState = LOW;

  }
}

void handlePIRSensor() {
  int pirState = digitalRead(PIR_SENSOR_PIN);
  Serial.print("PIR Sensor: ");
  Serial.println(pirState);
  // Your code to handle the PIR sensor data goes here
  if(pirState == 1)
  {
     tone(BUZZER_PIN, 1000);
    buzzerState = HIGH;
    }
 else
 {
  noTone(BUZZER_PIN);
      buzzerState = LOW;

  }
}

void handleIRSensor() {
  int irState = digitalRead(IR_SENSOR_PIN);
  Serial.print("IR Sensor: ");
  Serial.println(irState);
  // Your code to handle the IR sensor data goes here
  if(irState == 0)
  {
       tone(BUZZER_PIN, 1000);
    buzzerState = HIGH; 
    }
  else
  {
     noTone(BUZZER_PIN);
      buzzerState = LOW;

    }
}

void handleUltrasonicSensor() {
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

  unsigned long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  unsigned int distance = duration / 29 / 2;

  Serial.print("Ultrasonic Distance: ");
  Serial.println(distance);
 if(distance <= 10)
 {
  tone(BUZZER_PIN, 1000);
    buzzerState = HIGH;
  
  }
 else
 {
   noTone(BUZZER_PIN);
      buzzerState = LOW;
  }
}

void handleRFIDSensor() {
  
  Serial.println("RFID reader initialized...");
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Select one of the cards
     noTone(BUZZER_PIN);
     buzzerState = LOW;
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("RFID DETECTED: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        tone(BUZZER_PIN, 1000);
        buzzerState = HIGH;
        delay(500);
        
        buzzerState = LOW;
      }
      noTone(BUZZER_PIN);
     buzzerState = LOW;
      Serial.println();  // Print a newline

      // Halt and stop reading the card to avoid multiple reads
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
  
    }
     else
   {noTone(BUZZER_PIN);
    buzzerState = LOW;}
  } 
  
  
}
int getSensorLED(int sensor) {
  switch (sensor) {
    case 0:
      return LED_TOUCH;
    case 1:
      return LED_PIR;
    case 2:
      return LED_IR;
    case 3:
      return LED_ULTRASONIC;
    case 4:
      return LED_RFID;
    default:
      return -1; // Invalid sensor
  }
}
