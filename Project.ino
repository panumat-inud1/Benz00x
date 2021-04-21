// PlantControl@gmail.com
// plant1234

#include <ESP8266WiFi.h>
#include <DNSServer.h>            
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  

#include <Ticker.h>  

#include <BlynkSimpleEsp8266.h>

#include <LiquidCrystal_I2C.h>
#include <i2c_keypad.h>
//#include <Wire.h>
#include <EEPROM.h>  // คำสั่ง save EEPROM.commit();
//#include <Adafruit_ADS1015.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <SPI.h>  
#include <RTClib.h>

#include "Max44009.h"

BlynkTimer timer;
#define BLYNK_PRINT Serial

RTC_DS3231 RTC;

LiquidCrystal_I2C lcd(0x27, 20, 4);

I2CKEYPAD key(21, 22); // if you use ESP8266 or ESP32 you can change SDA and SCL pin

//Adafruit_ADS1115 ads;
//int16_t adc0, adc1, adc2, adc3;   

#define ONE_WIRE_BUS 14 // D5 กำหนดขาที่จะเชื่อมต่อ Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

Max44009 myLux(0x4A);

Ticker pumper;

#define Pump 13 //D7
#define Fan 12  //D6
#define Led 15  //D8
#define Buzzer;
#define PinConfig 3 // RX resetWifi
#define WiFiName "Caladium_bicolor"

char auth[] = "flksihu6Nab_3zHm1PK3-0LCCOtS6pUS";
char ssid[20] = "";
char password[15] = "";

float Temperature;
int Moisture,Lux,PH = 7;
int  TMin,TMax,MMax,MMin,SLux1,SLux2,SLux11,SLux22;
int  Min,Max;
int Hour,Minute,Seccond;
//int Year,Mount,Day;
int HH1,MM1,SS1,HH11,MM11,HH2,MM2,SS2,HH22,MM22;

unsigned char Step = 5;
unsigned char Timer1 = 0;
unsigned char S1,S2;  // Pump = S1 and Fan S2

boolean keypress = true; 
byte count = 0;

int Sumk;

//-----Pump---------------------------
int setpump = 0;
int ledState = HIGH;
unsigned long previousMillis = 0;
const long interval = 10000;
//-----------------------------------

int checkAuto1 = 0;
int auto1 = 0;
int pus1 = 0;

int checkAuto2 = 0;
int auto2 = 0;
int pus2 = 0;

int checkAuto3 = 0;
int auto3 = 0;
int pus3 = 0;

int checkAuto4 = 0;
int auto4 = 0;
int pus4 = 0;

//=====================================================
void sendSensor()
{ Step++;
  if(Step >= 4)
  { 
  Blynk.virtualWrite(V0, Lux);
  Blynk.virtualWrite(V1, Temperature);
  Blynk.virtualWrite(V2, Moisture);
  Blynk.virtualWrite(V3, PH);
  Blynk.virtualWrite(V4, String(MMin)+'-'+String(MMax));
  Blynk.virtualWrite(V5, String(TMin)+'-'+String(TMax));
  
  if(S1 == 0){Blynk.virtualWrite(V6,0);}else{Blynk.virtualWrite(V6,255);} //PUMP
  if(S2 == 0){Blynk.virtualWrite(V7,0);}else{Blynk.virtualWrite(V7,255);}
  }
}
//=====================================================  
BLYNK_WRITE(V29) 
{ int value = param.asInt();
  Serial.println(value,DEC);
  Timer1 = value; 
}
//=====================================================
BLYNK_WRITE(V8) 
{ int value = param.asInt();
  Serial.println(value,DEC);
  TMin = value;
  EEPROM.write(0,TMin);
  EEPROM.commit(); 
}
//=====================================================
BLYNK_WRITE(V9) 
{ int value = param.asInt();
  Serial.println(value,DEC);
  TMax = value;
  EEPROM.write(1,MMax);
  EEPROM.commit();
}
//=====================================================
BLYNK_WRITE(V10) 
{ int value = param.asInt();
  Serial.println(value,DEC);
  MMin = value; 
  EEPROM.write(2,MMin);
  EEPROM.commit(); 
}
//=====================================================
BLYNK_WRITE(V11) 
{  
  int value = param.asInt();
  Serial.println(value,DEC);
  MMax = value; 
  EEPROM.write(3,MMax);
  EEPROM.commit();
}
//=====================================================

