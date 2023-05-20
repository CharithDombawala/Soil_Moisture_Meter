#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER 8//14        // sudu- button 2, kalu button 1,duburu- button 4, rathu-button 3    //  1 - gnd , 2- button 3, 3- button 4, 4- button 1, 5-button 2
#define LED_1 13//19
#define pb_cancel 4//3      
#define pb_ok 5//2 
#define pb_down 2//5 
#define pb_up 3//4 
//#define DHTpin 9 //15
 
//#define miosturesensordigital  //12
//#define moisturesensorPower 7
//#define moisturesensor A0 //23


//DHTesp dhtsensor;
LiquidCrystal_I2C lcd(0x3F,16,2);
//Global variable

byte days = 1;       // Range: 0-255
byte hours = 22;     // Range: 0-255
byte minutes = 43;   // Range: 0-255
byte seconds = 1;

int timeNow=0;
int timeLast=0;
 
int moisture[]={0,0};
byte temperature[]={0,0};
byte humidity[]={0,0};

bool alarm_enabled=true;
byte alarm_hours[]={5};
byte alarm_minutes[]={54};
bool alarm_triggered[]={false};

int BUZZERFrequency = 5000;
byte BUZZERDuration = 750;

byte current_mode=0;
byte n_mode=6;
String modes[]={"Set Time","Set moisture","Set Temparature","Set Humidity","Set Alarm","Bluetooth mode"};

void setup() {
  // put your setup code here, to run once:


  pinMode(BUZZER,OUTPUT);
  pinMode(LED_1,OUTPUT);
  pinMode(pb_cancel,INPUT_PULLUP);
  pinMode(pb_ok, INPUT_PULLUP);
  pinMode(pb_down, INPUT_PULLUP);
  pinMode(pb_up, INPUT_PULLUP);
  //pinMode(DHTpin, INPUT);
  //pinMode(moisturesensorPower, OUTPUT);
	//pinMode(moisturesensor, INPUT);
	// Initially keep the sensor OFF
	//digitalWrite(moisturesensorPower, LOW);
  
  //dhtsensor.setup(DHTpin,DHTesp::DHT22);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  // Print a message on both lines of the LCD.
  lcd.setCursor(5,0);   //Set cursor to character 2 on line 0
  lcd.print("Welcome");
  
 
}

void loop() {
 // put your main code here, to run repeatedly:
   update_time_with_check_alarm();
   
   
if (digitalRead(pb_ok)==LOW){  
     delay(200);
     go_to_menu();
    lcd.clear();   
}
  
     BlinkLED1();
     ring_alarm(); 
  
  

}


void print_text(String text, int column , int row, int text_size){
  lcd.clear();  
  lcd.setCursor(column,row);   //Set cursor to character 2 on line 0
  lcd.print(text);

}

// void print_time_now(){
//   lcd.clear();   
//   print_text(days,0,0,1);
//   print_text(F(":"),20,0,1);
//   print_text(hours,30,0,1);
//   print_text(F(":"),50,0,1);
//   print_text(minutes,60,0,1);
//   print_text(F(":"),80,0,1);
//   print_text(seconds,90,0,1);

// }

void update_time(){
  timeNow=millis()/1000;
  seconds=timeNow - timeLast;
  if (seconds>=60){
       minutes+=1;
       timeLast+=60;
  }       
  if (minutes==60){
       hours+=1;
       minutes=0;
  } 
  if (hours==24){
      days+=1;
      hours=0;
  }   
} 


void ring_alarm(){
  
  lcd.clear();  
  print_text(F("alarm"),0,0,2);
  digitalWrite(LED_1, HIGH);
 
 bool break_happened = false;
 
 while(break_happened == false && digitalRead(pb_cancel) == HIGH){
 
    if(digitalRead(pb_cancel)==LOW){
        delay(200);
        break_happened=true;
        break;
   }
    digitalWrite(BUZZER, HIGH);
    tone(BUZZER, BUZZERFrequency);
    delay(BUZZERDuration);
    noTone(BUZZER);
    delay(BUZZERDuration);

    
 }
  digitalWrite(LED_1, LOW);
  lcd.clear();  

}

void update_time_with_check_alarm(){
  //lcd.clear();     
  update_time();
  //print_time_now();

  if(alarm_enabled){
    
      if(alarm_triggered[0]==false && alarm_hours[0]==hours && alarm_minutes[0]==minutes){
          ring_alarm();
          alarm_triggered[0]=true;
      }

    }
}


int wait_for_button_press(){
  while(true){
    if(digitalRead(pb_ok)==LOW){
      delay(200);
      return pb_ok;
    }
    else if(digitalRead(pb_up)==LOW){
      delay(200);
      return pb_up;
    }
    else if(digitalRead(pb_down)==LOW){
      delay(200);
      return pb_down;
    }
    else if(digitalRead(pb_cancel)==LOW){
      delay(200);
      return pb_cancel;
    }
    update_time();
  }
}


