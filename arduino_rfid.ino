#include <SPI.h>
#include <MFRC522.h>

//LED
#define LED_PIN  7
//RFID PINS
#define RST_PIN  9
#define SS_PIN   10

//Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//UID
//UID buffer
int UID_BUF[7];

//CID
//currentTag's CID(Custom ID)
int CID_val;
//preTag's CID
int pre_val;

//Tag's transion status
boolean tag_transition = false;

void setup() {
  //SERIAL SETUP
  Serial.begin(9600);
  while (!Serial);

  //RFID and SPI SETUP
  //Init SPI bus
  SPI.begin();
  //Init MFRC522
  mfrc522.PCD_Init();

  //PIN SETUP
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  tag_transition = transition_finder(pre_val);
  digitalWrite(LED_PIN, LOW);
  //if Tag is on the Reader
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    //if currentTag's value is changed
    if (tag_transition == true) {
      pre_val = CID_val;

      //Send to PD
      Serial.println(CID_val);
      led_blink();
    }
  }
}


//read tag value and convert to int
void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    UID_BUF[i] = buffer[i];
  }
  CID_val = UID_BUF[1] % 10;
}

//compare preTag's value and currentTag's value
bool transition_finder(int val) {
  if (val !=  CID_val) {
    return true;
  } else {
    return false;
  }
}

//LED blink only one time
void led_blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);
}