BLYNK_WRITE(V12) 
{  
  int value = param.asInt();
  Serial.println(value,DEC);
  SLux11 = value; 
  EEPROM.write(12,SLux11);
  EEPROM.commit(); 
 
  /* pus3 = param.asInt();
   SLux1 = pus3; 
   EEPROM.write(12,SLux11);
   EEPROM.commit(); */
   
}
//=====================================================
BLYNK_WRITE(V13) 
{  
  int value = param.asInt();
  Serial.println(value,DEC);
  SLux22 = value; 
  EEPROM.write(13,SLux22);
  EEPROM.commit();
}
//=====================================================
BLYNK_WRITE(V14) 
{  
   auto1 = param.asInt();
}
//=====================================================
BLYNK_WRITE(V15) 
{  
   pus1 = param.asInt();
}
//=====================================================

//=====================================================
BLYNK_WRITE(V16) 
{  
   auto2 = param.asInt();
}
//=====================================================
BLYNK_WRITE(V17) 
{  
   pus2 = param.asInt();
}
//=====================================================
//=====================================================
BLYNK_WRITE(V18) 
{  
   auto3 = param.asInt();
}
//=====================================================
BLYNK_WRITE(V19) 
{  
   pus3 = param.asInt();
   /*
   pus3 = param.asInt();
   SLux1 = pus3; 
   EEPROM.write(5,SLux1);
   EEPROM.commit(); */
}
//=====================================================
BLYNK_WRITE(V20) 
{  
   auto4 = param.asInt();
}
//=====================================================
BLYNK_WRITE(V21) 
{  
   pus4 = param.asInt();
   /*
   pus4 = param.asInt();
   SLux2 = pus4; 
   EEPROM.write(6,SLux2);
   EEPROM.commit(); */
}
//=====================================================


void setup(){
 
  Serial.begin(9600);
  pinMode(PinConfig,INPUT_PULLUP);
  pinMode(Pump,OUTPUT);
  pinMode(Fan,OUTPUT);
  pinMode(Led,OUTPUT);
  
  digitalWrite(Fan,HIGH);
  digitalWrite(Pump,HIGH);
  
   lcd.begin();
   sensors.begin();

   pumper.attach(48, pumpersetup); // ทุก 30 นาที จะเช็คว่า temp น้อยกว่าค่า min ไหม ถ้าน้อยกว่าให้ setpump = 1
  // ads.begin(); 
   Wire.begin(); // Start the I2C
   RTC.begin();  // Init RTC
  // Wire.setClock(100000);
  
   
   lcd.setCursor(0,0);
   lcd.print("       Welcome     ");
   
   WiFiManager wifiManager;
    keypress = digitalRead(PinConfig);
    if(keypress == LOW){
    //WiFiManager wifiManager;
    //digitalWrite(LEDFeedback,LOW);
    Serial.print("Please Connect...!");
    lcd.setCursor(0,1);
    lcd.print("  Conneting Wifi  ");
    wifiManager.resetSettings();
    wifiManager.autoConnect(WiFiName);
    //ESP.reset();

    WiFi.begin();
  while (WiFi.status() != WL_CONNECTED){
    //digitalWrite(LEDFeedback,HIGH);
    delay(500);
    Serial.print(".");
    //digitalWrite(LEDFeedback,LOW);
    count++;
    if(count == 60){
      count = 0;
      ESP.reset();
    }
     }
     
    WiFi.setAutoReconnect(true);
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.print("connected: ");

    //----LCD------
    delay(500);
    lcd.setCursor(0,2);
    lcd.print("    Connet Wifi    "); 
  }//------------EndWofimanager-------------
 
   Blynk.config(auth);
   timer.setInterval(2000L, sendSensor);
   Blynk.syncVirtual(V4);
  
  //if (rtc.lostPower()) { rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); }

  EEPROM.begin(512);
  TMin = EEPROM.read(0);
  TMax = EEPROM.read(1);
  MMin = EEPROM.read(2);
  MMax = EEPROM.read(3);
  Lux = EEPROM.read(4);
  SLux1 = EEPROM.read(5);
  SLux2 = EEPROM.read(6);
  HH1 = EEPROM.read(7); MM1 = EEPROM.read(8); HH11 = EEPROM.read(9); MM11 = EEPROM.read(10);
  HH2 = EEPROM.read(11); MM2 = EEPROM.read(12); HH22 = EEPROM.read(13); MM22 = EEPROM.read(14);
  SLux11 = EEPROM.read(12);
  SLux22 = EEPROM.read(13);
 
}

