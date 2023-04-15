// Include necessary libraries
#include <Keypad.h>             // Library for using Keypad
#include <Password.h>           // Library for using Password
#include <MFRC522.h>            // Library for using RFID
#include <SPI.h>                // Library for using SPI

// Define pins for various components
#define pirPin 31               // PIR Sensor
#define RST_PIN 6               // RFID Reset Pin
#define SS_PIN  53              // RFID SS Pin
#define Beep 43                 // Alarm Buzzer
#define LED_E  41               // Entry/Exit LED
#define LED_Z2 38               // Zone 2 LED
#define LED_Z1 30               // Zone 1 LED
#define LED_GF 34               // Green LED on the front of the model to indicate if the alarm is armed or not
#define LED_RF 42               // Red LED on the front of the model to indicate if the alarm has been activated or not

// Initialize RFID variables
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create RFID object
boolean RFIDMode = true;            // Boolean to change modes 

// Initialize Zone 1 variables
const int knockSensor = A1;         // Knock Sensor Pin
const int knockSensor2 = A4;
const int threshold = 180;          // Knock Sensor Threshold
//int sensorReading = 0;              
//int knockValue=0;

// Initialize Temperature Sensor variables
int ThermistorPin = A2;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// Initialize alarm state
bool Alarm_State=false;

// Initialize My_Delay variables
int period = 5000;                  // Delay period in ms
unsigned long time_now = 0;

// Initialize Keypad variables
const byte ROWS = 4;                // Number of rows
const byte COLS = 4;                // Number of columns
char keys[ROWS][COLS] = {           // Define keypad layout
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25};  // Connect rows to these pins
byte colPins[COLS] = {26, 27, 28, 29};  // Connect columns to these pins
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); // Create keypad object

// Initialize variables to hold the new password
String newPasswordString; 
char newPassword[6]; 
Password password = Password("1616");  // Initialize password to "1616"
byte maxPasswordLength = 6;            // Maximum length of password
byte currentPasswordLength = 0;        // Current length of password


void setup() {
// Start serial communication at 9600
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
// Initialize SPI bus and MFRC522 RFID reader  
  SPI.begin();         
  mfrc522.PCD_Init();
// Set the LED and Beeper pins as an output.
  pinMode(LED_E, OUTPUT);
  pinMode(LED_Z1, OUTPUT);
  pinMode(LED_Z2, OUTPUT);
  pinMode(LED_RF, OUTPUT);
  pinMode(LED_GF, OUTPUT);
  pinMode(Beep, OUTPUT);
    
}


void loop(){
 // Call various functions in loop
  my_key();    // Handle keypad inputs
  Entry_Exit();    // Check entry/exit zone for motion
  Zone_1();    // Check zone 1 for knock detection
  emergency();    // Check for overheating
  RFID();    // Check for RFID card present

}                



//Entry_Exit is a delay zone
void Entry_Exit(){
  // Check if alarm is activated and motion is detected
  if (Alarm_State == 1 && digitalRead(pirPin) == HIGH) {
    // Print message to serial monitor and activate LED E
    Serial.println("Main Door: Motion detected.");
    my_key();
    digitalWrite(LED_E, HIGH);
    countdown();    // Start countdown to disarm alarm
    if (Alarm_State == 0) {
      return;
    }
    RFID();    // Check for RFID card present
    TurnOnAlarm();    // Activate alarm
    }
  }


//Zone 1 is an immediate zone
void Zone_1(){
 // Check if alarm is activated and knock sensor threshold is exceeded
  if (Alarm_State == 1 && analogRead(knockSensor) >= threshold) {
    // Print message to serial monitor and activate LED Z1
    Serial.println("Zone 1 Detection");
    TurnOnAlarm();    // Activate alarm
    digitalWrite(LED_Z1, HIGH);
    my_key();
    }
  }

//Emergency Zone
void emergency(){
  // Read thermistor sensor value and calculate temperature
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  if(Tc >50){
    Serial.println(Tc);    
  }
  if(Tc>100){
  // Check if temperature is above 100 degrees 
    // Activate alarm and print message to serial monitor
    Alarm_State = 1;
    Serial.println("Emergency Zone 2");
    digitalWrite(LED_Z2, HIGH);
    
    TurnOnAlarm();    // Activate alarm
    RFID();    // Check for RFID card present
    My_Delay();    // Wait for some time
  }
  if (analogRead(knockSensor2) >= threshold) {
    // Activate alarm and print message to serial monitor
    Alarm_State = 1;
    Serial.println("Emergency Zone 2");
    digitalWrite(LED_Z2, HIGH);
    RFID();    // Check for RFID card present
    TurnOnAlarm();    // Activate alarm
    if(Alarm_State ==0){
      return;
    }
  }
  //My_Delay();    // Wait for some time
    
  
}

