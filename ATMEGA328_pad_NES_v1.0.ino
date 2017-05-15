//2017.05.03 Lou's version of the mini NES joypad
//2017.05.14 Now it works!!

#include <Wire.h>

#define NESPAD_ADDRESS 0x52

#ifndef CPU_FREQ
#define CPU_FREQ 8000000L
#endif

#ifndef TWI_FREQ
#define TWI_FREQ 400000L
#endif


uint8_t readB=0xFF;
uint8_t readC=0xFF;
uint8_t readD=0xFF;
uint8_t nes_dataA[]={0x82,0x7E,0x7C,0x82,0x18,0x32,0xFF,0xFF};
uint8_t nes_dataB[2];
uint8_t nes_add[]={0x00,0x00,0xA4,0x20,0x03,0x01,0x00,0x00};
byte LastMasterCommand = 0;



void setup() {
  //Serial.begin(57600);
  //Freq I2C --> 400kHz
  //TWBR = ((CPU_FREQ / TWI_FREQ) - 16) / 2;
  DDRB=0x00;
  DDRC=0x00;
  DDRD=0x00;
  PORTB=0xFF;
  PORTC=0xFF;
  PORTD=0xFF;
  /*
  PORTB=0xC6;
  PORTC=0x0F;
  PORTD=0x1F;
  */
  Wire.begin(NESPAD_ADDRESS);                // join i2c bus with address 0x52
  TWBR = ((CPU_FREQ / TWI_FREQ) - 16) / 2;
  delay(10);
  Wire.onRequest(slavesRespond); // register event
  Wire.onReceive(receiveEvent); // register event
  //Serial.println("\nREADY!\n");
 }

  

void loop() {
  // Push-buttons!!!
  
  readB=PINB;
  readC=PINC;
  readD=PIND;
  //Home button

  if(((~readB>>5)&0x01)&((~readB>>4)&0x01)){
    nes_dataB[0]=0xF7;
    nes_dataB[1]=0xFF;
  }
  else{
  nes_dataB[0] = ((readB & 0x01)<<7) | ((readD & 0x40)) | ((readB & 0x20)>>1) | ((readB & 0x10)>>2) | 0x2B;
  nes_dataB[1] = ((readB & 0x08)<<3) | ((readC & 0x01)<<4) | ((readD & 0x80)>>6)| ((readD & 0x20)>>5) | 0xAC;
 } 

}





void slavesRespond(){
 
  byte buffer[21]={0};
  
  switch(LastMasterCommand){
    
    case 0x00:   // Return 8 bytes command
        for(int i=0;i<6;i++){
          buffer[i]=nes_dataA[i];
        }
        buffer[6]=nes_dataB[0];
        buffer[7]=nes_dataB[1];
        //Serial.println(buffer[6], HEX);
        //Serial.println(buffer[7], HEX);
        Wire.write(buffer, 21);
    break;
    
    case 0xFA:   // Return WII code
        for(int i=0;i<8;i++){
          buffer[i]=nes_add[i];
        }
        Wire.write(buffer, 8);
    break;

    }
  LastMasterCommand = 0;          // null last Master's command
}


void receiveEvent(int howMany) {
 
  while (Wire.available()) { // loop through all 
    LastMasterCommand = Wire.read(); // receive byte 
    //Serial.println(LastMasterCommand, HEX);
  }
}


