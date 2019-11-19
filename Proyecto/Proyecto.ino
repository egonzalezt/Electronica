/**
    ───────────────────────────────────────────────────────────────────────────────────────────────
    ─██████████████─██████████████─██████████████─████████████───██████████████─████████████████───
    ─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░████─██░░░░░░░░░░██─██░░░░░░░░░░░░██───
    ─██░░██████░░██─██░░██████░░██─██░░██████████─██░░████░░░░██─██░░██████░░██─██░░████████░░██───
    ─██░░██──██░░██─██░░██──██░░██─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██────██░░██───   
    ─██░░██──██░░██─██░░██████░░██─██░░██████████─██░░██──██░░██─██░░██──██░░██─██░░████████░░██───
    ─██░░██──██░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░██──██░░██─██░░██──██░░██─██░░░░░░░░░░░░██───
    ─██░░██──██░░██─██░░██████████─██░░██████████─██░░██──██░░██─██░░██──██░░██─██░░██████░░████───
    ─██░░██──██░░██─██░░██─────────██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─────
    ─██░░██████░░██─██░░██─────────██░░██████████─██░░████░░░░██─██░░██████░░██─██░░██──██░░██████─ 
    ─██░░░░░░░░░░██─██░░██─────────██░░░░░░░░░░██─██░░░░░░░░████─██░░░░░░░░░░██─██░░██──██░░░░░░██─
    ─██████████████─██████─────────██████████████─████████████───██████████████─██████──██████████─
    ───────────────────────────────────────────────────────────────────────────────────────────────
*/

//remove the logo when you put the code on arduino IDE

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

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

char auth[] = ""; //BLYNK device token
char ssid[] = ""; //network ssid
char pass[] = ""; //network password  

int statuss = 2;  //check current status of the system
String tam [4] ={"","","",""}; //card register

void setup() 
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);  //Blynk initialization
  pinMode(relayInput, OUTPUT);  //setup relay pinmode
  Serial.begin(9600); // Initiate a serial communication
  SPI.begin();  // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  Blynk.virtualWrite(V4, "Welcome");  //Put a message on blynk 16x2 virtual screen
  Blynk.virtualWrite(V5, " Opedor System"); //Put a message on blynk 16x2 virtualscreen
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
    Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
    check(cardRd());
  }
}

/**
Method card, read the card on the Rfid reader to get the id of the card
@return content that means the id of the card

other methods that need it 
  -check
  -Add
  -Delete
*/

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

/**
  Method check the function of this method is check if the card on the reader has the permissions to open and close the door 
*/

void check(String value)
{
  if(value=="")
  {}
  else
  {
    for(int j = 0; j < 4; j++)  //loop to compare the card with the registered cards
    {
      Serial.print(value.equals(tam[j])); //condition to compare the card with the registered cards
      if (value.equals(tam[j])==1&&j<=4) //condition to check if the card can open the door
      {
        Blynk.virtualWrite(V4, "Welcome Door"); //write a message on screen
        Blynk.virtualWrite(V5, "Open"); //write a message on screen
        Serial.println("Access Granted ");
        Serial.println("Welcome ");
        Serial.println("Door unlocked");
        digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
        delay(5000);
        digitalWrite(relayInput, LOW); //turn ralay off to desactivate the solenoid
        Blynk.virtualWrite(V4, "Door"); //write a message on screen
        Blynk.virtualWrite(V5, "Locked"); //write a message on screen
        Serial.println("Door locked");
        Serial.println();
        statuss = 1;
        break;
      }
      else
      {
        Serial.println("Access Denied");
        Blynk.virtualWrite(V4, "Access Denied");  //write a message on screen
      }
    }   
  }
  Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
  Blynk.virtualWrite(V5, " "); //clear the screen
}

/**
  * Method to register RFID cards, this method is activated only when the user presses the button
*/
void Add(String value)
{
  if(value=="")
  {
    Serial.println("Please put the card on the card reader");  
  }
  else
  {
    for(int i = 0; i < 4; i++)  //loop to add only 4 cards 
    {
      if(value.equals(tam[i])==0&&i<4)  //conditional to check if the card is already registered
      {
        if(tam[i]=="")  //condition to check if there is a space to add the card
        {
          Blynk.virtualWrite(V4, "Adding card");  //write a message
          Serial.println("Adding card");
          tam[i]= value;
          Blynk.virtualWrite(V4, "Card added"); //write a message on screen
          Serial.println("Success!! card added");
          Blynk.virtualWrite(V5, "Remove the card");  //write a message on screen
          Serial.println("please remove the card from the reader");
          break;
        }  
      }
      else if(value.equals(tam[i])==1&&i<4) //condition to send a message to the user notified that the card is already added
      {
        Blynk.virtualWrite(V4, "This card "); //write a message on screen
        Blynk.virtualWrite(V5, "is already added.");  //write a message on screen
        break;
      }
    }
  }
  delay(2000);
  Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
  Blynk.virtualWrite(V5, " ");  //clear the screen
}

