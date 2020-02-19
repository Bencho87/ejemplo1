/* I2C LCD 16x2 Arduino Tutorial
 * More info http://www.ardumotive.com/i2clcd(en).html
 * Dev: Michalis Vasilakis Date: 19/11/2016 Ver. 1.0 */

//Libraries 
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
//////////////////////////////////////////////
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <EEPROM.h>
#include <swRTC.h>

#include <avr/wdt.h>

swRTC rtc; //create a new istance of the lib

ThreeWire myWire(11,13,10); //IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
//////////////////////////////////////////////

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address, if it's not working try 0x27.
int falla=0;
int addr = 0;
byte addrl = 0;
byte addrh = 0;
byte neventos = 0;
byte nevents;
byte temnevents=0;
int intTemp = 0;
int intTemp1 = 0;
int addrTemp = 0;
int hp, mp, sp;
int index = 0;
//////////////////////////

int lastfail = 0;
int lastdir;
int lastevent;
int keypadevento = 0;
int paroevento = 0;
String Cadena;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// zona de respaldo 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const byte FILAS=4; //cuatro filas
const byte COLS=3; //cuatro columnas
//definimos el keymap
char keys[FILAS][COLS]={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
//conectar los pines FILA0,FILA1,FILA2,FILA3 a estos pins de arduino MEGA
byte rowPins[FILAS]={39,41,43,45};
//conectar los pines COL0, COL1, COL2, COL3 a estos pins de arduino
byte colPins[COLS]={47,49,51};
//crear el keypad
Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins, FILAS, COLS);

#define ledpinVerde 31
#define ledpinRojo 33
int n = 0;
int nx = 0;
int ny = 0;

int indux = 0;


void setup(){
   pinMode(40, OUTPUT);
   Serial3.begin(9600);
   //EEPROM.write(0,0);//en los dos primeros bytes se guarda la dirección
   //EEPROM.write(1,0);//
   //EEPROM.write(2,0);// numero de eventos
  
   lcd.begin(16,2);   // iInit the LCD for 16 chars 2 lines
   lcd.backlight();   // Turn on the backligt (try lcd.noBaklight() to turn it off)

   keypad.addEventListener(keypadEvent); //añadimos un evento de escucha para este keypad

   //////////////////////////////////////////////
   Rtc.Begin();
   if(!Rtc.GetIsRunning())
   Rtc.SetIsRunning(true);
   //////////////////////////////////////////////
    delay(500);
        //// Revisar la ultima dirección almacenada y el último dato 
      nevents = EEPROM.read(2); 
      addrh = EEPROM.read(0);
      addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
      intTemp=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
      intTemp=  intTemp + addrl; //Sumamos la parte LOW para completar nuestro "int"
      lastdir = intTemp;   
      lastevent = EEPROM.read(lastdir-6); //leer el motivo del último evento
 
      
      if(lastdir != 0) //SIGNIFICA QUE HAY REGISTRADO ALGÚN EVENTO
      {
        if(lastevent != 0) //significa que el último evento no fue una liberación
        {
          digitalWrite(40,HIGH); //REACTIVAR LA ALARMA POR QUE HAY ALGUN EVENTO ALMACENADO
          switch(lastevent)
          {
            case 1:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Falta material    ");
            break;
            case 2:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Patron/Abrasivo   ");
            break;
            case 3:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Falla elect/mec   ");
            break;
            case 4:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Falta aire/agua   ");
            break;
            case 5:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Destape segmentos ");
            break;
            case 6:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Nivelacion platos ");
            break;
            case 7:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Calibrado medidas ");
            break;
            case 8:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Falta operador    ");
            break;
            case 9:
            lcd.setCursor(0,0);
            lcd.print("    Paro por      ");
            lcd.setCursor(0,1);
            lcd.print("Otros             ");
            break;
            default:
            lcd.setCursor(0,0);
            lcd.print(lastevent);
            lcd.print("x");
            lcd.print(lastdir);
            break;
            }
          delay(2000);
          }
          else //significa que no hay ningun paro anterior o hay una liberación
        {
          lcd.setCursor(0,0); //First line
          lcd.print("    ANTWORT");
          lcd.setCursor(0,1); //Second line
          lcd.print(" * INGENIERIA *");
          delay(3000);
          lcd.setCursor(0,0); //First line
          lcd.print("    REGISTRO ");
          lcd.setCursor(0,1); //Second line
          lcd.print("   * PAROS *      ");
          delay(500);
          lcd.noBacklight();
          }
        
        }
///////////////////////////Mensaje de bienvenida
        else //significa que no hay ningun paro anterior o hay una liberación
        {
          lcd.setCursor(0,0); //First line
          lcd.print("    ANTWORT");
          lcd.setCursor(0,1); //Second line
          lcd.print(" * INGENIERIA *");
          delay(3000);
          lcd.setCursor(0,0); //First line
          lcd.print("    REGISTRO ");
          lcd.setCursor(0,1); //Second line
          lcd.print("   * PAROS *      ");
          delay(500);
          lcd.noBacklight();
          }

          //************************************************************//
          wdt_enable(WDTO_8S); //UN RESET DE 2 SEGUNDOS
          //************************************************************//
  }

  char bait;
  RtcDateTime now;
 
