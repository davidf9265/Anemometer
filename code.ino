#include <avr/io.h>
#include <avr/interrupt.h>
//#include <Wire.h>
#include "RTC.h"


//parametros globales
#define SAMPLE_SIZE 10
//#define RTC_ADDRESS 0x68 //B1101000

//variables globales de estado
bool flag_RTC, 
        flag_speed_detect,
        flag_statistics,
        flag_store,
        flag_show,
        flag_reset_alarm,
        flag_sleep = false;


//estado "wake up"
//ISR(){
//
//}



void setup()
{
	//ajuste Debugging
    Serial.begin(9600);
    //ajuste del RTC y configuración general:
    //Iniciar conexión con el RTC
    //dirección generica del DS
    
    Serial.println("Iniciando RTC...");
    Wire.begin(RTC_ADDRESS);
    set_RTC_time(29, 8, 7, 11); //date, month, day, hour

    Serial.println("Configurando alarma predefinida...");
    set_RTC_alarm();
    //configurar alarma


}

void loop()
{
    Serial.print("Hora: ");
    Serial.print(get_RTC_hour());
    Serial.print(" Minutos: ");
    Serial.print(get_RTC_mins());
    Serial.print(" Segundos: ");
    Serial.println(get_RTC_secs());
    delay(10000);

	if(flag_RTC){
        //habilitación de los circuitos perifericos
        flag_RTC = false; 
        flag_speed_detect = true;
    }

    if(flag_speed_detect){
        //el bucle for se utiliza como verificador de
        //muestra completa
        for(int i = 0; i > SAMPLE_SIZE ; i++){
            //algoritmo de detección de velocidad
        }
        flag_speed_detect = false;
        flag_statistics = true;
    }

    if(flag_statistics){
        //promedios y otros cálculos estadísticos
        flag_statistics = false;
        flag_store = true;
    }

    if(flag_store){
        //store in SD
        flag_store = false;
        flag_show = true;
    }

    if(flag_show){
        //mostrar en SD
        flag_show = false;
        flag_reset_alarm = true;
    }

    if(flag_reset_alarm){
        //mostrar en SD
        flag_reset_alarm = false;
        flag_sleep = true;
    }

    if(flag_sleep){
        //modo de bajo consumo
        flag_sleep = false;
    }
}