unsigned long last_time = 0;  // Time 
unsigned long last_time1 = 0; // Read Sensor
unsigned long last_time2 = 0; //Blynk
unsigned long last_time3 = 0; //Time_Pump
unsigned long last_time4 = 0; //Loop Pump


void loop(){
  
   if( millis() - last_time > 1000){ last_time = millis(); Time(); }
    
    key.scand();
    key.on(PRESS, [](char key) { Serial.print("PRESS: ");   
         if( key == '*' ) { setdis(">", 0,0); keyboard(); }
    else if( key == 'D' ) { lcd.clear();   Set_time(); }  });
    
  /*  else if(key == 'C'){ EEPROM.write(0,35); 
                         EEPROM.write(1,40);
                         EEPROM.write(2,70);
                         EEPROM.write(3,99);
                         TMin = EEPROM.read(0);
                         TMax = EEPROM.read(1);
                         MMin = EEPROM.read(2);
                         MMax = EEPROM.read(3);
                         Lux = EEPROM.read(4);
                         EEPROM.commit();
                         } */
       if( millis() - last_time2 > 2000) {
      last_time2 = millis(); 
      Blynk.run();
      timer.run(); }
      
       if( millis() - last_time1 > 5000) {
       last_time1 = millis();  
       ReadTemperature();  
       ReadMoisture();
       ReadLux();                
       showMenu();
       }

       
       //-------check_Auto---------
        if (auto1 == 1){
          checkAuto1 = 1 ;
        }else{ 
          checkAuto1 = 0 ; 
        }
        /*if (auto2 == 1){
          checkAuto2 = 1 ;
        }else{ 
          checkAuto2 = 0 ; 
        }
        if (auto3 == 1){
          checkAuto3 = 1 ;
        }else{ 
          checkAuto3 = 0 ; 
        }
        if (auto4 == 1){
          checkAuto4 = 1 ;
        }else{ 
          checkAuto4 = 0 ; 
        } */
        
      //----Pump----   
      if (checkAuto1 == 1 ){
        if(pus1 == 1){ S1 = 1;
            digitalWrite(Pump,LOW); 
        }else{ S1 = 0;
           digitalWrite(Pump,HIGH); 
          }
      }
  
      //----Fan----
       if (checkAuto1 == 1 ){
        if(pus2 == 1){ S2 = 1;
            digitalWrite(Fan,LOW); 
        }else{ S2 = 0;
           digitalWrite(Fan,HIGH); 
          }
      }
      //----Lux1----
       if (checkAuto1 == 1 ){ 
       
       // if(pus3 == 1){ 
            
            //analogWrite(15,SLux1);
               EEPROM.write(12,SLux11); analogWrite(15,SLux11);  setdis((String)SLux11,16,2);
      //  } 
      }
      
      //----Lux2---- 
      /*
        if (checkAuto1 == 1 ){
          if(pus4 == 1){
            analogWrite(15,SLux2);
          }else{
            analogWrite(15,SLux2);
           }
      }  */
      //----End----
    
       
      //------Auto----

      /*if(Moisture < MMin){ S1 = 1;  digitalWrite(Pump,LOW); 
      if( millis() - last_time3 > 10000){ last_time3 = millis();  digitalWrite(Pump,HIGH); S1 = 0;  }
      
      } else{ S1 = 0; digitalWrite(Pump,HIGH);}
      if(Moisture >= MMax){ S1 = 0; digitalWrite(Pump,HIGH); } */
      if (checkAuto1 == 0 ){
      if(setpump == 1){  pump1(); }else{digitalWrite(Pump,HIGH); S1 = 0;}
 
      if(Temperature > TMax){ S2 = 1; digitalWrite(Fan,LOW); }else{ S2 = 0; digitalWrite(Fan,HIGH);}
      if(Temperature <= TMin){ S2 = 0; digitalWrite(Fan,HIGH);}
  
   
      if(HH1 == Hour && MM1 == Minute || HH1 < HH11){ EEPROM.read(5); analogWrite(15,SLux1);setdis( (String)SLux1,16,2); 
      }else if(HH11 == Hour && MM11 == Minute){ SLux1 = 0; EEPROM.write(5,SLux1); EEPROM.commit(); analogWrite(15,SLux1); }

      if(HH2 == Hour && MM2 == Minute || HH2 < HH22){ EEPROM.read(6); analogWrite(15,SLux2);setdis((String)SLux2,16,2);
      }else if(HH22 == Hour && MM22 == Minute){ SLux2 = 0; EEPROM.write(6,SLux2); EEPROM.commit(); analogWrite(15,SLux2); }

      }
      
      //----END_Auto-------


}//-----------------------------------End_Loop-------------------------------------




