//Requires 
//http://arduino.esp8266.com/staging/package_esp8266com_index.json
//VERSION: 2.1.0-rc2
//ArduinoNunchuk
//Metro
//WS2812
//Faraday.Vesc

//Optional defines
//#define ENABLEDEVMODE //Output debugging information
#define ENABLEWEBUPDATE //Enable web updates through http://10.10.100.254/update
#define ENABLEVESC //Control the vesc through serial
//#define ENABLESERVOESC // Enable servo output for traditional motorcontrollers
#define ENABLEWIFI //Enable wifi AP
#define ENABLENUNCHUK //Enable control through a nunchuk
//#define ENABLELED //Enable led control
//#define ENABLEDEADSWITCH //Enable dead man switch 
//#define ENABLEOTAUPDATE //Not working
#define ENABLESMOOTHING //Enable smothing of input values
#define ENABLENONLINEARBRAKE // Non linear braking, softer braking in the beginning
//#define ENABLEDISCBRAKE //Disc brakes

//How many clients should be able to connect to this ESP8266
#define MAX_SRV_CLIENTS 1

//Pins
#define PINEXTERNALRESET 16
#define PINDEADSWITCH 12
#define PINSERVOESC 0
#define PINSERVOBRAKE1 2
#define PINSERVOBRAKE2 14

//Required includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
//#include <WiFiUdp.h>
//#include <ArduinoOTA.h>
#include <IPAddress.h>
#include <Servo.h>
#include <Metro.h>
#include <ws2812_i2s.h>


//Optional includes
#if defined(ENABLEVESC)
#include <FaradayVESC.h>
#include <Ticker.h>
#endif
//#if defined(ENABLENUNCHUK)
#include <Wire.h>
#include <ArduinoNunchuk.h>
//#endif
#if defined(ENABLEWEBUPDATE)
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#endif
//There is an issue when putting "if defined()" sections below includes

const char *faradayVersion = "20160227";

//Wifi
const char *ssid = "FARADAY200";
const char *password = "faraday200";
const int port = 8899;
WiFiServer server(port);
WiFiClient serverClients[MAX_SRV_CLIENTS];

//Metro timers
Metro metroControllerRead = Metro(50);
Metro metro250ms = Metro(250);
Metro metroLeds = Metro(200);
Metro metroLedsReadyState = Metro(50);
Metro metroControllerCommunication = Metro(1000);

#if defined(ENABLESERVOESC)
int servoMaxPWM = 2000;
int servoMinPWM = 1000;
int servoNeutralPWM = 1500;
Servo servoESC;
#endif

#if defined(ENABLELED)
#define LEDSTOTAL 14
#define LEDSCONTROLCOUNT   12
#define LEDSFRONTCOUNT   1
#define LEDSBACKCOUNT   1
#define LEDSCONTROLINDEX   0
#define LEDSFRONTINDEX   12
#define LEDSBACKINDEX   13

static WS2812 leds;
static Pixel_t pixels[LEDSTOTAL];
#endif

#if defined(ENABLEWEBUPDATE)
bool allowWebUpdate = true;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
#endif

#if  defined(ENABLEVESC)
Ticker vescTicker;
FaradayVESC vesc = FaradayVESC();
#endif

long lastloop = millis();
long lastinputduration = millis();
long maxinputduration = 0;
long mininputduration = 1000;

//Power
byte defaultInputNeutral = 50;
byte defaultInputMinBrake = 48;
byte defaultInputMaxBrake = 0;
byte defaultInputMinAcceleration = 52;
byte defaultInputMaxAcceleration = 100;
float defaultMaxStepUP = 2;
float defaultSmoothAlpha = 0.5;


//Current control
float defaultCurrentNeutral = 0;
float defaultCurrentAccelerationMax = 60;
float defaultCurrentAccelerationMin = 0.25;
float defaultCurrentBrakeMax = 60;
float defaultCurrentBrakeMin = 0;

//Control
volatile bool controlDead = false;
bool controlEnabled = false;
float controlPower = defaultInputNeutral;
int controlTarget = defaultInputNeutral;
bool controlCruiseControl = false;
byte controlType = 0;

//Motor
byte motorDirection = 0; //0 = Neutral, 1= Acc, 2= Brake, 3 = Reverse
byte motorTargetPercent = 0;
byte motorPercent = 0;