void go_to_menu(){
  while(digitalRead(pb_cancel)==HIGH){
    lcd.clear();  
    print_text((modes[current_mode]),0,1,2);
    lcd.setCursor(12,0);   
    lcd.print("Menu");


    int pressed=wait_for_button_press();

    if(pressed==pb_up){
      delay(200);
      current_mode+=1;
      current_mode=current_mode % n_mode;
    }

    else if(pressed==pb_down){
      delay(200);
      current_mode-=1;
      if(current_mode<0){
        current_mode=n_mode-1;
      }
    }

    else if(pressed==pb_ok){
      delay(200);
      run_mode(current_mode);
    }

    else if(pressed==pb_cancel){
      delay(200);
      break;
    }
    update_time();
  }

}


void run_mode(int current_mode){
    if(current_mode==0){
      set_time();
    }
    else if(current_mode==1){
      set_moisture();        
    }
    else if(current_mode==2){
      set_temp();       
    }
    else if(current_mode==3){
      set_humidity();        
    }
    else if(current_mode==4){
      set_alarm();      
    }
    else if(current_mode==5){
       print_text(F("BluetoothMode"),40,40,1);
    }
}
void set_time(){
  int temp_hour=hours;
  while(true){
    lcd.clear();  
    print_text(("Hour :"+String(temp_hour)),0,0,2);

    int pressed=wait_for_button_press();

    if(pressed==pb_up){
        delay(200);
        temp_hour+=1;
        temp_hour=temp_hour % 24;
      }

    else if(pressed==pb_down){
        delay(200);
        temp_hour-=1;
        if(temp_hour<0){
          temp_hour=23;
        }
      }

    else if(pressed==pb_ok){
        delay(200);
        hours=temp_hour;
        break;
      }

    else if(pressed==pb_cancel){
        delay(200);
        break;
    }
      update_time();    
    }

    int temp_minute=minutes;
    while(true){
      lcd.clear();  
      print_text(("Minute :"+String(temp_minute)),0,0,1);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          temp_minute+=1;
          temp_minute=temp_minute % 60;
        }

      else if(pressed==pb_down){
          delay(200);
          temp_minute-=1;
          if(temp_minute<0){
            temp_minute=59;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          minutes=temp_minute;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
        update_time();
      }

      lcd.clear();  
      print_text(F("Time is set"),0,0,1);
      delay(1500);
  }
  



void set_alarm(){
    ring_alarm();
    int temp_hour=alarm_hours[0];
    while(true){
      lcd.clear();  
      print_text(("Hour :"+String(temp_hour)),0,0,1);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          temp_hour+=1;
          temp_hour=temp_hour % 24;
        }

      else if(pressed==pb_down){
          delay(200);
          temp_hour-=1;
          if(temp_hour<0){
            temp_hour=23;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          alarm_hours[0]=temp_hour;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
        update_time();
      }

      int temp_minute=alarm_minutes[0];
      while(true){
        lcd.clear();  
        print_text(("Minute :"+String(temp_minute)),0,0,1);

        int pressed=wait_for_button_press();

        if(pressed==pb_up){
            delay(200);
            temp_minute+=1;
            temp_minute=temp_minute % 60;
          }

        else if(pressed==pb_down){
            delay(200);
            temp_minute-=1;
            if(temp_minute<0){
              temp_minute=59;
            }
          }

        else if(pressed==pb_ok){
            delay(200);
            alarm_minutes[0]=temp_minute;
            break;
          }

        else if(pressed==pb_cancel){
            delay(200);
            break;
          }
          update_time();
        }

        lcd.clear();  
        print_text(F("Alarm is set"),0,0,1);
        alarm_enabled=true;
        delay(1500);

  }

void set_moisture(){
    int m_upper_threshold=moisture[0];
    while(true){
      lcd.clear();  
      print_text(("Upper Level: "+String(m_upper_threshold)),0,0,1);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          m_upper_threshold+=1;
          m_upper_threshold=m_upper_threshold % 100;
        }

      else if(pressed==pb_down){
          delay(200);
          m_upper_threshold-=1;
          if(m_upper_threshold<0){
            m_upper_threshold=100;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          moisture[0]=m_upper_threshold;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
      update_time();
      }

      int m_lower_threshold=moisture[1];
      while(true){
        lcd.clear();  
        print_text(("Lower Level:"+String(m_lower_threshold)),0,0,2);

        int pressed=wait_for_button_press();

        if(pressed==pb_up){
            delay(200);
            m_lower_threshold+=1;
            m_lower_threshold=m_lower_threshold % 100;
          }

        else if(pressed==pb_down){
            delay(200);
            m_lower_threshold-=1;
            if(m_lower_threshold<0){
              m_lower_threshold=100;
            }
          }

        else if(pressed==pb_ok){
            delay(200);
            moisture[1]=m_lower_threshold;
            break;
          }

        else if(pressed==pb_cancel){
            delay(200);
            break;
          }
        update_time();
        }

        lcd.clear();  
        print_text(F("Moisture Levels"),0,0,1);
        lcd.setCursor(0, 1);
        lcd.print("are set");

        delay(1500);

  }
void set_temp(){
    int upper_threshold=temperature[0];
    while(true){
      lcd.clear();  
      print_text(("Upper Level: "+String(upper_threshold)),0,0,1);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          upper_threshold+=1;
          upper_threshold=upper_threshold % 50;
        }

      else if(pressed==pb_down){
          delay(200);
          upper_threshold-=1;
          if(upper_threshold<0){
            upper_threshold=50;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          temperature[0]=upper_threshold;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
      update_time();          
      }

      int lower_threshold=temperature[1];
      while(true){
        lcd.clear();  
        print_text(("Lower Level: "+String(lower_threshold)),0,0,1);

        int pressed=wait_for_button_press();

        if(pressed==pb_up){
            delay(200);
            lower_threshold+=1;
            lower_threshold=lower_threshold % 50;
          }

        else if(pressed==pb_down){
            delay(200);
            lower_threshold-=1;
            if(lower_threshold<0){
              lower_threshold=50;
            }
          }

        else if(pressed==pb_ok){
            delay(200);
            temperature[1]=lower_threshold;
            break;
          }

        else if(pressed==pb_cancel){
            delay(200);
            break;
          }
        update_time();          
        }

        lcd.clear();  
        print_text(F("Temp Levels are"),0,0,1);
        lcd.setCursor(0, 1);
        lcd.print("set");
        delay(1500);

  }
void set_humidity(){
    int h_upper_threshold=humidity[0];
    while(true){
      lcd.clear();  
      print_text(("Upper Level: "+String(h_upper_threshold)),0,0,1);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          h_upper_threshold+=1;
          h_upper_threshold=h_upper_threshold % 100;
        }

      else if(pressed==pb_down){
          delay(200);
          h_upper_threshold-=1;
          if(h_upper_threshold<0){
            h_upper_threshold=100;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          temperature[0]=h_upper_threshold;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
        update_time();
      }

      int h_lower_threshold=temperature[1];
      while(true){
        lcd.clear();  
        print_text(("Lower Level: "+String(h_lower_threshold)),0,0,1);

        int pressed=wait_for_button_press();

        if(pressed==pb_up){
            delay(200);
            h_lower_threshold+=1;
            h_lower_threshold=h_lower_threshold % 50;
          }

        else if(pressed==pb_down){
            delay(200);
            h_lower_threshold-=1;
            if(h_lower_threshold<0){
              h_lower_threshold=50;
            }
          }

        else if(pressed==pb_ok){
            delay(200);
            humidity[1]=h_lower_threshold;
            break;
          }

        else if(pressed==pb_cancel){
            delay(200);
            break;
          }
          update_time();          
        }

        lcd.clear();  
        print_text(F("Humidity Levels"),0,0,1);
        lcd.setCursor(0,1);   
        lcd.print("are set");
        delay(1500);

  }  
// int readMoistureSensor() {
// 	digitalWrite(moisturesensorPower, HIGH);	// Turn the sensor ON
// 	delay(10);							// Allow power to settle
// 	int val = analogRead(moisturesensor);	// Read the analog value form sensor
// 	digitalWrite(moisturesensorPower, LOW);		// Turn the sensor OFF
// 	return val;							// Return analog moisture value
// }

// void check_sensors(){
//   TempAndHumidity data= dhtsensor.getTempAndHumidity();
//   if(data.temperature>temperature[0]){
//     lcd.clear();  
//     print_text("Temp Level High",0,40,1);
//   }
//   else if(data.temperature<temperature[1]){
//     lcd.clear();  
//     print_text("Temp Level Low",0,40,1);
//   }
//   if(data.humidity>humidity[0]){
//     lcd.clear();  
//     print_text("Humidity Level High",50,40,1);
//   }
//   else if(data.humidity<humidity[1]){
//     lcd.clear();  
//     print_text("Humidity Level Low",50,40,1);
//   }
//   if(readMoistureSensor()>moisture[0]){
//     lcd.clear();  
//     print_text("Moisture Level High",50,40,1);
//   }
//   else if(readMoistureSensor()<moisture[1]){
//     lcd.clear();  
//     print_text("Moisture Level Low",50,40,1);
//     ring_alarm();
//   } 
//  print_text("Soil Moisture Level: "+String(readMoistureSensor()),50,10,1);
//  print_text("Temperature: "+String(data.temperature),50,30,1);
//  print_text("Humidity: "+String(data.humidity),50,50,1);
// }

void BlinkLED(){
 digitalWrite(LED_1, HIGH);
 delay(1000);
 
 bool break_happened = false;
 
 while(break_happened == false && digitalRead(pb_cancel) == HIGH){
 
    if(digitalRead(pb_cancel)==LOW){
        delay(200);
        break_happened=true;
        break;
   }
  update_time();  
 }
  digitalWrite(LED_1, LOW);
  delay(1000);
}
void BlinkLED1(){
  digitalWrite(LED_1, HIGH);
  delay(1000);
  digitalWrite(LED_1, LOW);   
  delay(1000);
}
 