//---------------------------------Read_SenSor-----------------------------------
void ReadTemperature(){
  
  sensors.requestTemperatures(); //อ่านข้อมูลจาก library
  Temperature = sensors.getTempCByIndex(0);

 
  }
  
void ReadMoisture(){
    const int AirValue = 855;   //you need to replace this value with Value_1
    const int WaterValue = 450;  //you need to replace this value with Value_2
    int soilMoistureValue = 0;
    
    Moisture = 0;
    
    soilMoistureValue = analogRead(A0);
    Moisture = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    if(Moisture < 0){ Moisture = 0;}
  }
  
void ReadLux(){
    Lux = myLux.getLux();
    if(Lux <= 0){
      Lux = 0;

      }
  
  }

//-------------------------------------------------------------------------------

  
void showMenu(){ 
  
  setdis(" Temp :      [  -  ]", 0, 0);
  setdis(" Mois :      [  -  ]", 0, 1);
  setdis(" Lux  :        [   ]", 0, 2);
  setdis("P:    F:            ", 0, 3);

  if(Temperature > 0 && Temperature < 90 ){
  setdis((String)Temperature + "C", 7,0);
  setdis((String)Moisture, 8,1);
  setdis((String)Lux, 8,2); 
  }

  if(TMin > 0 && TMin < 50 && MMin > 0 && MMin < 99){
  setdis((String)TMin, 14,0);
  setdis((String)TMax, 17,0);
  setdis((String)MMin, 14,1);
  setdis((String)MMax, 17,1);
  }
  
  if(S1 == 1){
           setdis("ON",2, 3);
    }else{ setdis("OFF",2, 3); }
    
  if(S2 == 1){
           setdis("ON",8, 3);
    }else{ setdis("OFF",8, 3); }
     
}


void keyboard(){
    int y = 0;
    bool flg = true; //false

    while (flg) {      
    static char keyget = ' ';
    key.scand();
    key.on(PRESS, [](char key) {  Serial.print("PRESS: ");
      keyget = key;
      Serial.println(keyget);
    });
      
     if (keyget == 'B') {  //dow keypad
      y++;
      if(y > 2) y = 2;
      setdis(" ", 0,y-1);
      setdis(">", 0,y);
      Serial.print(y);
      keyget = ' ';
   } else if(keyget == 'A'){  //up keypad
      y--;
      if (y < 0) y = 0;
      setdis(">", 0,y);
      setdis(" ", 0,y+1);   
      Serial.print(y);
      keyget = ' '; 
   }else if (keyget == '#') { 
       if (y == 0) {
        //Temp
        
        flg = false;
        lcd.clear();
        keyget = ' ';
        return Set_Temp(); 
        
      } else if (y == 1) {
        //Mosi
        
        flg = false;
        lcd.clear();
        keyget = ' ';
        return Set_Mois();
        
      } else if (y == 2) {
        //Lux
        flg = false;
        lcd.clear();
        keyget = ' ';
        return Set_Lux();
      } else if (keyget == '*'){
      
        keyget = ' ';
        flg = false;
        lcd.clear();
      } 
       // return showMenu(Time);
     }    
    } //--------end loop whil--------
   }

 

