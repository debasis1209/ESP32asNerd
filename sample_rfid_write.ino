#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 0                        /* selecting the GPIO pin no. 0 as  Reset Pin of MFRC522 */
#define SS_PIN 5                         /* selecting the GPIO pin no. 5 as Slave Select or Chip Select of MFRC522 */ 
MFRC522 rfid(SS_PIN, RST_PIN);            /* creating an instance of MFRC522 class */

void setup() {
  Serial.begin(9600);                    /* begin the serial at 9600 baud rate */
  while(! Serial){};                      /* if serial has not started then do nothing */
  SPI.begin();                            /* begin the SPI communication */
  rfid.PCD_Init();                          /* initialize the RFID reader or start the RFID  communication */
  Serial.println(F("Writing to the NFC TAG..."));
}

void loop() {
  MFRC522::MIFARE_Key key;                                /* create an instance to match the key with the key of the tag */
  for(byte i = 0; i < 6;i++) key.keyByte[i] = 0xff;      /* assigning the key to match wtith the tag. NOTE: factory mode default to ffffffh */

  if(! rfid.PICC_IsNewCardPresent()){return;}            /* checking the new card is present or not */
  if(! rfid.PICC_ReadCardSerial()){return;}              /* selecting a card from the number of cards by reading its serial number */

  /* printing the card uid */
  Serial.println(F("card uid:"));
  for(byte i = 0; i < rfid.uid.size;i++){
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : " ");
    Serial.print(rfid.uid.uidByte[i],HEX);
  }

  /* priniting the card PICC type */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type picctype = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(picctype));

  byte buffer[18];                                            /* buffer to save data before load to nfc tag.NOTE: for each 16bytes 2 bytes extra required for start and stop byte */
  byte block;                                                  /* selecting the block in nfc tag to load */
  MFRC522::StatusCode status;                                 /* instance to check the status of authentication and writing */
  byte len;                                                  /* length of data to load */

  Serial.setTimeout(20000L);                                  /* set timeout for write some data from serial */
  len = Serial.readBytesUntil('#',(char*)buffer,16);          /* finding the length of the data actually consumed from the serial for user.NOTE: if you want refer ACII table */
  for(byte i = len; i < 16;i++) buffer[i] = ' ';              /* rest of the assigned bytes will be padded by zeroes */

  block = 4;                                                  /* selecting block 4 to write */
  /* checking the status of authentication for writing using key B */
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key, &(rfid.uid));
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("PICC authentication failed with key B."));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  else Serial.println("reader authentication success with key B.");

  /* writing and checking the status of successful write */
  status = rfid.MIFARE_Write(block, buffer, 16);
  if(status != MFRC522::STATUS_OK){
    Serial.print(F("Write Operation failed."));
    Serial.print(rfid.GetStatusCodeName(status));
    return;
  }
  else Serial.println("writing authentication successful to the nfc tag at block 4.");

  rfid.PICC_HaltA();        /* halt the PICC */
  rfid.PCD_StopCrypto1();    /* halt the PCD */
}
