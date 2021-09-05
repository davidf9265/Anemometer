//#ifndef __INTELLISENSE__ sleep_enable(); #endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <SPI.h>
#include <SD.h>
//#include <Wire.h> // importado en el RTC.h
#include "RTC.h"


//parametros globales (macros)
#define SAMPLE_SIZE 10
#define SAVE_FULL_TIME false
#define DEBUG_SPEED 2.887f
#define POWER_CIRCUIT_PHASE 10 //10 milisegundos
//#define RTC_ADDRESS 0x68 //B1101000

//variables globales de estado
bool flag_RTC, 
        flag_speed_detect,
        flag_statistics,
        flag_store,
        flag_show,
        flag_sleep = false;

//variables globales de tiempo
byte DD,
        MM,
        AA,
        DY,
        h,
        m,
        s = 0;

//variable para el archivo de la SD
File myFile;

//función sleep
void goingToSleep(){
    attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);
    Serial.println("Int OK");
    
    sleep_enable();
    
    Serial.println("Set sleep");
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    //digitalWrite(LED_BUILTIN, LOW);
    
    Serial.println("cpu off");
    delay(5000);
    sleep_cpu();

    Serial.println("woke up!");
    //digitalWrite(LED_BUILTIN, HIGH);
}

//función wake up
void wakeUp(){
    Serial.println("int.");
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(2));
    flag_RTC = true;
}




void setup()
{
    //configuración de pines
    pinMode(2, INPUT_PULLUP); //int0
    //pinMode(LED_BUILTIN, OUTPUT); //LED  pin

    //pin habilitación de circuitos
    pinMode(7, OUTPUT); 

    //habilitación de SD
    //pinMode(1, OUTPUT);

	//ajuste Debugging
    Serial.begin(115200);
    //ajuste del RTC y configuración general:
    //Iniciar conexión con el RTC
    //dirección generica del DS
    delay(1000);

    Serial.println("RTC OK");
    Wire.begin(RTC_ADDRESS);
    set_RTC_time(29, 8, 7, 11); //date, month, day, hour

    //configurar alarma
    Serial.println("CA");
    set_RTC_alarm();

    delay(1000);
    //flag sleep inicial
    flag_sleep = true;

    if(!SD.begin(10)){
        Serial.println("SD broke");
        return;
    }

    delay(5000);
}




void loop()
{
    //Print hour
    Serial.print("h: ");
    Serial.print(h = get_RTC_hour());
    Serial.print(" m: ");
    Serial.print(m = get_RTC_mins());
    Serial.print(" s: ");
    Serial.println(s = get_RTC_secs());

    Serial.print("Stat: ");
    Serial.println(get_RTC_data(0x0F,"stat",false),BIN);

	if(flag_RTC){
        digitalWrite(7, HIGH);
        //habilitación de los circuitos perifericos
        
        Serial.println("Perifericos OK");
        
        //espera a que se activen los perifericos
        delay(POWER_CIRCUIT_PHASE);

        flag_RTC = false;
        flag_speed_detect = true;
    }

    if(flag_speed_detect){
        //el bucle for se utiliza como verificador de
        //muestra completa
        delay(5000);
        for(int i = 0; i > SAMPLE_SIZE ; i++){
            //algoritmo de detección de velocidad
        }

        Serial.println("vel. viento OK");

        flag_speed_detect = false;
        flag_statistics = true;
    }

    if(flag_statistics){
        
        //promedios y otros cálculos estadísticos
        Serial.println("Stat. OK");

        flag_statistics = false;
        flag_store = true;
    }

    if(flag_store){
        //store in SD
        Serial.print("op");

        delay(1000);
        myFile = SD.open("data.txt", FILE_WRITE);

        if(myFile){
            if(SAVE_FULL_TIME){
                //nothing yet
            }
            else{
                //myFile.print("h"+h);
                myFile.println("h"
                                + String(h)
                                + ",m" 
                                + String(m) 
                                + ",s" 
                                + String(s)
                                + ",v" 
                                + String(DEBUG_SPEED,2));
                Serial.print("Wr");
            }
            Serial.print("Cl");
            myFile.close();
        }
        
        Serial.println("Store OK");
        delay(1000);

        flag_store = false;
        flag_show = true;
    }

    if(flag_show){
        
        //mostrar en LCD
        Serial.println("LCD OK");
        
        flag_show = false;
        flag_sleep = true;
    }

    if(flag_sleep){
        Serial.println("Sleeping");
        digitalWrite(7, LOW);
        //modo de bajo consumo

        flag_sleep = false;

        goingToSleep();

        //limpiar el registro de control de estado
        //para evitar que el micro vuelva a entrar
        //en bajo consumo.
        flush_status();
    }
}
