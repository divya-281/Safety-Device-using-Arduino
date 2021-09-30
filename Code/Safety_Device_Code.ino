#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int buzzer = 8, button = 6;
static const int RXgps = 3, TXgps = 4;
static const int gsmtx = 9, gsmrx = 10;
static const uint32_t gps_baudrate = 9600;
String textMessage;
float Lat = 0, Lon = 0;
int Day = 0,Month = 0, Year = 0, Hour = 0, Minute = 0, Second = 0;

SoftwareSerial gsm(gsmtx, gsmrx);
TinyGPSPlus gps;
SoftwareSerial pos(RXgps, TXgps);

void setup() 
{
  Serial.begin(9600);
  gsm.begin(9600);   // Setting the baud rate of GSM Module  
    pos.begin(gps_baudrate);
pinMode(buzzer , OUTPUT);
  pinMode(button , OUTPUT);

}

void loop() 
{
  while(digitalRead(button)== HIGH)
  {
    for (int i = 0; i < 15; i++)
    {
      tone(buzzer, 1000);
      delay(100);
      tone(buzzer, 2000);
      delay(100);
    }
    noTone(buzzer);
    digitalWrite(button,LOW);
    coordinate();
    Message();
    break;
  }
}

void Message()
{
    gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1 second
    gsm.println("AT+CMGS=\"+919495479178\"\r"); // Replace x with mobile number
    delay(1000);
    Display();
    delay(100);
    gsm.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module 
    delay(100);
}

void coordinate()
{
  if (millis() > 8000 && gps.charsProcessed() < 3)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
  while (pos.available() > 0)
  {
    if (gps.encode(pos.read()))
      {
        time_Convert();
        delay(1000);
        break;
      }
  }
  digitalWrite(6,LOW);
  delay(1000);
}

void time_Convert() // GPS FUNCTION
{
  //Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Lat = gps.location.lat();
    Lon = gps.location.lng();
  }
  // Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Day = gps.date.day();
    Month = gps.date.month();    
    Year = gps.date.year();
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  //Serial.print(F(" "));
  if (gps.time.isValid())
  {
    int h = 0;
    if (gps.time.minute() > 29)
      h++;
    if (gps.time.hour() > 17 & gps.time.minute()>29)
    {
      Hour = gps.time.hour()-19+h;
      
    }
    else
    {
      Hour = gps.time.hour()+5+h;
      
    }
    if (gps.time.minute() < 30)
      Minute = gps.time.minute()+30;
    else
    {
      Minute = gps.time.minute()-30;
    }

    
    if (gps.time.second() < 10) 
    Second = gps.time.second();
  }
  else
  {
    Day = 0;
    Month = 0;
    Year = 0;
    Hour = 0;
    Minute = 0;
    Second = 0;
  }
  digitalWrite(button, LOW);
}

void Display()
{
    Serial.println("I need HELP.\nLocation: " + String("Lat: ") +String(Lat) + " "+String("Lon: ") + String(Lon));
    gsm.println("I need HELP.\nLocation: " + String("Lat: ") +String(Lat) + " "+String("Lon: ") + String(Lon));
    gsm.println("Time: " +String(Hour) + ":"+ String(Minute)+":"+ String(Second));
    Serial.println("Time: " +String(Hour) + ":"+ String(Minute)+":"+ String(Second));
    gsm.println("Date: " +String(Day) + "/"+ String(Month)+"/"+ String(Year));
    Serial.println("Date: " +String(Day) + "/"+ String(Month)+"/"+ String(Year));
}
