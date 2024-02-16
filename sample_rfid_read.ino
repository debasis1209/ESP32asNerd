#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 0
#define SS_PIN 5
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);    /* begin the serial */
  while(! Serial){};      /* if serial does not start do not do anything */
  SPI.begin();            /* begin the SPI communication */
  rfid.PCD_Init();        /* initialize the rfid */
  Serial.println(F("reading from the NFC TAG..."));
}

void loop() {
  MFRC522::MIFARE_Key key;                                  /* creating an instance to match the key with the nfc tag */
  for(byte i = 0; i < 6;i++) key.keyByte[i] = 0xff;          /* as factory mode has default key of fffffffh so we assigned the array of key instance to ffffffffh.(NOTE:key A and Key B is 6 bytes of length */

  byte block;                                  /* created an instance to select the block to read */
  byte len;                                    /* created an instance length of data to read */ 
  MFRC522::StatusCode status;                  /* created an instance to check status of the reading and authentication of nfc tag.(NOTE: without authentication you can't read) */

  if(! rfid.PICC_IsNewCardPresent()){return;}    /* checking for the new card present in the proximity */
  if(! rfid.PICC_ReadCardSerial()){return;}      /* select a card from the number of available cards by reading the serial of the card */
  Serial.println(F("Card Detected..."));
  rfid.PICC_DumpDetailsToSerial(&(rfid.uid));      /* dump the card's uid to Serial monitor of the Arduino IDE */

  Serial.println(F("**Card Detected:**"));
  Serial.print(F("ID is: "));
  
  byte buffer[20];                                  /* setting the buffer size to read from FIFO from the NFC tag */
  block = 4;                                        /* setting the block to read */      
  len = 20;                                        /* length of data to read */

  /* checking the status of authentication with key B using local instance key */
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 4, &key, &(rfid.uid));          
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("authentication successful by key B"));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  /* checking the status of read from the FIFO */
  status = rfid.MIFARE_Read(block, buffer, &len);
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("reading failed."));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  /* printing the reading bytes from the FIFO to serial monitor */ 
  for(uint8_t i = 0; i < len;i++){
    if(buffer[i] != 20)
    {
      Serial.write(buffer[i]);
    }
  }
  Serial.print(" ");
  delay(1000);
  rfid.PICC_HaltA();              /* halt the card. PICC:- Proximity Inductive Coupling Card */ 
  rfid.PCD_StopCrypto1();          /* halt the reader. PCD:- Proximity Coupling Device */ 
  
}