//-----------------------------------SET_SENSOR_ALL--------------------------------- 
 void Set_Temp(){
     bool flg = true;
     int y = 0,x =0;  
     setdis("   Set-Temperature  " , 0, 0);
     setdis(" Tmin =   ", 0, 1);
     setdis(" TMax =   ", 0, 2);
     setdis(" Exit      ", 0, 3);

     setdis((String)TMin,8,1);
     setdis((String)TMax,8,2);
    while (flg) { 
      
    static char keyget = ' ';
    key.scand();
    key.on(PRESS, [](char key) {  Serial.print("PRESS: ");
      keyget = key;
 
      Serial.println(keyget);
    }); 
       // setdis((String)y ,8,3);
    if (keyget == 'B') {  //dow keypad
      y++;
      if(y > 3) y = 3;
      setdis(" ", 0,y-1);
      setdis(">", 0,y);
      Serial.print(y);
      keyget = ' ';
   } else if(keyget == 'A'){  //up keypad
      y--;
      if (y < 1) y = 1;
      setdis(">", 0,y);
      setdis(" ", 0,y+1);   
      Serial.print(y);
      keyget = ' '; 
  } else if (keyget == '#') {
       if (y == 1) {
        setdis(" ", 0,1);
        TMin = SetParam(2,8,1);

        EEPROM.write(0,TMin);
        EEPROM.commit();
        setdis(">", 0,1);
        // lcd.clear();
        //flg = false;
        keyget = ' ';
        
        } else if (y == 2) {
        //TMax
        setdis(" ", 0,2);
        TMax = SetParam(2,8,2);
        EEPROM.write(1,TMax);
        EEPROM.commit();
        //lcd.clear();
        //flg = false;
        setdis(">", 0,2);
        keyget = ' ';
   
      } else if (y == 3 ){
        //Exit
        keyget = ' ';
        flg = false;
        lcd.clear();
      } 
      
    }  
  } //----- END loop while
 }



 void Set_Mois(){
     bool flg = true;
     int y = 0;  
     setdis("   Set-Moisture   " , 0, 0);
     setdis(" Mmin =   ", 0, 1);
     setdis(" MMax =   ", 0, 2);
     setdis(" Exit      ", 0, 3);

     setdis((String)MMin,8,1); 
     setdis((String)MMax,8,2);
   
    while (flg) { 
      
    static char keyget = ' ';
    key.scand();
    key.on(PRESS, [](char key) {  Serial.print("PRESS: ");
      keyget = key;
 
      Serial.println(keyget);
    }); 
       // setdis((String)y ,8,3);
    if (keyget == 'B') {  //dow keypad
      y++;
      if(y > 3) y = 3;
      setdis(" ", 0,y-1);
      setdis(">", 0,y);
      Serial.print(y);
      keyget = ' ';
   } else if(keyget == 'A'){  //up keypad
      y--;
      if (y < 1) y = 1;
      setdis(">", 0,y);
      setdis(" ", 0,y+1);   
      Serial.print(y);
      keyget = ' '; 
  } else if (keyget == '#') {
       if (y == 1) {
        setdis(" ", 0,1);
        
        MMin = SetParam(2,8,1);
        EEPROM.write(2,MMin);
        EEPROM.commit();
        //lcd.clear();
        //flg = false;
        setdis(">", 0,1);
        keyget = ' ';
        
        } else if (y == 2) {
       
        setdis(" ", 0,2);
        
        MMax = SetParam(2,8,2);
        
        EEPROM.write(3,MMax);
        EEPROM.commit();
        //lcd.clear();
        //flg = false;
        setdis(">", 0,2);
        keyget = ' ';
   
      } else if (y == 3 ){
        //Exit
        keyget = ' ';
        flg = false;
        lcd.clear();
      } 
      
    }  
  } //----- END loop while
 }



 void Set_Lux(){
  
  int Year, Mount, Day;
  bool flg = true;
  int y = 0;  
  SLux1 = EEPROM.read(5);
  SLux2 = EEPROM.read(6);
      
     setdis("  ON     OFF   Lux   ", 0, 0);
     setdis((String)HH1,1,1);  setdis(":",3,1);  setdis((String)MM1,4,1); setdis(" ",6,1); setdis((String)HH11,7,1); setdis(":",9,1); setdis((String)MM11,10,1);
     setdis((String)SLux1,15,1);
     setdis((String)HH2,1,2);  setdis(":",3,2);  setdis((String)MM2,4,2); setdis(" ",6,2); setdis((String)HH22,7,2); setdis(":",9,2); setdis((String)MM22,10,2);
     setdis((String)SLux1,15,1);
     
     
//     setdis((String)HH1 +":"+ MM1 + "-" + HH11 +":"+ MM11 +"  " + SLux1,1, 1);
//     setdis((String)HH2 +":"+ MM2 + "-" + HH22 +":"+ MM22 +"  " + SLux2,1, 2);
     setdis(" Exit  ", 0, 3);
     
  while (flg) { 
      
    static char keyget = ' ';
    key.scand();
    key.on(PRESS, [](char key) {  Serial.print("PRESS: ");
      keyget = key;
 
      Serial.println(keyget);
    }); 
       // setdis((String)y ,10,3);
    if (keyget == 'B') {  //dow keypad
      y++;
      if(y > 3) y = 3;
      setdis(" ", 0,y-1);
      setdis(">", 0,y);
      Serial.print(y);
      keyget = ' ';
   } else if(keyget == 'A'){  //up keypad
      y--;
      if (y < 1) y = 1;
      setdis(">", 0,y);
      setdis(" ", 0,y+1);   
      Serial.print(y);
      keyget = ' '; 
  } else if (keyget == '#') {
       if (y == 1) {
        //Time1 NO
        setdis(" ", 0,1);
        setdis("                   ",1,1);
        HH1 = SetParam(2,1,1);
        MM1 = SetParam(2,4,1);
       // setdis(":",5,1);
        //Time1 OFF
        HH11 = SetParam(2,8,1);
        MM11 = SetParam(2,11,1);

        SLux1 = SetParam(3,15,1);
        
        EEPROM.write(7,HH1);
        EEPROM.write(8,MM1);
        EEPROM.write(9,HH11);
        EEPROM.write(10,MM11);
        
        EEPROM.write(5,SLux1);
        EEPROM.commit();
        setdis(">", 0,1);
        //lcd.clear();
        //flg = false;
      
        keyget = ' ';
        
        } else if (y == 2) {
        //Time2 ON
        setdis(" ", 0,2);
        setdis("                   ",1,2);
        HH2 = SetParam(2,1,2);
        MM2 = SetParam(2,4,2);
        //setdis(":",5,1);
        
        //Time2 OFF
        HH22 = SetParam(2,8,2);
        MM22 = SetParam(2,11,2);

        SLux2 = SetParam(3,15,2);
        
        EEPROM.write(11,HH2);
        EEPROM.write(12,MM2);
        EEPROM.write(13,HH22);
        EEPROM.write(14,MM22);
        EEPROM.write(6,SLux2);
        EEPROM.commit();
        
        setdis(">", 0,2);
        //lcd.clear();
        //flg = false;
        
        keyget = ' ';
   
      } else if (y == 3 ){
        //Exit
        keyget = ' ';
        flg = false;
        lcd.clear();
      } 
      
    }  
   } //----- END loop while
  }
