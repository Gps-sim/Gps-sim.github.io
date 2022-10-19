#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial sgps(5,4);
SoftwareSerial sgsm(7, 8);
TinyGPSPlus gps;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
sgsm.begin(9600);
sgps.begin(9600);
Serial.println("GPS starting");

}

void loop() {
  // put your main code here, to run repeatedly:
sendLocation();
}

void sendLocation(){
  while(sgps.available()){
  gps.encode(sgps.read());
}
if(gps.location.isUpdated()){
  //Serial.begin(9600);
  sgsm.print("\r");
  delay(1000);
  sgsm.print("AT+CMGF=1\r");
  delay(1000);
  sgsm.print("AT+CMGS=\"+2567571089861\"\r");
  delay(1000);
  //sgsm.print("");
  sgsm.print("www.google.com.ph/maps/place/");
  sgsm.println(gps.location.lat(),6);
  sgsm.print(",");
  sgsm.print(gps.location.lng(),6);
  sgsm.print("\r");
  delay(1000);
  sgsm.println((char)26);
  delay(1000);
  Serial.write(0x1A);
  Serial.write(0x0D);
  Serial.write(0x0A);
  delay(1000);
}
}
