#include <HomeSpan.h>
#include "AccessoryInformation.h"
#include "Player.h"
#include "Button.h"
#include "QueueMaster.h"
#include "Config.h"

QueueMaster* qm;


void setup() {

  // setup for wifi:
  const char* ssid = WIFI_SSID;
  const char* password = WIFI_PASS;
  const int logLevel = LOG_LEVEL;
  const char* pairingCode = PAIRING_CODE;

  // setup for queueMaster:
  const int writeGpioPins[2] = ANALOGUE_WRITE_GPIO_PINS;
  const int sampleInterval = SAMPLE_INTERVAL;
  pinMode(writeGpioPins[0], OUTPUT);
  pinMode(writeGpioPins[1], OUTPUT);

  // setup for FourScore device
  const bool useButtons = USE_BUTTONS;

  // start serial communication:
  Serial.begin(115200);

  // homeSpan.enableAutoStartAP();
  homeSpan.setLogLevel(logLevel);
  homeSpan.setWifiCredentials(ssid, password);
  homeSpan.setPairingCode(pairingCode);
  
  /*
  * we use a bridge containing all features of the FourScore unit to simplify
  * management in the HomeApp.
  */
  homeSpan.begin(Category::Bridges,"FourScore Key Hanger", "FourScore", "FourScore Key Hanger");

  LOG2("Creating QueueMaster"); LOG2("\n");
  qm = new QueueMaster(writeGpioPins, sampleInterval);

  // NOTE: could use setWifiCallback to wait for connection before creating devices https://github.com/HomeSpan/HomeSpan/blob/master/docs/Reference.md
  new SpanAccessory();  
    new AccessoryInformationNonIdentifiable("FourScore Bridge");
    new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");

  LOG2("Creating players in factory"); LOG2("\n");
  playerFactory(qm);
  if (useButtons) {
    LOG2("Creating buttons in factory"); LOG2("\n");
    buttonFactory(qm);
  }
}

void loop() {
  homeSpan.poll();
  qm->loop();
}
