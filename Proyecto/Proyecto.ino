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
char ssid[] = "WISPEREXT";
char pass[] = "wispergt2020";


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

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
}

void loop() 
{
  Blynk.run();
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()) 
  {
    Serial.print("Hola");
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  if(statuss==1)
  {
    Serial.println(".");
  }
  else
  {
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
      }
      else
      {
        Serial.println("Access Denied");  
      }
    }   
  }
}

void add1(String value)
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
          Serial.println("Adding card");
          tam[i]= value;
          Serial.println("Success!! card added");
          Serial.println("please remove the card from the reader");
          break;
        }  
      }
      else if(i==3)
      {
        Serial.println("Sorry but you can only add 4 cards." );
      }
    }
  }
  delay(2000);
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
        Serial.println("Deleting card");
        tam[i]= "";
        Serial.println("Success!! card deleted");
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
}

BLYNK_WRITE(V1) 
{
  statuss=1;
  int a = param.asInt();
  if (a == 1) 
  {
    Serial.println("Please put the card on the reader");
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent())
    {
      Serial.println("Card not detected"); 
    }
    else
    {
      Delete(cardRd());      
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
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent())
    {
      Serial.println("Card not detected"); 
    }
    else
    {
      add1(cardRd());
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
    digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
  } 
  if(a==0)
  {
    digitalWrite(relayInput, LOW); // turn relay off
    Serial.println("Door locked");
    Serial.println();
  }
  statuss=0;
}