void My_Delay(){
  time_now = millis();
  while(millis() < time_now + period){
        //wait approx. [period] ms
    if(Alarm_State ==0){
      return;
    }      
  }
}

void countdown(){
  // The loop counts down 
  for(int x = 1; x<6; x++){
    if (Alarm_State == 0) {
      return;
    }
    // Print out the current count to the serial monitor
    Serial.print("Alarm Activating in: "); 
    Serial.println(6-x);
    // Call the function "my_key" to handle any inputs from the keypad
    my_key();
    RFID();
    // Delay for 1 second
    delay(1000);
  }
}
// Function to turn on the alarm
void TurnOnAlarm(){
  Serial.println("Alarm Activated"); // Print a message to the Serial Monitor
  digitalWrite(LED_GF,LOW); // Turn off the LED connected to the GF pin
  digitalWrite(LED_RF,HIGH); // Turn on the LED connected to the RF pin
  digitalWrite(Beep,HIGH); // Turn on the beep sound
  my_key(); // Call the function to check if the correct key is inserted
  RFID(); // Call the function to check the RFID card
}

// Function to turn off the alarm
void TurnOffAlarm(){
  Serial.println("Alarm Disarmed"); // Print a message to the Serial Monitor
  digitalWrite(Beep,LOW); // Turn off the beep sound
  digitalWrite(LED_RF,LOW); // Turn off the LED connected to the RF pin
  digitalWrite(LED_E,LOW); // Turn off the LED connected to the E pin
  digitalWrite(LED_Z1,LOW); // Turn off the LED connected to the Z1 pin
  digitalWrite(LED_Z2,LOW); // Turn off the LED connected to the Z2 pin
  digitalWrite(LED_GF,LOW); // Turn off the LED connected to the GF pin
  Alarm_State=false; // Set the alarm state to false
}


void RFID(){

  // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  //Serial.println();
  Serial.println("Card Read OK!!");
  Alarm_State = !Alarm_State;
    //Serial.println(Alarm_State);
    if (Alarm_State == 1){
      Serial.println("System Armed!!!");
      digitalWrite(LED_GF,HIGH);
    }  
    else{
      TurnOffAlarm();
    }

  // Halt PICC and re-select it so DumpToSerial doesn't get confused
  mfrc522.PICC_HaltA();
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    return;
  }
  delay(500);

}
// my_key function to get a key from the keypad
void my_key(){
  // Read the key from the keypad
  char key = keypad.getKey();

  // If a key is pressed
  if (key != NO_KEY){
      // Wait for 60 ms
      delay(60); 
      switch (key){
        case 'A': 
          // Do nothing if key 'A' is pressed
          break; 
        case 'B': 
          // Do nothing if key 'B' is pressed
          break; 
        case 'C': 
          // Do nothing if key 'C' is pressed
          break; 
        case 'D': 
          // Do nothing if key 'D' is pressed
          break; 
        case '#': 
          // Check the password if key '#' is pressed
          checkPassword(); 
          break;
        case '*': 
          // Reset the password if key '*' is pressed
          resetPassword(); 
          break;
        default: 
          processNumberKey(key);
      }
  }
}

// processNumberKey function to process number keys
void processNumberKey(char key) {
   // Print the key to the serial monitor
   Serial.print(key);
   // Increase the length of the current password
   currentPasswordLength++;
   // Append the key to the password
   password.append(key);
   // If the length of the current password reaches the maximum length
   if (currentPasswordLength == maxPasswordLength) {
      // Check the password
      checkPassword();
   } 
}

// checkPassword function to check the password
void checkPassword() {
   // If the password is correct
  if (password.evaluate()){
    Serial.println(" Correct!");
    Alarm_State = !Alarm_State;
    //Serial.println(Alarm_State);
    if (Alarm_State == 1){
      Serial.println("System Armed!!!");
      digitalWrite(LED_GF,HIGH);
      }  
    else{
      TurnOffAlarm();
      }
  }  
  else {
      // Print "Wrong password!" to the serial monitor
      Serial.println(" Wrong password!");
   }
   // Reset the password
   resetPassword();
}

void resetPassword() {
   // Reset the password
   password.reset(); 
   // Reset the length of the current password
   currentPasswordLength = 0; 
}

