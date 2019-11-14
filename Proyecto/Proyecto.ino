/**
* Project members
* Esteban Gonzalez Tamayo
* Luis Angel Jaimes Mora
* David Garcia Contreras
*/

//libraries
#include <SPI.h>
#include <MFRC522.h>


//Pin Connection
#define relayInput D1 // the input to the relay pin
#define SS_PIN D4  //D2
#define RST_PIN D2 //D1

#include <Blynk.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "pzWcYBtaQLGaWdnwtSRcg4sJFsqjzwDv";

char ssid[] = "IoT-B19";
char pass[] = "meca2017*";


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

int statuss = 0;
int out = 0;
String tam [4] ={"","","",""}; //card register

void setup() 
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(relayInput, OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void loop() 
{
  Blynk.run();

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  Serial.print("card-> ");
  Serial.print(content);
  Serial.println();
  check(content);
}

BLYNK_WRITE(V3) 
{
  int a = param.asInt();
  Serial.println("Test"+a);
  if (a == 1) {
      Serial.println("Access Granted ");
      Serial.println("Welcome ");
      Serial.println("Door unlocked");
      digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
       } 
  if(a==0)
  {
      digitalWrite(relayInput, LOW); // turn relay off
      Serial.println("Door locked");
      Serial.println();
  } 
}

void check(String value)
{
  if(value=="")
  {}
  else
  {
  for(int j = 0; j < 4; j++)
  {
    Serial.print(value.equals(tam[j]));
    if (value.equals(tam[j])==1&&j<=4) //condition to check if the card can open the door
    {
      Serial.println("Access Granted ");
      Serial.println("Welcome ");
      Serial.println("Door unlocked");
      digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
      delay(5000);
      digitalWrite(relayInput, LOW); // turn relay off
      Serial.println("Door locked");
      Serial.println();
      statuss = 1;
      break;
    }else
    {
      Serial.println("Access Denied");  
    }
  }  
    
  }
}

void add1(String value)
{
  for(int i = 0; i < 4; i++)
  {
    Serial.println(value.equals(tam[i]));
    Serial.println("tam: "+tam[i]);
    if(value.equals(tam[i])==0&&i<4)
    {
      Serial.println("Adding card");
      if(tam[i]=="")
      {
        tam[i]= value;
        Serial.println("Success!! card added");
        break;
      }  
    }
    else if(i==3)
    {
      Serial.println("Sorry but you can only add 4 cards." );
    }
    else if(i<4)
    {
      Serial.println("Please remove the card, this card is already added.");
      if(value.equals(tam[i])==1)
      {
        break;
      }
    }
  }
}

BLYNK_WRITE(V2) {
  int a = param.asInt();
  Serial.println(a);
  if (a == 1) 
  {
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    Serial.println();
    Serial.print("card-> ");
    Serial.print(content);
    Serial.println();
    add1(content);
  } 
}
