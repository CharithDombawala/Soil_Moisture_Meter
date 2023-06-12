#include <DHT.h>
#include <LiquidCrystal_I2C.h>


byte BUZZER = 8;     //14        // sudu- button 2, kalu button 1,duburu- button 4, rathu-button 3    //  1 - gnd , 2- button 3, 3- button 4, 4- button 1, 5-button 2
byte LED_1 = 13 ;    //19
byte pb_cancel = 4;  //3
byte pb_ok = 5;      //2
byte pb_down = 2;    //5
byte pb_up = 3 ;     //4
byte DHTpin = 9 ;    //15


byte moisturesensorPower = 6;  //12   //3-nil  4-kaha  2-rathu 1 -kola
byte moisturesensor= A0;      //23


DHT dht(DHTpin, DHT11);

LiquidCrystal_I2C lcd(0x3F, 16, 2);
//Global variable

byte hours = 10;    // Range: 0-255
byte minutes = 0;  // Range: 0-255
byte seconds = 0;

int timeNow = 0;
int timeLast = 0;

float moisture[] = { 800, 300 };
byte temperature[] = { 40, 15 };
byte humidity[] = { 85, 60 };

bool alarm_enabled = false;
byte alarm_hours[] = { 5 };
byte alarm_minutes[] = { 54 };
bool alarm_triggered = true;

int BUZZERFrequency = 5000;
byte BUZZERDuration = 750;

byte current_mode = 0;
byte n_mode = 7;
String modes[] = { "Measure moisture","Set moisture", "Set Temparature", "Set Humidity","Set Time", "Set Alarm", "Bluetooth mode" };

void setup() {
  // put your setup code here, to run once:


  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(pb_cancel, INPUT_PULLUP);
  pinMode(pb_ok, INPUT_PULLUP);
  pinMode(pb_down, INPUT_PULLUP);
  pinMode(pb_up, INPUT_PULLUP);
  pinMode(DHTpin, INPUT);
  pinMode(moisturesensorPower, OUTPUT);
  pinMode(moisturesensor, INPUT);

  dht.begin();
  // Initially keep the sensor OFF
  digitalWrite(moisturesensorPower, LOW);

  lcd.init();
  lcd.clear();
  lcd.backlight();  // Make sure backlight is on

  // Print a message on both lines of the LCD.
  lcd.setCursor(5, 0);  //Set cursor to character 2 on line 0
  lcd.print("Welcome");
  delay(2000);
}

void loop() {
  //  // put your main code here, to run repeatedly:
  update_time_with_check_alarm();


  if (digitalRead(pb_up)==LOW){
       delay(200);
       go_to_menu();
  }
  
  
  check_sensors();
}


void print_text(String text, int column, int row) {
  lcd.clear();
  lcd.setCursor(column, row);  //Set cursor to character 2 on line 0
  lcd.print(text);
}


void update_time() {
  timeNow = millis() / 1000;
  seconds = timeNow - timeLast;
  if (seconds >= 60) {
    minutes += 1;
    timeLast += 60;
  }
  if (minutes == 60) {
    hours += 1;
    minutes = 0;
  }
  if (hours == 24) {
    
    hours = 0;
  }
}


