#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <TinyGPS++.h>
#include <DS3231.h>

#include <Keypad.h>

const int ROWS = 4; //four rows
const int COLS = 4; //four columns
int i=0;
int k[4];
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte rowPins[ROWS] = {8,9,10,11}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4,5,6,7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String inputString;
long inputInt;
DS3231  rtc(SDA, SCL);
static const uint32_t GPSBaud = 9600;

#define mySerial1 Serial3 
#define SS_PIN  53
#define RST_PIN 13
TinyGPSPlus gps;

#define mySerial Serial1
#define ss Serial2

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
int count=0;  
long randNumber;
int f=0;
String a ="";
String b="";
String c="";
String e="";
int t1=0;
String d;
String D;
String t;
 
void setup() 
{
  Serial.begin(9600); 
  rtc.begin();
  mySerial1.begin(9600);
  randomSeed(analogRead(0));
  inputString.reserve(10);
  ss.begin(GPSBaud); 
  SPI.begin();      
  mfrc522.PCD_Init();   
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  while (!Serial);  
  delay(100);
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } 
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

}

void loop()
{
  if (count==0)
  {
    rfid();
  }
  else if (count==1)
  {
    fingerprint();
  }
  else if (count==2)
  {
    t1 = otp();
    Serial.println(t1);
  }
  else if (count==3)
  {
    gps1();
  }
  else if (count==4)
  {
    
    timedisplay();
  }
  else if (count==5)
  {
    prints();
  }
  else if (count==6)
  {
    gsm();
  }
  else if (count==7)
  {
    keypade();
    Serial.println(inputInt);
  }
  else if (count==8)
  {
    compareotp();
  }
}




void rfid() 
{
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
 
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "23 4B B0 03") // Make sure you change this with your own UID number
  {
    Serial.println("Authorised access");
    a = "Kantharuban's Account:";
    count=1;
    Serial.println();
    delay(500);
  }
 
 else   {
    Serial.println(" Access denied");
    a="Unknown User";
    delay(500);
  }
}





void fingerprint() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER)  goto NoFinger; 
  else if (p == FINGERPRINT_OK) {
    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)  goto NoMatch;
    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)  goto NoMatch;
    Serial.print("Welcome! Your ID is ");
    count=2;
    
    Serial.println(finger.fingerID);
    if (finger.fingerID == 1)
    {
      b="Kantharuban's biometric ok;";
    }
    
    
    Serial.println(". You are granted access.");
    delay(1500);
    return; 
  }
  NoMatch: 
  {
  Serial.println("Access  Denied"); 
  b="Biometric failed;";
  delay(1000); 
  return;
  } 
  NoFinger: 
  {
  Serial.println("No finger detected");
  delay(1000);
  } 
}




int otp() {
  randNumber = random(999,9999);
  count=3;
  
  return(randNumber);
  delay(1000);
}



void gps1()
{
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    c=gps.location.lat();
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    e=gps.location.lng();
    count=4;
  }
  else
  {
    Serial.print(F("INVALID"));
    c="INVALID";
    e=";";
    count=4;
  }

  

  
  Serial.println();
}




void timedisplay()
{
  // Send Day-of-Week
  d=rtc.getDOWStr();
  Serial.print(d);
  Serial.print(" ");
  
  // Send date
  D=rtc.getDateStr();
  Serial.print(D);
  Serial.print(" -- ");

  // Send time
  t=rtc.getTimeStr();
  Serial.println(t);
  count=5;

  delay (200);
}


void prints()
{
  Serial.println(a);
  Serial.println(b);
  Serial.print(c);
  Serial.print(";");
  Serial.println(e);
  Serial.print(d);
  Serial.print("--");
  Serial.print(D);
  Serial.print("--");
  Serial.print(t);
  count=6;
}






void gsm()
{
   mySerial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
     delay(1000);  // Delay of 1 second
     mySerial1.println("AT+CMGS=\"+919360534685\"\r"); // Replace x with mobile number
     delay(1000);
     mySerial1.println(a);
      mySerial1.println(b);
      mySerial1.print(c);
      mySerial1.print(";");
      mySerial1.println(e);
      mySerial1.print(d);
      mySerial1.print("--");
      mySerial1.print(D);
      mySerial1.print("--");
      mySerial1.println(t);
     mySerial1.println("Your One time password is");// The SMS text you want to send
     mySerial1.println(randNumber);
     delay(100);
     mySerial1.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module 
      delay(1000);
      count=7;
}







void keypade() {
  char key = keypad.getKey();

  if (key) {
    

    if (key >= '0' && key <= '9') {     // only act on numeric keys
      inputString += key;               // append new character to input string
    } else if (key == '#') {
      if (inputString.length() > 0) {
        inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
        f = inputInt;
//        Serial.println(inputInt);
         count=8;
        inputString = "";               // clear input
        

      }
    } else if (key == '*') {
      inputString = "";                 // clear input
    }
  }
}






void compareotp()
{
  if (t1 == f )
  {
    Serial.println("Account login successful");
    count=9;
  }
  else
  {
    Serial.println("Account login failed; Wrong OTP");
    count=9;
  }
}
