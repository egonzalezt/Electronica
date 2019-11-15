/**
* Project members
* Esteban Gonzalez Tamayo
* Luis Angel Jaimes Mora
* David Garcia Contreras
*/

//libraries
#include <SPI.h>
#include <MFRC522.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Pin Connection
#define relayInput D1 // the input to the relay pin
#define SS_PIN D4  //D2
#define RST_PIN D2 //D1

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

char auth[] = "pzWcYBtaQLGaWdnwtSRcg4sJFsqjzwDv";
char ssid[] = "WISPEREXT";
char pass[] = "wispergt2020";

int statuss = 2;
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
  Blynk.virtualWrite(V4, "Welcome");
  Blynk.virtualWrite(V5, " Opedor System");
}

void loop() 
{
  Blynk.run();
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  if(statuss!=1)
  {
    Blynk.virtualWrite(V4, " Opedor System");
    check(cardRd());
  }
}

String cardRd()
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
  Serial.print("card-> "+content);
  Serial.println();
  return content;
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
        Blynk.virtualWrite(V4, "Welcome Door");
        Blynk.virtualWrite(V5, "Open");
        Serial.println("Access Granted ");
        Serial.println("Welcome ");
        Serial.println("Door unlocked");
        digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
        delay(5000);
        digitalWrite(relayInput, LOW); // turn relay off
        Blynk.virtualWrite(V4, "Door");
        Blynk.virtualWrite(V5, "Locked");
        Serial.println("Door locked");
        Serial.println();
        statuss = 1;
        break;
      }
      else
      {
        Serial.println("Access Denied");
        Blynk.virtualWrite(V4, "Access Denied");  
      }
    }   
  }
  Blynk.virtualWrite(V4, " Opedor System");
  Blynk.virtualWrite(V5, " ");
}

void Add(String value)
{
  if(value=="")
  {
    Serial.println("Please put the card on the card reader");  
  }
  else
  {
    for(int i = 0; i < 4; i++)
    {
      if(value.equals(tam[i])==0&&i<4)
      {
        if(tam[i]=="")
        {
          Blynk.virtualWrite(V4, "Adding card");
          Serial.println("Adding card");
          tam[i]= value;
          Blynk.virtualWrite(V4, "Card added");
          Serial.println("Success!! card added");
          Blynk.virtualWrite(V5, "Remove the card");
          Serial.println("please remove the card from the reader");
          break;
        }  
      }
      else if(value.equals(tam[i])==1&&i<4)
      {
        Blynk.virtualWrite(V4, "This card ");
        Blynk.virtualWrite(V5, "is already added.");
        break;
      }
    }
  }
  delay(2000);
  Blynk.virtualWrite(V4, " Opedor System");
  Blynk.virtualWrite(V5, " ");
}

void Delete(String value)
{
  if(value=="")
  {
    Serial.println("Please put the card on the card reader");  
  }
  else
  {
    for(int i = 0; i < 4; i++)
    {
      if(value.equals(tam[i])==1&&i<4)
      {
        Blynk.virtualWrite(V4, "Deleting card");
        Serial.println("Deleting card");
        tam[i]= "";
        Blynk.virtualWrite(V4, "Card deleted");
        Serial.println("Success!! card deleted");
        Blynk.virtualWrite(V5, "Remove the card");
        Serial.println("please remove the card from the reader"); 
        break;
      }
      else
      {
        Serial.println("Error");
      }
    }
  }
  delay(2000);
  Blynk.virtualWrite(V4, " Opedor System");
  Blynk.virtualWrite(V5, " ");
}

BLYNK_WRITE(V1) 
{
  statuss=1;
  int a = param.asInt();
  if (a == 1) 
  {
    Blynk.virtualWrite(V4, "Put the card");
    Blynk.virtualWrite(V5, "on the reader");
    Serial.println("Please put the card on the reader");
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent())
    {
      Blynk.virtualWrite(V4, "Card");
      Blynk.virtualWrite(V5, "not detected");
      Serial.println("Card not detected"); 
    }
    else
    {
      Delete(cardRd()); 
      Blynk.virtualWrite(V4, " Opedor System");
      Blynk.virtualWrite(V5, " ");     
    }
  }
  statuss=0; 
}

BLYNK_WRITE(V2) 
{
  statuss=1;
  int a = param.asInt();
  if (a == 1) 
  {
    Serial.println("Please put the card on the reader");
    Blynk.virtualWrite(V4, "Put the card");
    Blynk.virtualWrite(V5, "on the reader");
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent())
    {
      Blynk.virtualWrite(V4, "Card");
      Blynk.virtualWrite(V5, "not detected");
      Serial.println("Card not detected"); 
    }
    else
    {
      Add(cardRd());
      Blynk.virtualWrite(V4, " Opedor System");
      Blynk.virtualWrite(V5, " ");
    }
  }
  statuss=0; 
}

BLYNK_WRITE(V3) 
{
  statuss=1;
  int a = param.asInt();
  if (a == 1) 
  {
    Serial.println("Access Granted ");
    Serial.println("Welcome ");
    Serial.println("Door unlocked");
    Blynk.virtualWrite(V4, "Access Granted");
    delay(500);
    Blynk.virtualWrite(V5, "Door unlocked");
    Blynk.virtualWrite(V4, "Welcome       ");
    digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
    delay(2000);
    Blynk.virtualWrite(V4, " Opedor System");
    Blynk.virtualWrite(V5, " ");
  } 
  if(a==0)
  {
    Blynk.virtualWrite(V4, "Door locked");
    digitalWrite(relayInput, LOW); // turn relay off
    Serial.println("Door locked");
    Serial.println();
    delay(2000);
    Blynk.virtualWrite(V4, " Opedor System");
  }
  statuss=0;
}
