#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include <Wire.h> // importado en el RTC.h
#include "RTC.h"


//parametros globales (macros)
#define SAMPLE_SIZE 10
//#define RTC_ADDRESS 0x68 //B1101000

//variables globales de estado
bool flag_RTC, 
        flag_speed_detect,
        flag_statistics,
        flag_store,
        flag_show,
        flag_sleep = false;


//función sleep
void goingToSleep(){
    attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);
    
    sleep_enable();
    Serial.print("Enabling interrupts");
    
    
    Serial.print("Setting sleep configs");
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    Serial.print("Led off");
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("cpu off");
    //attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);
    delay(5000);
    
    sleep_cpu();
    Serial.println("woke up!");
    digitalWrite(LED_BUILTIN, HIGH);
}


//función wake up
void wakeUp(){
    Serial.print("interrupted");
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(2));
    flag_RTC = true;
    Serial.println("waking up!");
}

void setup()
{
    //configuración de pines
    pinMode(2, INPUT_PULLUP); //int0
    pinMode(LED_BUILTIN, OUTPUT); //LED  pin

    //pin habilitación de circuitos
    pinMode(7, OUTPUT); 


	//ajuste Debugging
    Serial.begin(115200);
    //ajuste del RTC y configuración general:
    //Iniciar conexión con el RTC
    //dirección generica del DS
    delay(1000);

    Serial.println("Iniciando RTC...");
    Wire.begin(RTC_ADDRESS);
    set_RTC_time(29, 8, 7, 11); //date, month, day, hour

    //configurar alarma
    Serial.println("Configurando alarma predefinida...");
    set_RTC_alarm();

    //flag sleep inicial
    flag_sleep = true;

    delay(5000);

    Serial.println("Activando modo sleep...");
}

void loop()
{
    //Print hour
    Serial.print("Hora: ");
    Serial.print(get_RTC_hour());
    Serial.print(" Minutos: ");
    Serial.print(get_RTC_mins());
    Serial.print(" Segundos: ");
    Serial.println(get_RTC_secs());

    Serial.print("Status: ");
    Serial.println(get_RTC_data(0x0F,"Status",false),BIN);

	if(flag_RTC){
        Serial.println("Habilitación de los  circuitos periféricos");
        //habilitación de los circuitos perifericos
        flag_RTC = false;
        flag_speed_detect = true;
    }

    if(flag_speed_detect){
        Serial.println("Detección de velocidad");
        //el bucle for se utiliza como verificador de
        //muestra completa
        for(int i = 0; i > SAMPLE_SIZE ; i++){
            //algoritmo de detección de velocidad
        }
        flag_speed_detect = false;
        flag_statistics = true;
    }

    if(flag_statistics){
        Serial.println("Cálculos estadísticos");
        //promedios y otros cálculos estadísticos
        flag_statistics = false;
        flag_store = true;
    }

    if(flag_store){
        Serial.println("Almacenamiento de la información en SD");
        //store in SD
        flag_store = false;
        flag_show = true;
    }

    if(flag_show){
        Serial.println("Visualización de los datos");
        //mostrar en SD
        flag_show = false;
        flag_sleep = true;
    }

    if(flag_sleep){
        Serial.println("Estado de reposo: Micro 'sleepmode' Circuitos apagados");
        //modo de bajo consumo

        flag_sleep = false;

        goingToSleep();

        //limpiar el registro de control de estado
        //para evitar que el micro vuelva a entrar
        //en bajo consumo.
        flush_status();
    }
}
