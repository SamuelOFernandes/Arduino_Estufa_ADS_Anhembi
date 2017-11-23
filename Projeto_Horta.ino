#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Bibliotéca LCD


LiquidCrystal_I2C lcd(0x3f,2,1,0,4,5,6,7,3, POSITIVE); // Inicializa o display no endereco 0x3f
 


//===// Valor sensor ( Máx: 1024"SECO")( Min: 0 "Supermer molhado") OBS: é preciso análisar/calibrar sempre quando for trocado o sensor//==

const int sensorSW = 4   ;           // Sinal digital de detecção de umidade
const int sensor   = A0  ;           // Pino analógico em que está conectado o sensor
const int Smax     = 0   ;           // valor máximo do sensor de calibração para percentual
const int Smin     = 1024;           // valor mínimo do sensor de calibração para percentual
const int relay    = 2   ;           // Pino em que está conectado o Relay           


//===========================================================// AMOSTRAGEM //==============================================================

int       V              ;           // Armazenamento do valor analógico do sensor de umidade
int       porcen         ;           // Recebe o calculo percentual de umidade
int       porcen2        ;
int       ACbomb         ;           // Variavel de chamada de bomba
String    bomb           ;           // Variavel de status da bomba
int       Recontador     ;           // 


//===================================================// CONFIGURAÇÃO DE IRRIGAÇÃO //=======================================================

unsigned long trega   = 3000 ;       // Tempo de rega em segundos                                     //AJUSTAR SEMPRE
unsigned long tespera = 20000;       // Tempo espera em segundos                                      //AJUSTAR SEMPRE
const int     regraR  = 50   ;       // Resistência (Em kohms) a partir da qual começa a regar        //AJUSTAR SEMPRE
//=========================================================================================================================================

void setup() {
  
  Serial.begin (9600)          ;     // Inicia comunicación serial
  pinMode      (relay,OUTPUT)  ;     // Configurar relay como salida
  pinMode      (sensorSW,INPUT);     // Se (1 = umidade OK) se (0 = Falta Umidade)
  pinMode      (sensor, INPUT) ;     // Configura sensor como entrada
  lcd.begin    (16,2)          ;     // Capacidade do LCD (16 caracteres por linhas/ duas linhas
  
}

void loop() {
  V      =  analogRead(sensor)                     ;       // amarzenamento da leitura sensor
  porcen =  100-((( V - Smin)*100)/(Smax - Smin))  ;       // Calculo do percentual de umidade de acordo com dados analógicos analisados.
  porcen2 = 100-((analogRead(sensor)/7.14)*100)/714;

//================================================= Rotina de Monitoramento Serial=========================================================  
           
  Serial.print("Leitura do Sensor: ");  Serial.print(analogRead(sensor)); Serial.print("\t Saída digital sensor: "); Serial.print(digitalRead(sensorSW));Serial.print("\tUmidade "); Serial.print(porcen); Serial.print("%\t"); Serial.print(bomb);
  Serial.print(porcen2)              ;
  Serial.println("\n")               ;    
  
//=============================================== Rotina de Amostragem e Decisão (Umidade % e Bomba on/off)================================
  lcd.setBacklight(HIGH);
  

  if(porcen<regraR){
    lcd.clear()         ;
    lcd.setCursor(0,0)  ;
    lcd.print("Bomba: ");
    bomb = "ON"         ;            // Status da bomba
    lcd.setCursor(6,0)  ;
    lcd.print(bomb)     ;
    ACbomb = 1          ;            // Chamada de acionamento da bomba
    delay(500)          ;
  }
  
  if(porcen>regraR){
    lcd.clear()         ;
    lcd.setCursor(0,0)  ;
    lcd.print("Bomba: ");
    bomb = "OFF"        ;             // Status da bomba
    lcd.setCursor(6,0)  ;
    lcd.print(bomb)     ;
    ACbomb = 0          ;             // Normalização da bomba
    delay(500)          ;
  }
  
  lcd.setCursor(0,1)    ;
  lcd.print(porcen)     ;
  lcd.setCursor(3,1)    ;
  lcd.print("% Umidade");
  delay(500)            ;

  if(ACbomb==1){                      // Decisão de acionamento
    digitalWrite(relay,HIGH)   ;      // Liga bomba
    delay(trega)               ;      // Tempod e operação
    digitalWrite(relay,LOW)    ;      // Desliga bomba
    delay(tespera)             ;      // Tempo de espera
    Recontador = Recontador + 1;      // Contador de regas
  }
    

}