void loop(){

  if(indux == 1000 || keypadevento == 1){
///////////////////ZONA DE RESPALDO OTRA VEZ
 //lcd.begin(16,2);   // iInit the LCD for 16 chars 2 lines
 //lcd._Rs = 0;
 //lcd._Rs = 1;
 //lcd.clear();
 //lcd.backlight(); 
// delay(50);
///////////////////////////////////////////

  now = Rtc.GetDateTime();
//////////////////////////////////////////////////////////////////////////

  if(keypadevento == 1) //opochurraron el tecladow 
  {
   // limpiarlinea();
   // lcd.setCursor(0,0);
   // lcd.print("Codigo:");
   // lcd.print(n);
   // delay(10);
    switch(n)
    {
       case 0: 
       nx = 100;
       lcd.setCursor(0,0); //Second line
       lcd.print("Presiona # para ");
       lcd.setCursor(0,1); //Second line
       lcd.print("Reanudar proceso");
       paroevento = 0; //no es un evento de paro
       delay(100);
       break;
       
       case 1: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         lcd.print("Falta material  ");
         nx = 0;
         paroevento = 1;
         delay(100); 
       break;
       case 2: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Patron/Abrasivo  ");
         nx = 0;
         paroevento = 1;
       delay(100); 
       break;
       case 3: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Falla elect/mec  ");
         nx = 0;
         paroevento = 1;
         delay(100); break;
       case 4: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Falta aire/agua ");
         nx = 0;
         paroevento = 1;
         delay(100); break;
       case 5: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Destape segmentos ");
         nx = 0;     
         paroevento = 1;
         delay(100); break;
       case 6: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Nivelacion platos ");
         nx = 0; 
         paroevento = 1;
        delay(1); break;
       case 7: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Calibrado medidas ");
         nx = 0;
         paroevento = 1;
         delay(100); break;
       case 8: 
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Falta operador    ");
         nx = 0; 
         paroevento = 1;
         delay(100); break;
       case 9:
         lcd.setCursor(0,0); //Second line
         lcd.print("# para guardar   ");
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Otros            "); 
         nx = 0; 
         paroevento = 1;
         delay(100); break;
      case 10: //caso enter para almacenar los datos
      
      //leer numero de eventos y leer direcci´n
      //para determinar si ya almacenó algun evento
      //de paro
      nevents = EEPROM.read(2);
            
      addrh = EEPROM.read(0);
      addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
      

      intTemp=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
      intTemp=  intTemp + addrl; //Sumamos la parte LOW para completar nuestro "int"
      addr = intTemp;   

      if((addr == 0 || EEPROM.read(addr-6)==0) && paroevento == 0)
      {
         lcd.setCursor(0,0); //Second li
         lcd.print("No hay paro anterior");
         lcd.setCursor(0,1);
         lcd.print("registrado           ");
        }
      else{
        if(addr != 0 && ((EEPROM.read(addr-6)!=0) && paroevento != 0))
          {
            lcd.setCursor(0,0); //Second li
            lcd.print("Paro anterior   ");
            lcd.setCursor(0,1);
            lcd.print("no liberado     ");
            }
        else{
              neventos = EEPROM.read(2);
              neventos++;
              EEPROM.write(2,neventos); //incremento el numero de eventos y lo almaceno
              //addr = addrh<<8
     
              //recuperamos la dirección en la que se almacenó el último dato
              if(addr == 0) addr = addr+3;
              //if(paroevento == 0) {EEPROM.write(addr,0);} //liberación
              //else {EEPROM.write(addr,n);}//el motivo //todos estos son de 8 bits}
              EEPROM.write(addr,ny);
              EEPROM.write(addr+1,now.Month());//mes
              EEPROM.write(addr+2,now.Day());//dia
              EEPROM.write(addr+3,now.Hour());//hora
              EEPROM.write(addr+4,now.Minute());//minuto
              EEPROM.write(addr+5,now.Second());//segundo
              //almacenar la última dirección
              addrTemp = addr+6; //la dirección en la que se quedó
              addrh = addrTemp/256; 
              addrl = (addrTemp%256);


              EEPROM.write(0,addrh);//segundo
              EEPROM.write(1,addrl);//segundo 
              if(paroevento==1)
                { 
                  lcd.setCursor(0,0); //Second li
                  lcd.print("Evento almacenado    ");
                  paroevento = 0; //reiniciar la bandera
                  rtc.setTime(0,0,0); //set the time here
                  rtc.startRTC(); //start the RTC
                  digitalWrite(40, HIGH); //SE ACTIVA LA ALARMA
                  delay(1000);
                } 
              else
                {
                  lcd.setCursor(0,0); //Second li
                  lcd.print("   Paro           ");
                  lcd.setCursor(0,1); //Second line
                  lcd.print("   Liberado       ");
                  // rtc.stopRTC(); //stop the RTC
                  digitalWrite(40,LOW);      
                }
              }
             }
//             paroevento = 0; //liberamos el teclado de alguna manera bizarra se haya guardado el evento o no
             keypadevento = 0;
             n = 33;//revisar esto por que debe cambiar
        delay(2000); break;
        case 11://revisar esto por que debe cambiar 
       /*
        addrh = EEPROM.read(0);
        addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
        ////addr = addrh<<8
        intTemp1=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
        intTemp1=  intTemp1 + addrl; //Sumamos la parte LOW para completar nuestro "int"
        addr = intTemp1;   
        nevents = EEPROM.read(2);
        lcd.setCursor(0,1);
        lcd.print(addr);
        lcd.print("eventos= ");
        lcd.print(nevents-1);
        delay(3000);
       // n = 44;
       */
       ////addrh = EEPROM.read(0);
       ////addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
       ////intTemp=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
       ////intTemp=  intTemp + addrl; //Sumamos la parte LOW para completar nuestro "int"
       ////addr = intTemp;   
       ////if(EEPROM.read(addr-6)!=0)
       ////n = 33;
       ////keypadevento = 0;
        
     //   lcd.begin(16,2);
        delay(1); break;
        default: 
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Llamar servicio ");
         n = 400;
         nx = 0; 
        delay(1);
      }
    } //if keypadevent == 1 ? Significa que alguien pusho el teclado 




//////////////////////////////////////////////////////////////////////////    
  if (Serial3.available()) { 
        bait = Serial3.read();
          if(bait == 0x55)
            { 
              temnevents = EEPROM.read(2);
              Serial3.write(temnevents); 
              delay(5);
              for(index = 0; index <= temnevents-1; index++)
                {
                  addr = 3+6*index;
                  Serial3.write(EEPROM.read(addr));    //motivo
                  // Serial.print("   ");
                  Serial3.write(EEPROM.read(addr+1)); //mes     
                  // Serial.print("   ");
                  Serial3.write(EEPROM.read(addr+2)); //dia     
                  // Serial.print("   ");
                  Serial3.write(EEPROM.read(addr+3)); //hora     
                  // Serial.print("   ");
                  Serial3.write(EEPROM.read(addr+4)); //minuto      
                  // Serial.print("   ");
                  Serial3.write(EEPROM.read(addr+5)); //segundo
                delay(5);     
              } 
          lcd.setCursor(0,1);
          lcd.print("Datos descargados      ");
          delay(100);
        }
    if(bait == 0x3F)
        {
          EEPROM.write(0,0);//en los dos primeros bytes se guarda la dirección
          EEPROM.write(1,0);//
          EEPROM.write(2,0);// numero de eventos
          lcd.setCursor(0,1);
          lcd.print("Registro borrado      ");
          n = 205;
          delay(1000);
          
         }
        }
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
    if(nx == 100 && n == 33)
    {
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("  Control  de   ");
      lcd.setCursor(0,1);
      lcd.print("  Registros     ");
      nx = 0;
      n = 205;
      delay(1000);
      }
    else
      {
//        if(n == 205) //UNA VEZ LIBERADO IMPREME LA HORA NORMAL
//          {
            //para mostrar el reloj cuando no haya paro guardado 
                  addrh = EEPROM.read(0);
                  addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
                  intTemp=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
                  intTemp=  intTemp + addrl; //Sumamos la parte LOW para completar nuestro "int"
                  addr = intTemp;
            if((addr == 0 || EEPROM.read(addr-6)==0 || n == 205)&&keypadevento == 0)
            {
            lcd.backlight();
            lcd.setCursor(0,0);
            lcd.print("    ");
            lcd.print(now.Hour());
            lcd.print(":");
            lcd.print(now.Minute());
            lcd.print(":");
            lcd.print(now.Second());  
            lcd.print("    ");
          //  lcd.setCursor(0,1);
          //  lcd.print("                ");
            delay(10);  
          }
      }
   
   
   if(nx == 0&& n == 33) //significa que guardo una falla
    {
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("    ");
    lcd.print(rtc.getHours());
    lcd.print(":");
    lcd.print(rtc.getMinutes());
    lcd.print(":");
    lcd.print(rtc.getSeconds());  
    lcd.print("        ");
    //digitalWrite(40, HIGH); //SE ACTIVA LA ALARMA
    delay(10);
      }
    else
      {
       //detener tono durante 500ms  
       //digitalWrite(40, LOW);//ya no libero el paro aqui
      }
      indux = 0;
  } indux++;
  wdt_reset();
  keypad.getKey();
  delay(1);
}



