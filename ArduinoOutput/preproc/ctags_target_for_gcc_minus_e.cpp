# 1 "c:\\Github\\PN532-Practice\\PN532_Write\\PN532_Write.ino"
# 2 "c:\\Github\\PN532-Practice\\PN532_Write\\PN532_Write.ino" 2
# 3 "c:\\Github\\PN532-Practice\\PN532_Write\\PN532_Write.ino" 2
# 4 "c:\\Github\\PN532-Practice\\PN532_Write\\PN532_Write.ino" 2







Adafruit_PN532 nfc(18, 19, 23, 5);


char * url = "G1P7"; //업로드 할 카드 정보
uint8_t ndefprefix = (0x01);


void setup(void) {
  Serial.begin(115200);
  Serial.println("--------------RFID Writeover Mode initiated--------------");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();

  if (! versiondata) {
    Serial.print("Didn't find PN532 board");
    while (1); // halt
  }

  Serial.print("Found PN532 board");
  nfc.SAMConfig();
}

void loop(void)
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t dataLength;

  Serial.println("\r\nPlace your NDEF formatted NTAG2xx tag on the reader to update the");
  Serial.println("NDEF record and press any key to continue ...\r\n");
  // Wait for user input before proceeding
  while (!Serial.available());
  // a key was pressed1
  while (Serial.available()) Serial.read();

  // 1.) Wait for an NTAG203 card.  When one is found 'uid' will be populated with
  // the UID, and uidLength will indicate the size of the UID (normally 7)
  success = nfc.readPassiveTargetID((0x00), uid, &uidLength);

  if (success)
  {
    Serial.println("Found an ISO14443A card");

    if (uidLength != 7)
    {
      Serial.println("This doesn't seem to be an NTAG203 tag");
    }
    else
    {
      uint8_t data[32];
      Serial.println("Seems to be an NTAG2xx tag");
      memset(data, 0, 4);
      success = nfc.ntag2xx_ReadPage(3, data);

      if (!success)
      {
        Serial.println("Unable to read the Capability Container (page 3)");
        return;
      }
      else
      {
        if (!((data[0] == 0xE1) && (data[1] == 0x10)))
        {
          Serial.println("This doesn't seem to be an NDEF formatted tag.");
          Serial.println("Page 3 should start with 0xE1 0x10.");
        }
        else
        {
          dataLength = data[2]*8;
          Serial.println("Tag is NDEF formatted.");
          Serial.print("Erasing previous data area ");
          for (uint8_t i = 4; i < (dataLength/4)+4; i++)
          {
            memset(data, 0, 4);
            success = nfc.ntag2xx_WritePage(i, data);
            Serial.print(".");
            if (!success)
            {
              Serial.println(" ERROR!");
              return;
            }
          }
          Serial.println(" DONE!");

          Serial.print("Writing URI as NDEF Record ... ");
          success = nfc.ntag2xx_WriteNDEFURI(ndefprefix, url, dataLength);
          if (success)
          {
            Serial.println("DONE!");
          }
          else
          {
            Serial.println("ERROR! (URI length?)");
          }

        } // CC contents NDEF record check
      } // CC page read check
    } // UUID length check

    // Wait a bit before trying again
    Serial.flush();
    while (!Serial.available());
    while (Serial.available()) {
    Serial.read();
    }
    Serial.flush();
  }
}
