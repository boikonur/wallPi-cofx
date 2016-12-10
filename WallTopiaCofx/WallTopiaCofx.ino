/*
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |         GND/RST2  [ ][ ]            |
         |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |   C5
         |          5V/MISO2 [ ][ ]  A4/SDA[ ] |   C4
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                    SCK/13[ ] |   B5
         | [ ]IOREF                 MISO/12[ ] |   .
         | [ ]RST                   MOSI/11[ ]~|   .
         | [ ]3V3    +---+               10[ ]~|   .
         | [ ]5v    -| A |-               9[ ]~|   .
         | [ ]GND   -| R |-               8[ ] |   B0
         | [ ]GND   -| D |-                    |
         | [ ]Vin   -| U |-               7[ ] |   D7
         |          -| I |-               6[ ]~|   .
         | [ ]A0    -| N |-               5[ ]~|   .
         | [ ]A1    -| O |-               4[ ] |   .
         | [ ]A2     +---+           INT1/3[ ]~|   .
         | [ ]A3                     INT0/2[ ] |   .
         | [ ]A4/SDA  RST SCK MISO     TX>1[ ] |   .
         | [ ]A5/SCL  [ ] [ ] [ ]      RX<0[ ] |   D0
         |            [ ] [ ] [ ]              |
         |  UNO_R3    GND MOSI 5V  ____________/
          \_______________________/                 */


// track001.mp3 <--fileformat!!!!!!
// libraries
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <Bounce2.h>

SdFat sd;
SFEMP3Shield MP3player;

int8_t current_track = 1;
boolean playerEnabled=false;
String inputString = "";

#define FX_NUM 5

String fx[FX_NUM] =
{  "X", "fxtrack1", "fxtrack2", "fxtrack3", "fxtrack4"};

boolean stringComplete = false;

#define LEDPIN 13

void setup() {

  delay(2000);
  Serial.begin(9600);

  pinMode(LEDPIN, OUTPUT);


  if (!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();

  MP3player.setVolume(40, 40);
    //Test Sound
  for (int temp = 5; temp < 40; temp++) {
    MP3player.enableTestSineWave(temp);
    delay(temp);
  }

  MP3player.disableTestSineWave();

  MP3player.setVolume(10, 10);

  Serial.println(F("Init and waiting"));

}

/**
   \note If the means of refilling is not interrupt based then the
   MP3player object is serviced with the availaible function.
*/
//#define USE_MP3_REFILL_MEANS 1

void loop()
{
  // Below is only needed if not interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )
  MP3player.available();
#endif

 // serialEvent();

  if (stringComplete)
  {

    if (inputString == "fxstop") //stop command
    {
      digitalWrite(LEDPIN, LOW);
      stopMusic();
      playerEnabled=false;
      Serial.println("Player Stopped");
    }

    for (int index = 1; index < FX_NUM; index++)
    {

      if (inputString == fx[index])
      {
        digitalWrite(LEDPIN, HIGH);
        stopMusic();
        playerEnabled=true;
        if(current_track!=index){
           current_track=index;
           Serial.println("Player Stopped");
        }

      }


      }

     inputString = "";
      stringComplete = false;
    }

    playLooped();
     // inputString = "";
     // stringComplete = false;
  }


void playLooped(){
  if  (playerEnabled == true){
    if (!MP3player.isPlaying())
    {
      int ret = MP3player.playTrack(current_track);
        Serial.println (ret);
    }

  }

}




void stopMusic() {

  playerEnabled=false;
  if (MP3player.isPlaying())
  {
    MP3player.stopTrack();
  }
}


void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();

    if (inChar == '\n')
    {
      if (inputString.length() > 0)
      {
//        Serial.println("string:");
//        Serial.println(inputString);
        stringComplete = true;
        break;
      }
    }
    else
    {
      inputString += inChar;
    }

  }
}
