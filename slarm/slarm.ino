#include <DS3232RTC.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

const int stepDelay=7500;
const int stepPin = 9;
const int dirPin = 8;
int alarm_time_h;
int alarm_time_m;
int alarm_time_s;
int clock_time_h;
int clock_time_m;
int clock_time_s;
int state_1_s;
int state=0;     //initial value is 0.
int pinLED=12;    //test LED
uint32_t last = 0;
int c;
int count =0;
int bt_array[4]={0,0,0,0};
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial BTserial(3, 4); // RX | TX

// Connect the HC-05 TX to Arduino pin 3 RX. 
// Connect the HC-05 RX to Arduino pin 4 TX through a voltage divider.

void setup()
{
	Serial.begin(9600);
	pinMode(pinLED, OUTPUT);
	lcd.begin();
    lcd.backlight();
	setSyncProvider(RTC.get);
	BTserial.begin(9600); 
	/*if(timeStatus() != timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");
    */
	attachInterrupt(digitalPinToInterrupt(2), interrupt_mortor, CHANGE);
    pinMode(stepPin,OUTPUT); 
    pinMode(dirPin,OUTPUT);
    digitalWrite(dirPin,HIGH);
    mp3_set_serial (Serial);    //set Serial for DFPlayer-mini mp3 module 
    mp3_set_volume (30);	
}

void loop()
{
    uint32_t now = millis();
    if (now - last >= 1000)
    {
        digitalClockDisplay();
        last = now;
    }
    clock_time_h=hour();
    clock_time_m=minute();
	clock_time_s=second();
	if (BTserial.available())
    {  
        c = BTserial.read();
        //Serial.println(c);
        bt_array[count]=c;
        //Serial.println(bt_array[count]);
        count+=1;
        if (count==4)
        {
            count=0;
        }
        alarm_time_h = (bt_array[0]*10)+(bt_array[1]);
        alarm_time_m = (bt_array[2]*10)+(bt_array[3]);
        alarm_time_s = 0;
        Serial.print(alarm_time_h);
        Serial.print(':');
        Serial.print(alarm_time_m);
        Serial.println();
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Set alarm!");
        delay(100);
        lcd.clear();
    }
	if ((alarm_time_h==clock_time_h)&&(alarm_time_m==clock_time_m)&&(clock_time_s==alarm_time_s))
	{
		state=1;
		state_1_s = clock_time_s;
		lcd.clear();
	}

	/*if (state==1)
	{
		if (clock_time_s == (state_1_s+10))
		{
			state=2;
		}
	}*/
	/*if (alarm_time==clock_time)
	{
		state=1;
	}
	*/
	mp3_ring();
	mortor();
	//Serial.println(state);
}

void mortor()
{
	if (state==2)
	{
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
        lcd.setCursor(13,1);
        lcd.print('3');
	}
    
    
}
void mp3_ring()
{
	if (state==1)
	{
		mp3_play (1);
        delay (10000);
        mp3_pause ();
        state=2;
	}
}

void interrupt_mortor()
{
	state=0;
}


/*########################################################################################*/
void digitalClockDisplay()
{
    // digital clock display of the time
    //serial_print();
    if (state==0)
    {
		print_time_on_LCD();
    }
    if (state==1 || state==2)
    {
    	lcd.setCursor(5,0);
    	lcd.print("slarm!");
        lcd.setCursor(5,1);
        lcd.print(state);
    }
    //print_time_on_LCD();
    /*if(second()!=0)
    {
        print_time_on_LCD();
        lcd.setCursor(10,1);
        lcd.print(get_second());
        
    }
    if(second()==0)
    {
        lcd.clear();
        print_time_on_LCD();
        lcd.setCursor(10,1);
        lcd.print(get_second());
    }*/
}

void serial_print(void)
{
    Serial.print(get_date());
    Serial.print(" ");
    Serial.print(get_time());
    Serial.println();
}

void print_time_on_LCD(void)
{
    lcd.setCursor(1, 0);
    lcd.print(get_date());
    lcd.setCursor(4, 1);
    lcd.print(get_time());
    lcd.setCursor(12,0);
    lcd.print(weekday_to_english());
}

String get_date(void)
{
    char date[20];
    sprintf(date, "%d/%02d/%02d", year(), month(), day());
    return (String)date;
}
String get_time(void)
{
    char time[20];
    sprintf(time, "%02d:%02d:%02d", hour(), minute(), second());
    return (String)time;
}
String weekday_to_english(void)
{
    String a;
    if(weekday()==1)
        a="SUN";
    if(weekday()==2)
        a="MON";
    if(weekday()==3)
        a="TUE";
    if(weekday()==4)
        a="WED";
    if(weekday()==5)
        a="THU";
    if(weekday()==6)
        a="FRI";
    if(weekday()==7)
        a="SAT";
    return a;
}