//------------------------------------------------------------------------------------



//---------------------------------------TIME-----------------------------------------
void Time(){
  
    DateTime now = RTC.now();
   
    Hour = now.hour();
    Minute = now.minute() ;
    Seccond = now.second() ;

  
    
//   Year = now.year(); 
//   Mount = now.month(); 
//   Day = now.day();
    setdis((String)Hour + ":" + Minute + ":" + Seccond, 12,3);
   
  }


  
void Set_time(){
  int Year,Mount,Day;
  int HH, MM, SS;
      
    bool flg = true;
     int y = 0;  
     setdis("   Set-Date/Time    ", 0, 0);
    //setdis(" Date :    /  /     ", 0, 1);
    // setdis(" Time :             ", 0, 2);
     setdis(" Exit               ", 0, 2);

   // char date[10] = "YY/MM/DD";
    //RTC.now().toString(date);
    //DateTime now = RTC.now();
    DateTime now = RTC.now();
    Year = now.year(),Mount = now.month(),Day = now.day();
    
    setdis((String)Year +"/"+Mount+"/"+Day,1,3);
    //setdis(date,8,1 );
    setdis((String)Hour +":"+ Minute +":"+Seccond,12,3);

  while (flg) { 
      
    static char keyget = ' ';
    key.scand();
    key.on(PRESS, [](char key) {  Serial.print("PRESS: ");
      keyget = key;
 
      Serial.println(keyget);
    }); 
       //setdis((String)y ,8,3);
    if (keyget == 'B') {  //dow keypad
      y++;
      if(y > 2) y = 2;
      setdis(" ", 0,y-1);
      setdis(">", 0,y);
      Serial.print(y);
      keyget = ' ';
   } else if(keyget == 'A'){  //up keypad
      y--;
      if (y < 1) y = 1;
      setdis(">", 0,y);
      setdis(" ", 0,y+1);   
      Serial.print(y);
      keyget = ' '; 
  } else if (keyget == '#') {
       if (y == 1) {
        //Date
        setdis(" ", 0,1);
        Year = SetParam(4,1,1);
        setdis("/",5,1);
        Mount = SetParam(2,6,1);
        setdis("/",8,1);
        Day = SetParam(2,9,1);

        HH = SetParam(2,12,1);
        setdis(":",14,1);
        MM = SetParam(2,15,1);
        setdis(":",17,1);
        SS = SetParam(2,18,1);
        
        RTC.adjust(DateTime( Year, Mount, Day, HH, MM, SS));
        setdis(">", 0,1);
        //lcd.clear();
        //flg = false;
        keyget = ' ';
        
        } /* else if (y == 2) {
        //Time
        setdis(" ", 0,2);
        
        HH = SetParam(2,8,2);
        setdis(":",10,2);
        MM = SetParam(2,11,2);
        setdis(":",13,2);
        SS = SetParam(2,14,2);
        RTC.adjust(DateTime( HH, MM, SS));
        //lcd.clear();
        //flg = false;
        setdis(">", 0,2);
        keyget = ' ';
   
      }*/ else if (y == 2 ){
        //Exit
        keyget = ' ';
        flg = false;
        lcd.clear();
      } 
      
    }  
   } //----- END loop while
  }
  