void ring_alarm() {

  lcd.clear();
  print_text(F("Alarm"), 0, 0);
  digitalWrite(LED_1, HIGH);

  bool break_happened = false;

  while (break_happened == false && digitalRead(pb_cancel) == HIGH ) {

    if (digitalRead(pb_cancel) == LOW) {
      delay(200);
      break_happened = true;
      
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

void update_time_with_check_alarm() {
  
  update_time();
  if (alarm_enabled) {

    if (alarm_triggered == false && alarm_hours[0] == hours && alarm_minutes[0] == minutes && alarm_enabled==true) {
      ring_alarm();
      alarm_triggered = true;
    }
  }
}


int wait_for_button_press() {
  while (true) {
    if (digitalRead(pb_ok) == LOW) {
      delay(200);
      return pb_ok;
    } else if (digitalRead(pb_up) == LOW) {
      delay(200);
      return pb_up;
    } else if (digitalRead(pb_down) == LOW) {
      delay(200);
      return pb_down;
    } else if (digitalRead(pb_cancel) == LOW) {
      delay(200);
      return pb_cancel;
    }
    update_time();
  }
}

int wait_for_button_press2() {
  while (true) {
   
    lcd.setCursor(8,0);
    lcd.print(String(hours)+":"+String(minutes)+":"+String(seconds));    
    
    if (digitalRead(pb_ok) == LOW) {
      delay(200);
      return pb_ok;
    } else if (digitalRead(pb_up) == LOW) {
      delay(200);
      return pb_up;
    } else if (digitalRead(pb_down) == LOW) {
      delay(200);
      return pb_down;
    } else if (digitalRead(pb_cancel) == LOW) {
      delay(300);
      return pb_cancel;
    }
    update_time();
  }
}



void go_to_menu() {
  while (digitalRead(pb_cancel) == HIGH) {
    lcd.clear();
    print_text((modes[current_mode]), 0, 1);
    lcd.setCursor(12, 0);
    lcd.print("Menu");


    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      current_mode += 1;
      current_mode = current_mode % n_mode;
    }

    else if (pressed == pb_down) {
      delay(200);
      current_mode -= 1;
      if (current_mode < 0) {
        current_mode = n_mode - 1;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      run_mode(current_mode);
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }
}


void run_mode(int current_mode) {      //{"Measure moisture","Set moisture", "Set Temparature", "Set Humidity","Set Time", "Set Alarm", "Bluetooth mode"}
  if (current_mode == 0) {
    Measure_moisture();
  } else if (current_mode == 1) {
    set_moisture();
  } else if (current_mode == 2) {
    set_temp();
  } else if (current_mode == 3) {
    set_humidity();
  } else if (current_mode == 4) {
    set_time(); 
  } else if (current_mode == 5) {
    alarm();
  } else if (current_mode == 6) {
    print_text(F("BluetoothMode"), 40, 40);
  }
}

void Measure_moisture(){

while(true){
     update_time();
     float step= ((moisture[0]-moisture[1])/5) ;
     int reading=readMoistureSensor();     
    if (moisture[1]>reading) {
         print_text("Saturated (" +String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry");
         BlinkLED();
         delay(1000);        
         
  } else if ((moisture[1]<=reading) && (reading <(moisture[1]+step))) {
         print_text("Too Wet ("+String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry"); 
        delay(1000);        
   
  } else if ((moisture[1]+step<=reading) && (reading <(moisture[1]+2*step))) {
         print_text("Wet ("+String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry"); 
         delay(1000);        
 
  } else if ((moisture[1]+2*step<=reading) && (reading<moisture[1]+3*step)) {
         print_text("Noramal ("+String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry");  
         delay(1000);  
  } else if ((moisture[1]+3*step<=reading ) && (reading<moisture[1]+4*step)) {
         print_text("Dry ("+String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry");  
        delay(1000);   
  } else if ((moisture[1]+4*step<reading ) && (reading<moisture[1]+5*step)) {
         print_text("Too Dry ("+String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry"); 
         delay(1000);            
  } else if ((moisture[0]<=reading) && (reading<=1000)) {
         print_text("Over Dry ("+String(reading)+")",0,1);
         lcd.setCursor(9,0); 
         lcd.print(String(reading/10)+"%"+" Dry"); 
         BlinkLED();
         delay(1000);  
  } else if (reading>1000) {         
         print_text("Over Dry ("+String(reading)+")",0,1);
         lcd.setCursor(8,0); 
         lcd.print("100% Dry"); 
         BlinkLED();
         delay(1000);   
   }
  
  if ((digitalRead(pb_up)==LOW) || (digitalRead(pb_cancel)==LOW)){
       delay(200);
       break;
  }
}
}


void set_time() {
  int temp_hour = hours;
  while (true) {
    
    print_text(("Hour :" + String(temp_hour)), 0, 1);
    lcd.setCursor(8,0);
    lcd.print(String(hours)+":"+String(minutes)+":"+String(seconds));
    
    int pressed = wait_for_button_press2();

    if (pressed == pb_up) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == pb_down) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      hours = temp_hour;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  int temp_minute = minutes;
  while (true) {
   
    print_text(("Minute :" + String(temp_minute)), 0, 1);
    lcd.setCursor(8,0);
    lcd.print(String(hours)+":"+String(minutes)+":"+String(seconds));    

    int pressed = wait_for_button_press2();

    if (pressed == pb_up) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == pb_down) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      minutes = temp_minute;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  lcd.clear();
  print_text(F("Time is set"), 0, 0);
  delay(1500);
}

void alarm(){
  while (true){
      
 print_text("To enable- Up",0,0);
 lcd.setCursor(0,1);
 lcd.print("To disable- Down");

 int pressed = wait_for_button_press();

 if (pressed == pb_up) {
      delay(200);
      alarm_enabled=true;      
      set_alarm();
      break;
    }

  else if (pressed == pb_down) {
      delay(200);
      alarm_enabled=false;
      break;
      }
 update_time();
}
}

void set_alarm() {
  int temp_hour = alarm_hours[0];
  while (true) {
    print_text(("Hour :" + String(temp_hour)), 0, 1);
    lcd.setCursor(8,0);
    lcd.print(String(hours)+":"+String(minutes)+":"+String(seconds));
    
    int pressed = wait_for_button_press2();

    if (pressed == pb_up) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == pb_down) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      alarm_hours[0] = temp_hour;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  int temp_minute = alarm_minutes[0];
  while (true) {
   
    print_text(("Minute :" + String(temp_minute)), 0, 1);
    lcd.setCursor(8,0);
    lcd.print(String(hours)+":"+String(minutes)+":"+String(seconds));
    
    int pressed = wait_for_button_press2();

    if (pressed == pb_up) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == pb_down) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      alarm_minutes[0] = temp_minute;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  lcd.clear();
  print_text(F("Alarm is set"), 0, 0);
  alarm_triggered= false;
  delay(1500);
}

void set_moisture() {
  int m_upper_threshold = moisture[0];
  while (true) {
    print_text(("Upper Level: " + String(m_upper_threshold)), 0, 1);
    lcd.setCursor(6, 0);
    lcd.print("M_Now: " + String(readMoistureSensor()));


    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      m_upper_threshold += 25;
      m_upper_threshold = m_upper_threshold % 1000;
    }

    else if (pressed == pb_down) {
      delay(200);
      m_upper_threshold -= 25;
      if (m_upper_threshold < 0) {
        m_upper_threshold = 1000;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      moisture[0] = m_upper_threshold;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  int m_lower_threshold = moisture[1];
  while (true) {
    print_text(("Lower Level:" + String(m_lower_threshold)), 0, 1);
    lcd.setCursor(6, 0);
    lcd.print("M_Now: " + String(readMoistureSensor()));

    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      m_lower_threshold += 25;
      m_lower_threshold = m_lower_threshold % 1000;
    }

    else if (pressed == pb_down) {
      delay(200);
      m_lower_threshold -= 25;
      if (m_lower_threshold < 0) {
        m_lower_threshold = 1000;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      moisture[1] = m_lower_threshold;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  lcd.clear();
  print_text(F("Moisture Levels"), 0, 0);
  lcd.setCursor(0, 1);
  lcd.print("are set");

  delay(1500);
}


void set_temp() {
  int upper_threshold = temperature[0];
  while (true) {
    print_text(("Upper Level: " + String(upper_threshold)), 0, 1);
    lcd.setCursor(6, 0);
    lcd.print("T_Now: " + String(dht.readTemperature()));

    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      upper_threshold += 1;
      upper_threshold = upper_threshold % 50;
    }

    else if (pressed == pb_down) {
      delay(200);
      upper_threshold -= 1;
      if (upper_threshold < 0) {
        upper_threshold = 50;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      temperature[0] = upper_threshold;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  int lower_threshold = temperature[1];
  while (true) {

    print_text(("Lower Level: " + String(lower_threshold)), 0, 1);
    lcd.setCursor(6, 0);
    lcd.print("T_Now: " + String(dht.readTemperature()));
 
    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      lower_threshold += 1;
      lower_threshold = lower_threshold % 50;
    }

    else if (pressed == pb_down) {
      delay(200);
      lower_threshold -= 1;
      if (lower_threshold < 0) {
        lower_threshold = 50;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      temperature[1] = lower_threshold;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  print_text(F("Temp Levels are"), 0, 0);
  lcd.setCursor(0, 1);
  lcd.print("set");
  delay(1500);
}
void set_humidity() {
  int h_upper_threshold = humidity[0];
  while (true) {
    
    print_text(("Upper Level: " + String(h_upper_threshold)), 0, 1);
    lcd.setCursor(6, 0);
    lcd.print("H_Now: " + String(dht.readHumidity()));

    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      h_upper_threshold += 1;
      h_upper_threshold = h_upper_threshold % 100;
    }

    else if (pressed == pb_down) {
      delay(200);
      h_upper_threshold -= 1;
      if (h_upper_threshold < 0) {
        h_upper_threshold = 100;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      temperature[0] = h_upper_threshold;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  int h_lower_threshold = humidity[1];
  while (true) {
    
    print_text(("Lower Level: " + String(h_lower_threshold)), 0, 1);
    lcd.setCursor(6, 0);
    lcd.print("H_Now: " + String(dht.readHumidity()));    

    int pressed = wait_for_button_press();

    if (pressed == pb_up) {
      delay(200);
      h_lower_threshold += 1;
      h_lower_threshold = h_lower_threshold % 50;
    }

    else if (pressed == pb_down) {
      delay(200);
      h_lower_threshold -= 1;
      if (h_lower_threshold < 0) {
        h_lower_threshold = 50;
      }
    }

    else if (pressed == pb_ok) {
      delay(200);
      humidity[1] = h_lower_threshold;
      break;
    }

    else if (pressed == pb_cancel) {
      delay(200);
      break;
    }
    update_time();
  }

  lcd.clear();
  print_text(F("Humidity Levels"), 0, 0);
  lcd.setCursor(0, 1);
  lcd.print("are set");
  delay(1500);
}
int readMoistureSensor() {
  digitalWrite(moisturesensorPower, HIGH);  // Turn the sensor ON
  delay(100);                                // Allow power to settle
  int val = analogRead(moisturesensor);     // Read the analog value form sensor
  digitalWrite(moisturesensorPower, LOW);   // Turn the sensor OFF
  return val;                               // Return analog moisture value
}

void check_sensors() {
 
  print_text("Moisture: " + String(readMoistureSensor()), 0, 0);
  if (readMoistureSensor() > moisture[0]) {
      BlinkLED();
      lcd.setCursor(0, 1);
      lcd.print("M_Level High");    
    } 
  else if (readMoistureSensor() < moisture[1]) {
      lcd.setCursor(0, 1);
      lcd.print("M_Level LOW");  
      ring_Buzzer();
  }   
  delay(1000);
  
  print_text("Temperature:" + String(int(dht.readTemperature())), 0, 0);
  lcd.setCursor(14, 0);
  lcd.print("'C");
  if (dht.readTemperature() > temperature[0]) {
      lcd.setCursor(0, 1);
      lcd.print("T_Level High");    
      BlinkLED();   
  } 
  else if (dht.readTemperature() < temperature[1]) {
      lcd.setCursor(0, 1);
      lcd.print("T_Level LOW");    
      BlinkLED();   
  }  
  delay(1000);
  
  print_text("Humidity: " + String(int(dht.readHumidity())), 0, 0);
  lcd.setCursor(12, 0);
  lcd.print("%");
  if (dht.readHumidity() > humidity[0]) {
      lcd.setCursor(0, 1);
      lcd.print("H_Level High");    
      BlinkLED();    
  }
  else if (dht.readHumidity() < humidity[1]) {
      lcd.setCursor(0, 1);
      lcd.print("H_Level LOW");    
      BlinkLED();
  }
  delay(1000);   
}

void BlinkLED() {
  digitalWrite(LED_1, HIGH);
  delay(1000);
  digitalWrite(LED_1, LOW);
  delay(1000);
}
void ring_Buzzer() {
  print_text(F("Please water"), 1, 0);
  lcd.setCursor(2, 1);
  lcd.print("your plants");    
   

  bool break_happened = false;

  while (break_happened == false && digitalRead(pb_cancel) == HIGH) {

    if (digitalRead(pb_cancel) == LOW) {
      delay(200);
      break_happened = true;
      set_moisture();     
      break;
    }
    BlinkLED();
    digitalWrite(BUZZER, HIGH);
    tone(BUZZER, BUZZERFrequency);
    delay(BUZZERDuration);
    noTone(BUZZER);
    delay(BUZZERDuration);
  }
  
}