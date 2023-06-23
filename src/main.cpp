#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h> //para o modulo SD Card
#include <SD.h>
#include <Wire.h> 
#include <RTClib.h>

RTC_DS3231 rtc;
Adafruit_SSD1306 display = Adafruit_SSD1306(); //objeto do tipo Adafruit_SSD1306

char t[32];
#define DHTTYPE DHT11
#define DHTPIN 22 
#define ledAmarelo 24
#define ledVerde 26

int pinoCS = 53; // Define o pino do chip select do módulo SD
File myFile; // Cria um arquivo para armazenar os dados
String file_n = " ";
String c_str = "";
char dados[] = "MEDIDAS/00.csv";
int c = 0; //variavel do contador

//char carac1 = 't';
// char carac2 = 'h';
float temperatura = 0;
float humidade = 0;
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
int registro = 1;

 void setup() {

  dht.begin();
  Serial.begin(9600); //Inicializa a serial
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  display.setCursor(10,20);
      display.print("teste");
      display.display(); 
      delay(1500);
      display.clearDisplay();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVerde, LOW);
  

   if (!rtc.begin()){ // <== (Utilizei a própria função de inicialização no if de verificação, do mesmo modo do SD card)
    Serial.println("Rtc não está executando");
 }else{
    Serial.println("Rtc em execução!");
   }
  
   //Modulo ligado
   Serial.println("Inicializando cartão SD...");
   if (!SD.begin(pinoCS)) {
    Serial.println("Inicialização do módulo SD falhou");
   return;
  }
   Serial.println("Módulo SD inicializado");
    
   if(SD.exists("contador.cfg") == 1){
    Serial.println("Padrão de arquivo detectado"); //debug    
    }
  else{
   Serial.println("Nova inicialização - criando arquivos padrão"); //debug
    //myFile = SD.open("temp.txt", FILE_WRITE);
    //myFile.close();

    myFile = SD.open("contador.cfg", FILE_WRITE);
    myFile.println("1"); 
    myFile.close();

    SD.mkdir("MEDIDAS");
    }

    myFile = SD.open("contador.cfg");
     c_str = "";
     while (myFile.available()){c_str += (char)myFile.read();}
     myFile.close(); 
     c = c_str.toInt();  

     int nd = log10(c)+1;    
     file_n += "MEDIDAS/";
    if(nd > 5){
    Serial.print("O arquivo do contador atingiu o limite");//debug
    file_n += "extra.csv"; 
  }
 else{
    for(int i = 0; i < 5-nd; i++){
      file_n += '0';           
      }
     file_n += c;
     file_n += ".csv";
  }

    Serial.println(file_n);//debug
    Serial.print("arquivo de medida atual: ");//debug
    Serial.println(file_n);//debug

    SD.remove(dados);

     myFile = SD.open(dados, FILE_WRITE);
     myFile.print("Dia da semana,Data,hora,Temperatura,Umidade,Leitura\n");// Escreve os cabeçalhos das colunas
     myFile.close();

    SD.remove("contador.cfg");
     myFile = SD.open("contador.cfg", FILE_WRITE);
     myFile.println(c + 1); 
     myFile.close();
}

  void escrever(String nome, float valor,char carac){
    String stringValor = String(valor);
  
    //int TamanhoNome = nome.length();
    
     if(carac == 't'){
      nome = "Temp:";
      // TamanhoNome = 5
      int posicao = 25;
      display.setCursor(posicao,15);
      display.print(nome);
      display.print(valor); 
      display.display();
  } else if(carac == 'h'){
      nome = "Humidade:";  
      // TamanhoNome = 9
      int posicao = 25;
      display.setCursor(posicao,28);
      display.print(nome);
      display.print(valor);
      display.print("%");
      display.display();
      display.clearDisplay();
    }
  }
