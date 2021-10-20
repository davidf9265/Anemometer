#define pwmPin 6 // pin pwm
#define ledPin 4 // pin prueba
//#define ledPin2 4 // pin prueba 2
//#define ledPin3 7 // pin prueba 3
#define Fclk 16000000.0 // frecuencia del relog
#define selPin 7 // pin para seleccionar emisor-receptor
#define distancia_sensores 0.10
#define regFrecuency 199 //este valor determina el valor del registo de comparacion con prescales de 1 frecuencia 40 kHz clk=16 MHz, segun calculos debe ser 199 segun ensayo y error 12

// VARIABLES GLOBALES

double regtime_pwm=0; // registro numero de overflow pwm
double regtime_vuelo=0; // registro numero de overflow contador tiempo de vuelo
double v=0;

bool flag_ic; // flag "input capture"
int flag_contador=0;

void config_PWM()
{
  pinMode(pwmPin,OUTPUT); 
  pinMode(ledPin,OUTPUT); 
  // pinMode(ledPin2,OUTPUT);
  //  pinMode(ledPin3,OUTPUT);
  pinMode(selPin, OUTPUT);
  //pwm_on(199);
  //Serial.begin(9600);
  // contador_tiempo_vuelo_on();
}
void loop()
{
  contador_tiempo_vuelo_off();
  v=deteccion_velocidad();
 
  Serial.print("velocidad: "); 
  //Serial.print("\n");
  //Serial.print(' ');
// contador_tiempo_vuelo_on();
// digitalWrite(ledPin2, digitalRead(ledPin2) ^ 1);
 // m=leer_reg_cont();
 // Serial.print("tiempo de vuelo: "); 
  Serial.println(v);
  //Serial.println("   ");

  //while(1);

}
//INTERRUPCIONES PWM


//INTERRUPCIONES CONTADOR TIEMPO DE VUELO
ISR(TIMER1_OVF_vect)          // interrupción por igualdade de comparación en TIMER1
{
   // digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
    pwm_off();
    addRegTime(regtime_vuelo);
   
    
  
}
ISR(TIMER1_CAPT_vect)          // interrupción por igualdade de comparación en TIMER1
{
    contador_tiempo_vuelo_off();
    flag_ic=true; //flag producido pro una deteccion de señal
   digitalWrite(ledPin, digitalRead(ledPin) ^ 1);

   
  
}
//METODOS
// PWM

void pwm_on(int Frecuency){ // activa el pwm
  pinMode(pwmPin, OUTPUT); // habilita la salida del PWM  
  // Configuracion del TIMER2
  TCCR0A = 0;                // El registro de control A queda todo en 0
  TCCR0B = 0;                //limpia registrador
  TCNT0  = 0;                //Inicializa el temporizador
  TCCR0A |=(1<<COM0A0);      // TOGGLE on compare match (CTC)
  OCR0A = Frecuency;            // carga el registrador de comparación: 16MHz/1024/1Hz -1 = 15624 = 0X3D08
  TCCR0A |= (1 << WGM01);   // activacion modo CTC
  TIMSK0 |= (1 << OCIE0A);  // habilita interrupción por igualdade de comparación
  TCCR0B |= (1<<CS00);   // clock configuration no prescaler  
}

void pwm_off(){ // desactiva el pwm
   TCCR0B&=~((1<<CS00)); //detiene el reloj
  
}


// CONFIGURACION TIEMPO DE VUELO
void contador_tiempo_vuelo_off(){
  TIMSK1=0;
  TCCR1B=(1<<WGM13); //detiene la deteccion de la señal de entrada
   TCCR1B&=~((1<<CS10)); //detiene el reloj
    
   pwm_off();
  
   flag_contador=0;
   
  }
void contador_tiempo_vuelo_on(){
  TCCR1A = 0;                        // El registro de control A queda todo en 0, pines OC1A y OC1B deshabilitados
  TCCR1B = 0;                        //limpia el registrador
  TIMSK1=0;
  TCNT1 =0;
  TCCR1B |= (1<<ICNC1)|(1<<ICES1)|(1<<CS10); // ICNC1: activa modulo cancelacion de ruido, ICESI:  activa el "edge detector", CS10: configura relog sin preescaler
  TIMSK1|=(1<<ICIE1)|(1<<TOIE1); // activa interrupciones ICIE1: input capture mode,  TOIE1: counter overflow.
  flag_contador=1;
  
  }
  
//SELEECCION EMISOR RECEPTOR
void seleccion_emisor_receptor( int pin){ // a: registro 1 tiempo de vuelo, pin: el pin de salida que se usara
    
    digitalWrite(pin, digitalRead(pin) ^ 1);
    
  }

//LEER REGISTRO
double leer_reg_cont(){
    double tm=0.0;    
    double regtiempoL; // registro tiempo de vuelo parte baja
    double regtiempoH; // registro tiempo de vuelo parte alta
    regtiempoL=ICR1L; //B0000101;
    regtiempoH=ICR1H; // B00000001;
    regtiempoH=regtiempoH*pow(2,8);
   // tm=regtiempoL+regtiempoH;
   tm=(regtiempoL+regtiempoH+regtime_vuelo*65535);
    return tm;
  }
  


// CALCULO DE VELOCIDADES
double calculo_velocidad(double tva, double tvc, double d){ // tva: tiempo de vuelo a favor, tvc: tiempo de vuelo en contra, d: distancia entre los sensores
  
  double v=d*((1/tva)-(1/tvc))*0.5*Fclk;
  return abs(v);
  }

// metodos auxiliares
void addRegTime(int reg){
  reg=reg+1;
  }

  //DETECCION DE VELOCIDAD
  double deteccion_velocidad(){
    
  //digitalWrite(selPin, LOW);
  double velocidad;
  int i=0; 
  double tiempo_vuelo[]={0,0};
    while(tiempo_vuelo[1]==0){
        regtime_vuelo=0;
        digitalWrite(selPin, digitalRead(ledPin) ^ 1);
        contador_tiempo_vuelo_off();
        //pwm_off();
         
        flag_ic=false;
        while( flag_ic==false){
       
            if(flag_contador==0){
               
               pwm_on(regFrecuency); // poner limite tiempo al pwm
               contador_tiempo_vuelo_on();
                }
            if(regtime_vuelo>244){
               //regtime_vuelo=0;
              contador_tiempo_vuelo_off();
               
               }
        }

  contador_tiempo_vuelo_off(); 

  tiempo_vuelo[i]=leer_reg_cont();
 
      
    //Serial.print(" tof1 ");
    //Serial.print(tiempo_vuelo[0]);
    //Serial.print(" tof2 ");
    //Serial.print(tiempo_vuelo[1]);
    //Serial.print("  ");
    i=i+1; 
   }
    //Serial.print(" tof1 ");
    //Serial.print(tiempo_vuelo[0]);
   /// Serial.print(" tof2 ");
    //Serial.print(tiempo_vuelo[1]);
    //Serial.print("  ");
   return calculo_velocidad(tiempo_vuelo[0], tiempo_vuelo[1], distancia_sensores);
  }
//double concatenar_registros(byte a, byte b){

  //}
