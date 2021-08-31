#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>

//parametros globales
#define SAMPLE_SIZE 10
#define RTC_ADDRESS 0x68 //B1101000

//variables globales de estado
bool flag_RTC, 
        flag_speed_detect,
        flag_statistics,
        flag_store,
        flag_show,
        flag_sleep = false;


//estado "wake up"
//ISR(){
//
//}

byte get_RTC_data(byte reg_address, String name){
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(reg_address); //dirige a registro de horas
    Wire.endTransmission(RTC_ADDRESS);

    Wire.requestFrom(RTC_ADDRESS, 1);
    
    while(Wire.available()){
        byte c = Wire.read();    // receive a byte as character
        Serial.print(name + ": ");
        Serial.println(c);         // print the character
    }
}

bool set_RTC_time(int date, int month, int day, int hour){
    //el parametro day recibe un valor entre 1 - 7
    //el parametro date recibe un valor entre 1 - 31 
    //el parametro DYDT permite configurar el RTC para trabajar
    //con día de la semana o día del mes.
    //las horas van en formato 24h.
    //los segundos y minutos son 0 por defecto
    //el año por defecto es 00

    //variables internas
    byte hour_reg = 0x00; //para configurar el registro 0x02 (horas).
    byte date_reg = 0x00;

    //configuración de segundos
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x00); //dirige a registro de segundos
    Wire.write(0x02); //escribe 0 segundos
    Wire.endTransmission();

    //configuración de minutos
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x01); //dirige a registro de minutos
    Wire.write(0x00); //escribe 0 minutos
    Wire.endTransmission();

    //configuración de horas
    //Preparar la escritura de registro
    if(hour / 20 >= 1){
        hour_reg |= 1 << 5; 
        hour_reg += hour % 20;
    }
    else if(hour / 10 >= 1){
        hour_reg |= 1 << 4 ;
        hour_reg += hour % 10;
    }
    else{
        hour_reg += hour;
    }

    //Serial.println(hour_reg);
    //envío I2C horas
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x02); //dirige a registro de horas
    Wire.write(hour_reg); //escribe 0 segundos
    Wire.endTransmission(RTC_ADDRESS);

    //configuración de día de la semana
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x03); //dirige a registro de días
    Wire.write(day); //escribe día del mes
    Wire.endTransmission(RTC_ADDRESS);

    //configuración de día del mes
    //Preparar la escritura de registro
    if(date / 10 >= 1){
        date_reg |= (date / 10) << 4; 
        date_reg += date % 10;
    }
    else{
        date_reg = date;
    }   

    //enviar configuración I2C dia del mes
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x04); //dirige a registro de día del mes
    Wire.write(date_reg); //escribe 0 segundos
    Wire.endTransmission(RTC_ADDRESS);

    //configuración del mes
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x05); //dirige a registro de mes
    Wire.write(0x01); //escribe enero
    Wire.endTransmission(RTC_ADDRESS);

    //configuración del año
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x06); //dirige a registro de año
    Wire.write(0x00); //escribe 00
    Wire.endTransmission(RTC_ADDRESS);

    //Confirmar cofiguración
    get_RTC_data(0x00,"Seconds");
    get_RTC_data(0x01,"Minutes");
    get_RTC_data(0x02,"Hours");
    get_RTC_data(0x03,"Days");
    get_RTC_data(0x04,"Date");
    get_RTC_data(0x05,"Month");
    get_RTC_data(0x05,"Year");
}

bool set_RTC_time_ext(int YEAR, int Month, int day, int date, int hour, int min, int sec, bool DYDT){
    //el parametro day recibe un valor entre 1 - 7
    //el parametro date recibe un valor entre 1 - 31 
    //el parametro DYDT permite configurar el RTC para trabajar
    //con día de la semana o día del mes.
    //las horas van en formato 24h.
}

void setup()
{
	//ajuste Debugging
    Serial.begin(9600);
    //ajuste del RTC y configuración general:
    //Iniciar conexión con el RTC
    //dirección generica del DS
    
    Serial.println("Hello everybody");
    Wire.begin(RTC_ADDRESS);
    set_RTC_time(29, 8, 7, 11);
    //configurar alarma

}

void loop()
{
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
        flag_sleep = true;
    }

    if(flag_sleep){
        //modo de bajo consumo
        flag_sleep = false;
    }
}