//FUNCION PARA COMPROBAR SI SE PULSA EL KEYPAD
void keypadEvent(KeypadEvent eKey){
  lcd.backlight();
  switch(keypad.getState()){
    case PRESSED:
    keypadevento = 1;
   // limpiarlinea();
   // lcd.setCursor(0,0);
   // lcd.print("Codigo:");
   // lcd.print(eKey);
    delay(10);
    switch(eKey){
       case '0': 
       nx = 100;
       n = 0;
       ny = 0;
       // lcd.setCursor(0,1); //Second line
       // lcd.print("Reanudar proceso");
       // delay(2000);  
      delay(100); break;
       case '1': 
         //lcd.setCursor(0,1); //Second line
       //lcd.print("                ");
         //lcd.print("Falta material  ");
         n = 1;
         ny = 1;
         //nx = 0;
      delay(100); break;
       case '2': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Patron/Abrasivo  ");
         n = 2;
         ny = 2;
         //nx = 0;
      delay(100); break;
       case '3': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Falla elect/mec  ");
         n = 3;
         ny = 3;
         //nx = 0;
         
      delay(100); break;
       case '4': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Falta aire/agua ");
         n = 4;
         ny = 4;
         //nx = 0;
         
      delay(100); break;
       case '5': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Destape segmentos ");
         n = 5;
         ny = 5;
         //nx = 0;
        
      delay(100); break;
       case '6': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Nivelacion platos ");
         n = 6;
         ny = 6;
         //nx = 0; 
      delay(100); break;
       case '7': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Calibrado medidas ");
         n = 7;
         ny = 7;
         //nx = 0;  
      delay(100); break;
       case '8': 
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Falta operador    ");
         n = 8;
         ny = 8;
         //nx = 0; 
       
       delay(100); break;
       case '9':
         //lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         //lcd.print("Otros            "); 
         n = 9;
         ny = 9;
         //nx = 0; 
       
      delay(100); break;
      case '#': //caso enter para almacenar los datos
      /*
      //leer numero de eventos y leer direcci´n
      //para determinar si ya almacenó algun evento
      //de paro
      nevents = EEPROM.read(2);
            
      addrh = EEPROM.read(0);
      addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
      

      intTemp=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
      intTemp=  intTemp + addrl; //Sumamos la parte LOW para completar nuestro "int"
      addr = intTemp;   

      if((addr == 0 || EEPROM.read(addr-6)==0) && n == 0)
      {
         lcd.setCursor(0,0); //Second li
         lcd.print("No hay paro anterior");
         lcd.setCursor(0,1);
         lcd.print("registrado           ");
        }
      else{
        if((EEPROM.read(addr-6)!=0) && n != 0)
          {
            lcd.setCursor(0,0); //Second li
            lcd.print("Paro anterior   ");
            lcd.setCursor(0,1);
            lcd.print("no liberado     ");
            }
        else{
              neventos = EEPROM.read(2);
              neventos++;
              EEPROM.write(2,neventos); //incremento el numero de eventos y lo almaceno
              //addr = addrh<<8
     
              //recuperamos la dirección en la que se almacenó el último dato
              if(addr == 0) addr = addr+3;
              EEPROM.write(addr,n);//el motivo //todos estos son de 8 bits
              EEPROM.write(addr+1,now.Month());//mes
              EEPROM.write(addr+2,now.Day());//dia
              EEPROM.write(addr+3,now.Hour());//hora
              EEPROM.write(addr+4,now.Minute());//minuto
              EEPROM.write(addr+5,now.Second());//segundo
              //almacenar la última dirección
              addrTemp = addr+6; //la dirección en la que se quedó
              addrh = addrTemp/256; 
              addrl = (addrTemp%256);


              EEPROM.write(0,addrh);//segundo
              EEPROM.write(1,addrl);//segundo 
              if(n!=0)
                { 
                  rtc.setTime(0,0,0); //set the time here
                  rtc.startRTC(); //start the RTC
                } 
              else
                {
                  lcd.setCursor(0,0); //Second li
                  lcd.print("   Paro    ");
                  lcd.setCursor(0,1); //Second line
                  lcd.print("   Liberado       ");
                  // rtc.stopRTC(); //stop the RTC
                  digitalWrite(40,LOW);      
                }
             }
      }
      n = 33;//revisar esto por que debe cambiar

      */ 
      n = 10;
      delay(100); break;
      case '*'://revisar esto por que debe cambiar 
      /*
      addrh = EEPROM.read(0);
      addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
      ////addr = addrh<<8
      intTemp1=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
      intTemp1=  intTemp1 + addrl; //Sumamos la parte LOW para completar nuestro "int"
      addr = intTemp1;   
      nevents = EEPROM.read(2);
      lcd.setCursor(0,1);
      lcd.print("eventos= ");
      lcd.print(nevents-1);
      delay(3000);
      n = 44;*/
      // n = 11;
      lcd.begin(16,2); //de todas formas se resetea la pantalla
            
      addrh = EEPROM.read(0);
      addrl = EEPROM.read(1);//leer la dirección de las primeras dos direcciones
      intTemp=  addrh<<8;    //Tomamos la parte HIGH y la desplazamos 8 dígitos a la izaquierda
      intTemp=  intTemp + addrl; //Sumamos la parte LOW para completar nuestro "int"
      addr = intTemp;
      if(addr != 0){ //si no está en la primera dirección, hay almacenados datos   
      if(EEPROM.read(addr-6)!=0){ // si el ultimo evento es un  paro
      switch(EEPROM.read(addr-6))
      {
          case 1:
            lcd.setCursor(0,1);
            lcd.print("Falta material    ");
            break;
            case 2:
            lcd.setCursor(0,1);
            lcd.print("Patron/Abrasivo   ");
            break;
            case 3:
            lcd.setCursor(0,1);
            lcd.print("Falla elect/mec   ");
            break;
            case 4:
            lcd.setCursor(0,1);
            lcd.print("Falta aire/agua   ");
            break;
            case 5:
            lcd.setCursor(0,1);
            lcd.print("Destape segmentos ");
            break;
            case 6:
            lcd.setCursor(0,1);
            lcd.print("Nivelacion platos ");
            break;
            case 7:
            lcd.setCursor(0,1);
            lcd.print("Calibrado medidas ");
            break;
            case 8:
            lcd.setCursor(0,1);
            lcd.print("Falta operador    ");
            break;
            case 9:
            lcd.setCursor(0,1);
            lcd.print("Otros             ");
            break;
            default:
            lcd.setCursor(0,1);
            lcd.print(lastevent);
            lcd.print("x");
            lcd.print(lastdir);
            break;
        }
      }
      else { lcd.setCursor(0,1);
            lcd.print("Proceso liberado    ");}
      }

      keypadevento = 0;
      delay(100); break;
      default: 
         lcd.setCursor(0,1); //Second line
         //lcd.print("                ");
         lcd.print("Llamar servicio ");
         n = 400;
         nx = 0;
         
      delay(100);
    }
  }
}




//FUNCION PARA LIMPIAR LA PRIMERA LINEA DEL LCD
void limpiarlinea(){
  lcd.setCursor(0,0);
  for(int i=0;i<16;i++){
    lcd.print(" ");
  }
}

//FUNCION PARA LIMPIAR TODO EL LCD
void limpiartodo(){
  lcd.setCursor(0,0);
  for(int i=0;i<16;i++){
    lcd.print(" ");}
  lcd.setCursor(0,1);
  for(int i=0;i<16;i++){
    lcd.print(" ");}
  
}
