#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Password.h>
#include <Keypad.h>  
#include <EasyButton.h>

const byte ARMAR = 0x40;
const byte DESARMAR = 0x50;
const byte VERIFICAR = 0x60;
const byte DISPARAR = 0x70;

const byte ARMADO = 0x61;
const byte DESARMADO = 0x62;
const byte DISPARANDO = 0x63;

#define pinBuzzer 13
#define pinDisparar 8

EasyButton btDisparar(pinDisparar, LOW, true);

LiquidCrystal_I2C lcd(0x27, 16, 2);

Password password = Password( "1234" ); //Senha  

//Teclado
const byte ROWS = 4; //linhas  
const byte COLS = 4; // colunas  
// Configuraçao do teclado  
char keys[ROWS][COLS] = {  
 { '1','2','3','A'},  
 { '4','5','6','B'},  
 { '7','8','9','C'},  
 { '*','0','#','D' }  
};  
   
byte rowPins[ROWS] = {30, 31,32, 33 };// Conecta os botoes de linhas do teclado com esses pinos do arduino.
byte colPins[COLS] = {34,35,36,37 }; // Conecta os botoes de coluna com esses pinos do arduino.  
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );  

int tentativas=0;
bool armado = false;
bool disparado = false;
String pass = "";

void setup() {
  // put your setup code here, to run once:
  btDisparar.setButtonHandle(disparar);
  Serial.begin(9600);
  Serial1.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0); //Linha 1 co lcd 
  lcd.print("Aperte # e digite");  
  lcd.setCursor(0,1); //linha 2 do lcd 
  lcd.print(" a senha");
  keypad.addEventListener(keypadEvent); //adiciona um evento com o Keypad   
}

void loop() {
  // put your main code here, to run repeatedly:
  keypad.getKey();
  if (armado == true)
  {
    btDisparar.work();
  }
  if (Serial1.available())
  {
    byte b = Serial1.read();
    switch(b)
    {
      case ARMAR: 
        armar();
        break;
      case DESARMAR:
        desarmar();
        break;
      case DISPARAR:
        disparar();
        break;
      case VERIFICAR:
        if (armado)
          Serial1.write(ARMADO);
        else
          Serial1.write(DESARMADO);
        if (disparado)
          Serial1.write(DISPARANDO);
    }
  }
}

void disparar()
{
  byte s[] = {0x55};
  tone(pinBuzzer, 1500);
  lcd.clear(); //Limpar lcd 
  lcd.setCursor(0,1); //linha 2 do lcd
  lcd.print("Alarme Disparando");
  Serial1.write(DISPARANDO);
}

void armar()
{
  lcd.clear(); //Limpar lcd 
  lcd.setCursor(0,1); //linha 2 do lcd
  lcd.print("Alarme Ligado");
  armado = true;
  Serial1.write(ARMADO);
}

void desarmar()
{
  lcd.clear(); //Limpar lcd 
  lcd.setCursor(0,1); //linha 2 do lcd
  lcd.print("Alarme Desligado"); 
  noTone(pinBuzzer);
  armado = false;
  tentativas = 0;
  Serial1.write(DESARMADO);
}

void alternar()
{
  if (armado)
    desarmar();
  else
    armar();
}

void keypadEvent(KeypadEvent chave){  
  switch (keypad.getState()){
    case PRESSED: //Se prescionar algum Botao
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("Senha"); 
      lcd.setCursor(0,1);
      switch (chave){
        case 'D':  //caso botao D
          verificarPassword(); //Tenta travar/destravar
          break;  
        case 'C':   //caso botao C
          password.reset(); //apagar senha  
          pass = "";
          break;  
        default:   
          password.append(chave); //acrescentar um caractere a senha atualmente digitada 
          pass += chave;
          lcd.print(pass); 
      }  
    }  
 }  
 void verificarPassword(){ 
  if (password.evaluate()){ //Se a senha digitada é igual a senha programada 
   alternar();  
  }  
  else{  
   lcd.clear();  //Limpar lcd
   lcd.setCursor(0,1);//Linha 1 do lcd  
   lcd.print("Senha errada"); 
   tentativas++;
   if (tentativas >= 3)
    disparar();
   password.reset(); //apagar senha  
   pass = "";
  }  
 } 