/**
  Method delete, this method is used to remove a card from the system to denied it the access.
*/

void Delete(String value)
{
  if(value=="")
  {
    Serial.println("Please put the card on the card reader");  
  }
  else
  {
    for(int i = 0; i < 4; i++)  //loop to search the card  
    {
      if(value.equals(tam[i])==1&&i<4) // conditional to verify if the required card was found
      {
        Blynk.virtualWrite(V4, "Deleting card");  //write a message on screen
        Serial.println("Deleting card");
        tam[i]= "";
        Blynk.virtualWrite(V4, "Card deleted"); //write a message on screen
        Serial.println("Success!! card deleted");  
        Blynk.virtualWrite(V5, "Remove the card");  //write a message on screen 
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
  Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
  Blynk.virtualWrite(V5, " ");  //clear the screen
}

/**
BLYNK_WRITE(V1) 
this function of BLYNK is used to delete card pressing a virtual button on the BLYNK APP, 
if the button didn't pressed the system never delete a card.
*/

BLYNK_WRITE(V1) 
{
  statuss=1;
  int a = param.asInt();  //read the virtual status -> if is 0 or 1, if it is 1 the system proceed to delete the card
  if (a == 1) 
  {
    Blynk.virtualWrite(V4, "Put the card"); //write a message on screen
    Blynk.virtualWrite(V5, "on the reader");  //write a message on screen
    Serial.println("Please put the card on the reader");
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent()) //condition to notify to the user if the card isn't on the RFID reader
    {
      Blynk.virtualWrite(V4, "Card"); //write a message on screen
      Blynk.virtualWrite(V5, "not detected"); //write a message on screen
      Serial.println("Card not detected"); 
    }
    else
    {
      Delete(cardRd()); 
      Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
      Blynk.virtualWrite(V5, " ");  //clear the screen
    }
  }
  statuss=0; 
}

/**
BLYNK_WRITE(V2) 
this function of BLYNK is used to register a card on the RFID reader pressing a virtual button on the BLYNK APP, 
if the button didn't pressed the system never add a new card.
*/

BLYNK_WRITE(V2) 
{
  statuss=1;
  int a = param.asInt();  //read the virtual status -> if is 0 or 1, if it is 1 the system proceed to register the new card
  if (a == 1) 
  {
    Serial.println("Please put the card on the reader");
    Blynk.virtualWrite(V4, "Put the card"); //write a message on screen
    Blynk.virtualWrite(V5, "on the reader");  //write a message on screen
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent())  //condition to notify to the user if the card isn't on the RFID reader
    {
      Blynk.virtualWrite(V4, "Card"); //write a message on screen
      Blynk.virtualWrite(V5, "not detected"); //write a message on screen
      Serial.println("Card not detected"); 
    }
    else
    {
      Add(cardRd());
      Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
      Blynk.virtualWrite(V5, " ");  //clear the screen
    }
  }
  statuss=0; 
}

/**
BLYNK_WRITE(V3) 
this function of BLYNK is used to open and close the door pressing a virtual button on the BLYNK APP, 
if the button didn't pressed the system never close or open the door.
*/

BLYNK_WRITE(V3) 
{
  statuss=1;
  int a = param.asInt();  //read the virtual status -> if is 0 or 1, if it is 1 the system proceed to open the door else the
                          //system proceed to close the door
  if (a == 1) 
  {
    Serial.println("Access Granted ");
    Serial.println("Welcome ");
    Serial.println("Door unlocked");
    Blynk.virtualWrite(V4, "Access Granted"); //write a message on screen
    delay(500);
    Blynk.virtualWrite(V5, "Door unlocked");  //write a message on screen
    Blynk.virtualWrite(V4, "Welcome"); //write a message on screen
    digitalWrite(relayInput, HIGH); //turn ralay on to activate the solenoid
    delay(2000);
    Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
    Blynk.virtualWrite(V5, " ");  //clear the screen
  } 
  if(a==0)
  {
    Blynk.virtualWrite(V4, "Door locked");  //write a message on screen
    digitalWrite(relayInput, LOW); //turn ralay off to desactivate the solenoid
    Serial.println("Door locked");
    Serial.println();
    delay(2000);
    Blynk.virtualWrite(V4, " Opedor System"); //write a message on screen
  }
  statuss=0;
}