//--------------------------------------------------------------------------------------------



void setdis(String msg,int x, int y){
    
  lcd.setCursor(x,y); lcd.print(msg);
  
  }


  
int SetParam(byte Num, char X, char Y) {
  char xx, yy, xx_bk;
  xx = X;
  xx_bk = xx; //use for key *
  yy = Y  ;  //height cur pos  (top left
  byte iNum = 0;
  int Sum = 0;
  String str;
  bool flg_float = false;

  lcd.blink();
  lcd.setCursor(xx, yy); //lcd.print(' ');
  
  while (1) {
    delay(50);
    static char keyget = ' ';
    key.scand();
    key.on(PRESS, [](char key) { // void on(KEYPAD_EVENT event, KeypadEventCallback callback) ;
      Serial.print("PRESS: ");
      keyget = key;
      Serial.println(keyget);
    });


    if ( (keyget == '#') || iNum >= Num) {
      keyget = ' ';
      lcd.noCursor();
      lcd.noBlink();
      Sum = str.toInt();
      return (Sum);
    }

   
    if ((keyget >= '0' && keyget <= '9' || keyget == 'D') && iNum <= Num) {
      
      str += keyget;
      lcd.setCursor(xx, yy); lcd.print(keyget);
      keyget = ' ';
      xx++;
      lcd.setCursor(xx, yy); //lcd.print(' ');
      iNum++;
    }
    
  } //------------ End loop while
}

//--------------------------------Pump----------------------------
void pumpersetup(){
   setpump = 1;
  }

void pump1(){
      ReadMoisture();
     if (Moisture <= MMin){
    
        int checkpump = 1;
      
     if(checkpump == 1){   
         
         unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == HIGH) {
      
      S1 = 1;
      ledState = LOW;  // Note that this switches the LED *off*
      
    } else { 
      
      S1 = 0;
      ledState = HIGH;  // Note that this switches the LED *on*
      setpump = 0;
      checkpump = 0;
      
    }
    digitalWrite(Pump, ledState);
       }       
     } 
   } 
 }

  


/*void  Wifi_Config()
{
    WiFiManager wifiManager;
    Flg_Wifi = digitalRead(PinConfig);
    if(Flg_Wifi == LOW){
    //WiFiManager wifiManager;
    //digitalWrite(LEDFeedback,LOW);
    Serial.print("Pleass Connect...!");
    wifiManager.resetSettings();
    wifiManager.autoConnect(WiFiName);
    ESP.reset();
  }
  
    WiFi.begin();
  while (WiFi.status() != WL_CONNECTED){
    //digitalWrite(LEDFeedback,HIGH);
    delay(500);
    Serial.print(".");
    //digitalWrite(LEDFeedback,LOW);
    count++;
    if(count == 60){
      count = 0;
      ESP.reset();
    }

    }
    Serial.println();
  Serial.print("connected: ");
  WiFi.setAutoReconnect(true);
  Serial.println(WiFi.localIP());
    
    } */

/*
 void Port(){ 
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  Serial.print("AIN0: "); Serial.println(adc0);
  Serial.print("AIN1: "); Serial.println(adc1);
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.print("AIN3: "); Serial.println(adc3);
  Serial.println(" ");
  
  } */
  
