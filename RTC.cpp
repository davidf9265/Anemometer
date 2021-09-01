#include "RTC.h"

//FUNCIONES DE DESCARGA

byte get_RTC_data(byte reg_address, String name, bool debug){
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(reg_address); //dirige a registro de horas
    Wire.endTransmission(RTC_ADDRESS);

    Wire.requestFrom(RTC_ADDRESS, 1);
    
    byte c;

    while(Wire.available()){
        c = Wire.read();    // receive a byte as character
        if(debug){
            Serial.print(name + ": ");
            Serial.println(c);         // print the character
        }
    }

    return c;
}

byte get_RTC_secs(){
    byte secs = get_RTC_data(0x00,"Segundos",false);

    //interpretar codificación de segundos del RTC
    secs = ( ( (secs & B01110000) >> 4) * 10) + (secs & B00001111);

    return secs;
}

byte get_RTC_mins(){
    byte mins = get_RTC_data(0x01,"Minutos",false);

    //interpretar codificación de segundos del RTC
    mins = ( ( (mins & B01110000) >> 4) * 10) + (mins & B00001111);

    return mins;
}

byte get_RTC_hour(){
    byte hour = get_RTC_data(0x02,"Horas",false);

    //interpretar codificación de segundos del RTC
    hour = ( ( (hour & B00100000) >> 4) * 20) + ( ( (hour & B00010000) >> 4) * 10) + (hour & B00001111);

    return hour;
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
    get_RTC_data(0x00,"Seconds",true);
    get_RTC_data(0x01,"Minutes",true);
    get_RTC_data(0x02,"Hours",true);
    get_RTC_data(0x03,"Days",true);
    get_RTC_data(0x04,"Date",true);
    get_RTC_data(0x05,"Month",true);
    get_RTC_data(0x05,"Year",true);
}

bool set_RTC_alarm(){
    //Programación de la alarma
    //Configuración para activación por hora.
    //A1M4 = 1, A1M3 = 1, A1M2 = 0, A1M1 = 0

    //para A1M1, el registro 0x07
    //además, se necesita configurar los segundos en 0
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x07); //dirige a segundos de alarma
    Wire.write(1<<7); //escribe 0 segundos, activa A1M1
    Wire.endTransmission();

    //para A1M2, el registro 0x08
    //además, se necesita configurar los minutos en 0
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x08); //dirige a segundos de alarma
    Wire.write(1<<7); //escribe 0 segundos, activa A1M1
    Wire.endTransmission();


    Serial.print("Segundos Alarma: ");
    Serial.println(get_RTC_data(0x07,"Segundos Alarma", false),BIN);
    Serial.print("Minutos Alarma: ");
    Serial.println(get_RTC_data(0x08,"Minutos Alarma", false),BIN);
    Serial.print("Horas Alarma: ");
    Serial.println(get_RTC_data(0x09,"Horas Alarma", false),BIN);
}
