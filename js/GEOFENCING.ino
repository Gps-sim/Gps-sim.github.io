#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

//--------------------------------------------------------------
//enter your personal phone number to receive sms alerts.
//phone number must start with country code.
const String PHONE = "+256700953178";
//--------------------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial SIM800A(rxPin,txPin);
//--------------------------------------------------------------

SoftwareSerial GPS(9,8); //rx tx
//GPS Module RX pin to Arduino 9
//GPS Module TX pin to Arduino 8
//AltSoftSerial GPS;
TinyGPSPlus gps;
//--------------------------------------------------------------
// #define BUZZER 4

// // Alarm
// int buzzer_timer = 0;
// bool alarm = false;
// boolean send_alert_once = true;

//----------------------------------
// keep track of time, after how much time the messages are sent

// Tracks the time since last event fired
boolean multiple_sms = false;
unsigned long previousMillis=0;
unsigned long int previoussecs = 0; 
unsigned long int currentsecs = 0; 
unsigned long currentMillis = 0; 
 int secs = 0; 
 int pmints = 0; 
 int mints = 0; // current mints
 int interval= 1 ; // updated every 1 second

 
//--------------------------------------------------------------
// Size of the geo fence (in meters)
const float maxDistance = 10;

//--------------------------------------------------------------
// float initialLatitude = 33.094691;
// float initialLongitude = 71.166038;

//for COCIS
// float initialLatitude = 0.331946;
// float initialLongitude = 32.570672;

//For HOME
// float initialLatitude = 0.299905;
// float initialLongitude = 32.607328;

//uh
//double initialLatitude = 0.332519;
//double initialLongitude = 32.57247;


//forvl five
double initialLatitude = 0.328607;
double initialLongitude = 32.570351;





double latitude, longitude;

//--------------------------------------------------------------


void getGps(double& latitude, double& longitude);


/*****************************************************************************************
 * setup() function
 *****************************************************************************************/
void setup()
{
  //--------------------------------------------------------------
  //Serial.println("Arduino serial initialize");
  Serial.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("SIM800a serial initialize");
  SIM800A.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("NEO6M serial initialize");
  GPS.begin(9600);
  //--------------------------------------------------------------
//  pinMode(BUZZER, OUTPUT);
  //--------------------------------------------------------------
  SIM800A.println("AT"); //Check GSM Module
  delay(1000);
  SIM800A.println("ATE1"); //Echo ON
  delay(1000);
  SIM800A.println("AT+CPIN?"); //Check SIM ready
  delay(1000);
  SIM800A.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  SIM800A.println("AT+CNMI=2,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  //AT +CNMI = 2,1,0,0,0 - AT +CNMI = 2,2,0,0,0 (both are same)
  //--------------------------------------------------------------
  delay(60000);
  //buzzer_timer = millis();
}





/*****************************************************************************************
 * loop() function
 *****************************************************************************************/
void loop()
{
  //--------------------------------------------------------------
  getGps(latitude, longitude);
  //--------------------------------------------------------------
  double distance = getDistance(latitude, longitude, initialLatitude, initialLongitude);
  //--------------------------------------------------------------
  Serial.print("Latitude= "); Serial.println(latitude, 6);
  Serial.print("Lngitude= "); Serial.println(longitude, 6);
  Serial.print("initialLatitude= "); Serial.println(initialLatitude, 6);
  Serial.print("initialLngitude= "); Serial.println(initialLongitude, 6);
  Serial.print("current Distance= "); Serial.println(distance);
  //--------------------------------------------------------------
  // Set alarm on?

  /* ---------------------------------------------------------*/
  if(distance > maxDistance) {
    
    multiple_sms = true;
    sendAlert();
    //------------------------------------------
  //   if(send_alert_once == true){
  //   // digitalWrite(BUZZER, HIGH);
  //     sendAlert();
  //     alarm = true;
  //    send_alert_once = false;
  //     buzzer_timer = millis();
      
  //  }
  //   //------------------------------------------
   }
   else{


      //send_alert_once = true;
      multiple_sms = false;
  }
  //--------------------------------------------------------------

  // Handle alarm
  
  // if (alarm == true) {
  //   if (millis() - buzzer_timer > 5000) {
  //     digitalWrite(BUZZER, LOW);
  //     alarm = false;
  //     //buzzer_timer = 0;
      
  //   }
  // }

  if ( multiple_sms = true)
  {
       currentMillis = millis();
       currentsecs = currentMillis / 1000; 
       if ((unsigned long)(currentsecs - previoussecs) >= interval) {
       secs = secs + 1; 

       if (( secs >= 60) && (distance > maxDistance))
       {
        sendAlert();
        multiple_sms = false;
        secs = 0;
       }
    }
  }
  //--------------------------------------------------------------  
  while(SIM800A.available()){
    Serial.println(SIM800A.readString());
  }
  //--------------------------------------------------------------
  while(Serial.available())  {
    SIM800A.println(Serial.readString());
  }
  //--------------------------------------------------------------


}



/*****************************************************************************************
* getDistance() function
*****************************************************************************************/

// Calculate distance between two points using Haversine= sine2(@/2)
double getDistance(double flat1, double flon1, double flat2, double flon2) {

  // Variables
  double dist_calc=0;
  double dist_calc2=0;
  double diflat=0;
  double diflon=0;

  // Calculations
  diflat  = radians(flat2-flat1);
  flat1 = radians(flat1);
  flat2 = radians(flat2);
  diflon = radians((flon2)-(flon1));

  dist_calc = (sin(diflat/2.0)*sin(diflat/2.0));
  dist_calc2 = cos(flat1);
  dist_calc2*=cos(flat2);
  dist_calc2*=sin(diflon/2.0);
  dist_calc2*=sin(diflon/2.0);
  dist_calc +=dist_calc2;

  dist_calc=(2*atan2(sqrt(dist_calc),sqrt(1.0-dist_calc)));
  
  dist_calc*=6371000.0; //Converting to meters

  return dist_calc;
}


/*****************************************************************************************
 * getGps() Function
*****************************************************************************************/
void getGps(double& latitude, double& longitude)
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (GPS.available()){
      if (gps.encode(GPS.read())){
        newData = true;
        break;
      }
    }
  }
  
  if (newData) //If newData is true
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    newData = false;
  }
  else {
    Serial.println("No GPS data is available");
    latitude = 0;
    longitude = 0;
  }
}




/*****************************************************************************************
* sendAlert() function
*****************************************************************************************/
void sendAlert()
{
  //return;
  String sms_data;
  sms_data = "Alert! Child is outside the fence.\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  //sms_data += String(latitude) + "," + String(longitude);

  //return;
  SIM800A.print("AT+CMGF=1\r"); //activate text mode
  delay(1000);
  SIM800A.print("AT+CMGS=\""+PHONE+"\"\r");
  delay(1000);
  SIM800A.print(sms_data);
  SIM800A.print(latitude, 6);
  SIM800A.print(",");
  SIM800A.print(longitude, 6);
  delay(100);
 
  SIM800A.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26 end of message
  delay(1000);
  Serial.println("SMS Sent Successfully.");
  
}