#if defined(ENABLENUNCHUK)
ArduinoNunchuk nunchuk = ArduinoNunchuk();
#endif

#if  defined(ENABLEVESC)
void vescSend(unsigned char *data, unsigned int len)
{
  Serial.write(data, len);
}

void triggerUpdate(int i)
{
  vesc.update();
}

void vescProcess(unsigned char *data, unsigned int len)
{
  Serial.println(*data);
}

void setupVESC()
{
  vesc.init(vescSend, vescProcess);
  //Call this method every millisecond
  vescTicker.attach_ms(1, triggerUpdate, 0);
}
#endif

#if defined(ENABLESERVOESC)
void setupSERVO()
{
  pinMode(PINSERVOESC, OUTPUT);
  servoESC.writeMicroseconds(servoNeutralPWM);
  servoESC.attach(PINSERVOESC, servoMinPWM, servoMaxPWM);
  servoESC.writeMicroseconds(servoNeutralPWM);
}
#endif

void setup()
{
  Serial.begin(115200);
  //Lets change the pins for serial as we need the i2s pin
#if defined(ENABLELED) && defined(ENABLESERVOESC)
  //Use other pins for rx/tx when leds are enabled
  Serial.swap();
#endif
  pinMode(PINEXTERNALRESET, OUTPUT);
  digitalWrite(PINEXTERNALRESET, LOW);
  delay(500);

  Serial.print("Faraday Motion FirmwareVersion:");
  Serial.println(faradayVersion);

#if defined(ENABLEWIFI)
  setupWIFI();
#endif

#if defined(ENABLEOTAUPDATE)
  setupOTA();
#endif

#if defined(ENABLEWEBUPDATE)
  setupWebUpdate();
#endif

#if  defined(ENABLEVESC)
  setupVESC();
#endif

#if defined(ENABLESERVOESC)
  setupSERVO();
#endif

#if defined(ENABLEDISCBRAKE)
  setupDiscBrake();
#endif

#if defined(ENABLENUNCHUK)
  nunchuk.init();
#endif

#if defined(ENABLELED)
  setupLeds();
#endif

#if defined(ENABLEDEADSWITCH)
  pinMode(PINDEADSWITCH, INPUT_PULLUP);
  attachInterrupt(PINDEADSWITCH, setDeadSwitch, RISING);
#endif

  delay(500);
  digitalWrite(PINEXTERNALRESET, HIGH);
}

void loop()
{
#if defined(ENABLEOTAUPDATE)
  ArduinoOTA.handle();
#endif
#if defined(ENABLEWEBUPDATE)
  if (allowWebUpdate)
    httpServer.handleClient();
#endif
#if defined(ENABLEWIFI)
  if (metro250ms.check() == 1) {
    hasClients();
    //Reset the max time
    maxinputduration  = 0;
    mininputduration  = 1000;
    yield();
  }
#endif

  yield();
  if (metroControllerRead.check() == 1) {    
#if defined(ENABLEWIFI)
    if (controlType == 0 || controlType == 1)
      readFromWifiClient();
#endif
#if defined(ENABLENUNCHUK)
    if (controlType == 0 || controlType == 2)
      readFromNunchukClient();
#endif
    yield();

#if defined(ENABLEDEADSWITCH)
    if (digitalRead(PINDEADSWITCH) == HIGH)
      setDeadSwitch();
    else
      controlDead = false;
#endif

    yield();
    if (metroControllerCommunication.check() == 1)
    {
      //It passed too long time since last communication with a controller
      setDefaultPower();
      yield();
    }

    //Convert and set the current power
    convertPower();
    yield();

    /*
    //check UART for data
    if(Serial.available()){
      size_t len = Serial.available();
      uint8_t sbuf[len];
      Serial.readBytes(sbuf, len);
      //push UART data to all connected telnet clients
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        if (serverClients[i] && serverClients[i].connected()){
          serverClients[i].write(sbuf, len);
          delay(1);
        }
      }
    }
    */
  }

#if defined(ENABLELED)
  if (metroLeds.check() == 1)
  {
    setLedControls();
    yield();
  }
  if (metroLedsReadyState.check() == 1)
  {
    showLedState();
    yield();
    leds.show(pixels);
  }
#endif
}
