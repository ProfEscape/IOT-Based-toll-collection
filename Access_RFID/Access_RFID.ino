//Code for the Smart Toll Gate System Project
#include<Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_SSD1306.h>

//Declaring the pins
constexpr uint8_t SS_PIN = D4;
constexpr uint8_t RST_PIN = D3;
constexpr uint8_t IR_PIN = D0;
constexpr uint8_t SERVO_PIN = D8;

// Init array that will store new NUID 
String tag = "";
String vehicleType = "";
//Balance of vehicle owners
int amt53 = 2000;
int amt45 = 1000;
int amt24 = 800;

//toll amount
int smallVehicle = 100;
int mediumVehicle = 200;
int largeVehicle = 300;

// instance of display
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 

//Instance of the servo
Servo servo_1;

//functions
int deductTax(int amount, String vehicle_type);
void scanCard(int amount, String honor_address);
void servoDown();
void servoUp();

void setup() { 
  Serial.begin(9600);

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for 0.5 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
  display.clearDisplay();

  //attaching the servo
  servo_1.attach(D8); 
  delay(100);
  servo_1.detach();

  //defining the IR pin
  pinMode(IR_PIN,INPUT);
}
 
void loop() {

  int irInput = digitalRead(IR_PIN);
  tag ="";

  if (irInput == LOW ){
    servoDown();             //calling the servoDown function 
    Serial.println("IR Working");
    display.clearDisplay();
    display.setCursor(32,0);
    display.print("WELCOME TO");
    display.setCursor(30,10);
    display.print("JSSSTU TOLL");
    display.setCursor(15,30);
    display.print("Please scan your");
    display.setCursor(40,40);
    display.print("fast tag");
    
    display.display();

    while (!rfid.PICC_ReadCardSerial()){
      if (rfid.PICC_IsNewCardPresent())
          Serial.println("Card detected");

      if (rfid.PICC_ReadCardSerial()){
        for (byte i=0; i <4; i++){
          tag += rfid.uid.uidByte[i];
        }
        Serial.println("Card read");
        break;
      }
    }

    if (tag == "195196235253"){
      vehicleType = "large";
      amt53 = deductTax(amt53, vehicleType);           //Deducting the amount
      scanCard(amt53, "Sir");  //calling the scanCard function to display the message
    }
    else if(tag == "5111085245"){
      vehicleType = "medium";
      amt45 = deductTax(amt45, vehicleType);     //Deducting the tax amount
      scanCard(amt45, "Ma'am");  //calling the scanCard function
    }
    else if(tag == "137111149124"){
      vehicleType = "small";
      amt24 = deductTax(amt24, vehicleType);      //Deducting the amount
      scanCard(amt24, "Ma'am");  //calling the scanCard function to display the message
    }
    else{
      display.clearDisplay();
      display.setCursor(3,0);
      display.print("Card not recognized!");
      display.setCursor(10,20);
      display.print(" Please register          yourself.");
      display.display();
    }
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

/************Functions***************/
int deductTax(int amount, String vehicle_type){
  if (vehicle_type == "small")
    amount -= smallVehicle;
  else if (vehicle_type == "medium")
    amount -= mediumVehicle;
  else if (vehicle_type == "large")
    amount -= largeVehicle;

  return amount;
}

void scanCard(int amount, String honor){

  if (amount >= 0){
    String message = "Remaining points:";
    message = message + String(amount);

    servoUp();              //move the servo shaft up
    Serial.println("Opening the gate");
    display.clearDisplay();
    display.setCursor(40,0);
    display.print("TOLL PAID!");
    display.setCursor(0,10);
    display.print(message);
    display.setCursor(20,30);
    display.print("THANK YOU, "+honor+".");
    display.setCursor(0,40);
    display.print("HAVE A SAFE JOURNEY!");
    display.display();
  }
  else{
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("INSUFFICIENT AMOUNT!");
    display.setCursor(0,30);
    display.print("Please recharge      through the helpline.");
    display.display();
  }
}

void servoDown(){ 
  servo_1.attach(D8);
  servo_1.write(0);
  servo_1.detach();
}

void servoUp(){
  servo_1.attach(D8);
  servo_1.write(180);
  servo_1.detach();
}
