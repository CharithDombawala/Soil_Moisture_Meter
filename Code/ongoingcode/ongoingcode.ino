#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>


#define  SCREEN_WIDTH 128
#define  SCREEN_HEIGHT 64
#define  OLED_RESET -1
#define  SCREEN_ADDRESS 0x3C

#define BUZZER 14
#define LED_1 19
#define pb_cancel 3  
#define pb_ok 2 
#define pb_down 5 
#define pb_up 4 
#define DHTpin 15
#define moisturesensor 23 
#define miosturesensordigital  12



DHTesp dhtsensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Global variable

 int days=0;
 int hours=0;
 int minutes=0;
 int seconds=0;

 unsigned long timeNow=0;
 unsigned long timeLast=0;
 
int moisture[]={0,0} 
int temperature[]={0,0}
int humidity[]={0,0}

bool alarm_enabled=true;
int alarm_hours[]={0};
int alarm_minutes[]={0};
bool alarm_triggered[]={false};

int BUZZERFrequency = 1000;
int BUZZERDuration = 200;

int current_mode=0;
int n_mode=6;
String modes[]={"1-Set Time","2-Set moisture Threshold","3-Set Temp Threshold","4-Set Humidity Threshold","5-Set Alarm","6-Bluetooth mode"};

void setup() {
  // put your setup code here, to run once:


  //pinMode(BUZZER,OUTPUT);
  //pinMode(LED_1,OUTPUT);
  pinMode(pb_cancel,INPUT_PULLUP);
  pinMode(pb_ok, INPUT_PULLUP);
  pinMode(pb_down, INPUT_PULLUP);
  pinMode(pb_up, INPUT_PULLUP);
  //pinMode(DHTpin, INPUT);

  //dhtsensor.setup(DHTpin,DHTesp::DHT22);

  Serial.begin(115200);

  //ssd1306_swithhcapvcc = generate display voltage from 3.3v internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            while (true);  //for(;;);
}

  display.display();
  delay(2000);
  
  display.clearDisplay();
  print_text("Welcome",10,28,2);
  delay(2000);
  display.clearDisplay(); 

}

void loop() {
  // put your main code here, to run repeatedly:
  update_time_with_check_alarm();
if (digitalRead(pb_ok)==LOW){
     delay(200);
     go_to_menu();
}
    check_sensors();
}




void print_text(String text, int column , int row, int text_size){
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column,row);
  display.println(text);
  display.display();

}

void print_time_now(){
  display.clearDisplay(); 
  print_text(String(days),0,0,2);
  print_text(":",20,0,2);
  print_text(String(hours),30,0,2);
  print_text(":",50,0,2);
  print_text(String(minutes),60,0,2);
  print_text(":",80,0,2);
  print_text(String(seconds),90,0,2);

}

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


void ring_alarm(string type){
  display.clearDisplay();
  
  print_text(type,0,0,2);
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
  display.clearDisplay();

}

void update_time_with_check_alarm(){
  update_time();
  print_time_now();

  if(alarm_enabled){
    
      if(alarm_triggered[0]==false && alarm_hours[0]==hours && alarm_minutes[0]==minutes){
          ring_alarm("Time to water");
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
    display.clearDisplay();
    print_text(modes[current_mode],0,0,2);

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
    // else if(current_mode==5){
    //   set_bluetooth();       
    // }    
}
void set_time(){
  int temp_hour=hours;
  while(true){
    display.clearDisplay();
    print_text("Enter hour :"+String(temp_hour),0,0,2);

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
      display.clearDisplay();
      print_text("Enter minute :"+String(temp_minute),0,0,1);

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

      display.clearDisplay();
      print_text("Time is set",0,0,1);
      delay(1500);
  }
  



void set_alarm(){
    int temp_hour=alarm_hours[0];
    while(true){
      display.clearDisplay();
      print_text("Enter hour :"+String(temp_hour),0,0,1);

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
          alarm_hours[alarm]=temp_hour;
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
        display.clearDisplay();
        print_text("Enter minute :"+String(temp_minute),0,0,1);

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
            alarm_minutes[alarm]=temp_minute;
            break;
          }

        else if(pressed==pb_cancel){
            delay(200);
            break;
          }
          update_time();
        }

        display.clearDisplay();
        print_text("Alarm is set",0,0,1);
        alarm_enabled=true;
        delay(1500);

  }

void set_moisture(){
    int m_upper_threshold=moisture[0];
    while(true){
      display.clearDisplay();
      print_text("Enter moisture Upper threshold :"+String(m_upper_threshold),0,0,1);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          m_upper_threshold+=1;
          m_upper_threshold=m_upper_threshold % 100;
        }

      else if(pressed==pb_down){
          delay(200);
          upper_threshold-=1;
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
        display.clearDisplay();
        print_text("Enter moisture lower threshold :"+String(m_lower_threshold),0,0,2);

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

        display.clearDisplay();
        print_text("Moisture thresholds are set",0,0,1);
        delay(1500);

  }
void set_temp(){
    int upper_threshold=temperature[0];
    while(true){
      display.clearDisplay();
      print_text("Enter temp Upper threshold :"+String(upper_threshold),0,0,1);

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
        display.clearDisplay();
        print_text("Enter temp lower threshold :"+String(lower_threshold),0,0,1);

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

        display.clearDisplay();
        print_text("Temperature thresholds are set",0,0,1);
        delay(1500);

  }
void set_humiditiy(){
    int h_upper_threshold=humidity[0];
    while(true){
      display.clearDisplay();
      print_text("Enter Humidity Upper threshold :"+String(h_upper_threshold),0,0,1);

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
        display.clearDisplay();
        print_text("Enter Humidity lower threshold :"+String(h_lower_threshold),0,0,1);

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

        display.clearDisplay();
        print_text("Humidity thresholds are set",0,0,1);
        delay(1500);

  }  
void check_sensors(){
  TempAndHumidity data= dhtsensor.getTempAndHumidity();
  if(data.temperature>temperature[0]){
    display.clearDisplay();
    print_text("Temp Level High",0,40,1);
  }
  else if(data.temperature<temperature[1]){
    display.clearDisplay();
    print_text("Temp Level Low",0,40,1);
  }
  if(data.humidity>humidity[0]){
    display.clearDisplay();
    print_text("Humidity Level High",50,40,1);
  }
  else if(data.humidity<humidity[1]){
    display.clearDisplay();
    print_text("Humidity Level Low",50,40,1);
  }
   // update krnn one 
//
}