void temperature_file_log(uint8_t diaSemana, int dia, int mes, int ano , int hora, int minuto, int segundo, float t_C,float h , int registro,char *filename){
 
  myFile = SD.open(filename, FILE_WRITE);

   if (myFile) {    
    DateTime now = rtc.now();
    diaSemana = now.dayOfTheWeek();

    dia = now.day();
    mes = now.month() ;
    ano = now.year();
    hora = now.hour() ;
    minuto = now.minute() ;
    segundo = now.second() ;

    display.setCursor(20,20);
    display.print("Arquivo criado");
    display.display();
    delay(2000);
    display.clearDisplay();
    
   // Escreve a data, hora e dados em colunas
   switch(diaSemana){
      case 1:
        myFile.print("Segunda");
        break;  
      case 2:
        myFile.print("Terça");
        break;  
      case 3:
        myFile.print("Quarta");
        break;
      case 4:
        myFile.print("Quinta");
        break;
      case 5:
        myFile.print("Sexta");
        break;
      case 6:
        myFile.print("Sabado");
        break;
      case 7:
        myFile.print("Domingo");
        break;
      default:
        break;
    }                      
    myFile.print(",");

    myFile.print(dia);
    myFile.print("/");
    myFile.print(mes);
    myFile.print("/");
    myFile.print(ano);
    myFile.print(",");

    myFile.print(hora);
    myFile.print(":");
    myFile.print(minuto);
    myFile.print(":");
    myFile.print(segundo);
    myFile.print(",");

    myFile.print(t_C);
    myFile.print(","); 

    myFile.print(h);
    myFile.print(",");

    myFile.println(registro);

    myFile.close(); // Fecha o arquivo

    }
  else {
    // se o arquivo não abrir, imprima um erro:
    display.setCursor(10,20);
    display.print("Arquivo nao foi criado");
     display.display();
     delay(2000);
     display.clearDisplay();
    Serial.print("Erro na abertura do arquivo: "); //debug
    Serial.println(filename);
    }
  } 
   //void testarLCD(String nome, int cursor){
   //lcd.setCursor(0,cursor);
   //lcd.print(nome);
   //delay(5000);
   //lcd.clear();
   //}

 void loop() {
  delay(delayMS);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Erro na leitura da Temperatura!");
  }
  else {
    char carac = 't';
    String nome = "Temp:";
    temperatura = event.temperature;
    escrever(nome,temperatura,carac);
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Erro na leitura da Umidade!");
  }
  else {
    char carac = 'h';
    String nome = "Humidade:";
    humidade = event.relative_humidity;
    escrever(nome,humidade,carac);
    
  }

  DateTime now = rtc.now();
  uint8_t diaSemana = now.dayOfTheWeek(); 
  sprintf(t, " %02d/%02d/%02d %02d:%02d:%02d ",now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());  
  Serial.print(F("Data/hora: "));
  Serial.println(t);
  Serial.print("Dia: ");
  Serial.println(diaSemana);
    

  delay(1000);  

  int limiar = 01;
  bool condicao1 = now.hour() == 15 && now.minute() >=limiar && now.minute() <= limiar+1;
  bool condicao2 = now.hour() == 15 && now.minute() >=limiar+2;

  if(condicao1) {
     digitalWrite(ledAmarelo, HIGH);
     digitalWrite(ledVerde, LOW);
     Serial.println("Escrevendo!"); //debug
     temperature_file_log(now.dayOfTheWeek(), now.day() ,now.month(),now.year(), now.hour(), now.minute(),now.second() ,temperatura,humidade , registro, dados);
     registro++;
  }
  else{
    if(condicao2){
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVerde, HIGH);
      display.setCursor(8,20);
      display.print("Terminei de escrever");
      display.display();
      delay(2000);
      display.clearDisplay();
      Serial.println("Terminei de escrever;)"); //debug
      myFile.close();
    }
  }

  Serial.print(temperatura); // debug
  Serial.print(" "); // debug
  Serial.println(humidade); // debug

  Serial.println("--------------------------------------");
}