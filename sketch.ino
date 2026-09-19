  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>
  #include <Servo.h>

  LiquidCrystal_I2C lcd(0x27, 16, 2); 
  Servo gateServo;

  const int trigPin = 3;        
  const int echoPin = 2;        
  const int greenLED = 4;       
  const int redLED = 5;         
  const int exitButtonPin = 6;  
  const int buzzerPin = 7;      
  const int servoPin = 9;       


  const int maxParkingSpaces = 5;       
  int availableSpaces = maxParkingSpaces; 
  const int activationDistance = 50;    
  bool isCarAtGate = false;             

  void setup() {
    
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(buzzerPin, OUTPUT); 
    
    pinMode(exitButtonPin, INPUT_PULLUP); 
    
    gateServo.attach(servoPin);
    gateServo.write(0); 
    
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    
    lcd.init();
    lcd.backlight();
    updateDisplay(); 
  }

  void loop() {
    long distance = readDistance();
    
    if (digitalRead(exitButtonPin) == LOW) {
      if (availableSpaces < maxParkingSpaces) {
        availableSpaces++;
        tone(buzzerPin, 2000, 100); 
        updateDisplay();
        delay(500); 
      }
    }

    if (distance > 0 && distance < activationDistance) {
      if (!isCarAtGate) {
        isCarAtGate = true; 
        
        if (availableSpaces > 0) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Rendszam OK!");
          lcd.setCursor(0, 1);
          lcd.print("Nyitas...");
          
          digitalWrite(greenLED, HIGH); 
          
          tone(buzzerPin, 1000, 150); 
          delay(200);
          tone(buzzerPin, 1500, 200);
          
          gateServo.write(90);          
          
          delay(3000); 
          
          availableSpaces--;
          gateServo.write(0);           
          digitalWrite(greenLED, LOW);  
          
          updateDisplay();
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Parkolo MEGTELT!");
          digitalWrite(redLED, HIGH);   
          
          tone(buzzerPin, 300, 1000); 
          
          delay(2000); 
          
          digitalWrite(redLED, LOW);    
          updateDisplay();
        }
      }
    } else {
      isCarAtGate = false;
    }
    
    delay(100);
  }

  long readDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 30000); 
    if (duration == 0) return 999; 
    
    long cm = duration * 0.034 / 2; 
    return cm;
  }

  void updateDisplay() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Szabad helyek:");
    lcd.setCursor(0, 1);
    if (availableSpaces > 0) {
      lcd.print(availableSpaces);
      lcd.print(" / ");
      lcd.print(maxParkingSpaces);
    } else {
      lcd.print("MEGTELT");
    }
  }