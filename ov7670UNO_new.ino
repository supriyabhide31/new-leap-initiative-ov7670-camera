#include<Wire.h>
#include<SPI.h>
#include<SD.h>


Sd2Card card;
SdVolume volume;
SdFile root;


const int CS = 10;
String filename;
#define WRST  0x1      // Output Write Pointer Reset
#define RRST  0x8      // Output Read Pointer Reset
#define WR    0x9      // Output Write Enable
#define VSYNC 0x3      // Input Vertical Sync marking frame capture
#define RCLK  0x2      // Output FIFO buffer output clock
// set OE to low gnd

// FIFO Ram input pins
#define DO7   0x7     
#define DO6   0x6   
#define DO5   0x5   
#define DO4   0x4   
#define DO3   0x17  
#define DO2   0x16 
#define DO1   0x15
#define DO0   0x14

//Photo dimensions
#define PHOTO_HEIGHT 10
#define PHOTO_WIDTH  10

//SetRegisters
#define CLKRC 0x11
#define CLKRC_VAL 80  // ALSO TRY 10H

#define COM7 0x12
#define COM7_VAL 0x04

#define COM3 0x0C
#define COM3_VAL 0x00

#define COM14 0x3E
#define COM14_VAL 0x00

#define SCALING_XSC 0x70
#define SCALING_XSC_VAL 0x4A

#define SCALING_YSC 0x71
#define SCALING_YSC_VAL 0x35

#define SCALING_DCWCTR 0x72
#define SCALING_DCWCTR_VAL 0x11

#define SCALING_PCLK_DIV 0x73
#define SCALING_PCLK_DIV_VAL 0x00

#define TSLB 0x3A
#define TSLB_VAL 0x0C

#define COM13 0x3D
#define COM13_VAL 0x99

#define COM17 0x42
#define COM17_VAL 0x00

#define COM4 0x0D
#define COM4_VAL 0x40

#define COM15 0x40
#define COM15_VAL 0xF0

#define COM11 0x3B
#define COM11_DAY 0x00
#define COM11_30FPS 0x0A
#define COM11_NIGHT 0xF2

#define MTXS 0x58
#define MTXS_VAL 0x1E

#define COM8 0x13
#define COM8_VAL 0x8F

#define COM9 0x14
#define COM9_VAL 0x4A

#define COM16 0x41
#define COM16_VAL 0x10

#define REG76 0x76
#define REG76_VAL 0x01             //E1H(CHANGE IF REQ)

#define DNSTH 0x4C
#define DNSTH_VAL 0x00

#define DBLV 0x6B
#define DBLV_VAL 0x3A

#define EXHCH 0x2A
#define EXHCH_VAL 0x00

#define EXHCL 0x2B
#define EXHCL_VAL 0x00

#define DM_LNL 0x92
#define DM_LNL_VAL 0x00

#define DM_LNH 0x93
#define DM_LNH_VAL 0x00

#define SATCR 0xC9
#define SATCR_VAL 0xC0

#define OV7670_I2C_ADDRESS                 0x21
#define I2C_ERROR_WRITING_START_ADDRESS    0x11
#define I2C_ERROR_WRITING_DATA             0x22
//EO Definitions

/*_______________________________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________________________*/

void SetPins()
{
  // Set mode for pins with either input or output
  pinMode(WRST , OUTPUT);
  pinMode(RRST , OUTPUT);
  pinMode(WR  , OUTPUT);
  pinMode(VSYNC, INPUT);
  pinMode(RCLK , OUTPUT);
  
  // FIFO Ram output pins
  pinMode(DO7 , INPUT);
  pinMode(DO6 , INPUT);
  pinMode(DO5 , INPUT);
  pinMode(DO4 , INPUT);
  pinMode(DO3 , INPUT);
  pinMode(DO2 , INPUT);
  pinMode(DO1 , INPUT);
  pinMode(DO0 , INPUT);
  
  // Delay 1 ms 
  delay(1);
}
int Count=0;

void writeReg()
{
    writeall(CLKRC, &CLKRC_VAL);
    
    
    writeall(COM7, &COM7_VAL);
    
    
    writeall(COM3,&COM3_VAL);
   
    
    writeall(COM14, &COM14_VAL);

    
    writeall(SCALING_XSC, &SCALING_XSC_VAL);
   
    
    writeall(SCALING_YSC, &SCALING_YSC_VAL);
    
    
    writeall(SCALING_DCWCTR, &SCALING_DCWCTR_VAL);
    
    
    writeall(SCALING_PCLK_DIV, &SCALING_PCLK_DIV_VAL);
    
    
    writeall(TSLB, &TSLB_VAL);
   
    
    writeall(COM13, &COM13_VAL);
    
    
    writeall(COM17, &COM17_VAL);
    
    
    writeall(COM4, &COM4_VAL);
    
    
    writeall(COM15, &COM15_VAL);
    
    
    writeall(COM11, &COM11_VAL);
   
    
    writeall(MTXS, &MTXS_VAL);
    
    
    writeall(COM8, &COM8_VAL);
    
    
    writeall(COM9, &COM9_VAL);
    
    
    writeall(COM16, &COM16_VAL);
   
    writeall(REG76, &REG76_VAL);
    
    
    writeall(DNSTH, &DNSTH_VAL);
    
    
    writeall(DBLV, &DBLV_VAL);
    
    
    writeall(EXHCH, & EXHCH_VAL);
    
    
    writeall(EXHCL,&EXHCL_VAL);
  
    
    writeall(DM_LNL, &DM_LNL_VAL);
    
    
    writeall(DM_LNH, &DM_LNH_VAL);
 
    
    writeall(SATCR, &SATCR_VAL);
    

    Serial.println("hi1");
    Serial.println(COM13);
    Serial.println(COM13_VAL);
    Serial.println("hi1");
    

  }
void writeall(int reg, int *ptr)
{
  Wire.beginabcTransmission(reg);
    Wire.write(ptr);
    Wire.endTransmission();
  }
void CaptureImage()
{
  pulseIn(VSYNC,LOW);

  //pulsing and Resetting write register
  digitalWrite(WRST,LOW);
  delay(3000);
  digitalWrite(WRST,HIGH);
  delay(3000);

   //write enable
   digitalWrite(WR,HIGH);
   
   pulseIn(VSYNC,LOW);
   delay(3000);
   digitalWrite(WR,LOW);
   delay(3000);

   
   
   Count++;
  }

String NFile()
{
  String ida="";          //fname
  String abc="PHOTO";
  String ext= ".jpg";
  ida= abc +Count + ext;
  return ida;
  }

void PulsePin(int PinNumber, int DurationMicroSecs)
{
  digitalWrite(PinNumber, HIGH);           // Sets the pin on
  delayMicroseconds(DurationMicroSecs);    // Pauses for DurationMicroSecs microseconds      
  
  digitalWrite(PinNumber, LOW);            // Sets the pin off
  delayMicroseconds(DurationMicroSecs);    // Pauses for DurationMicroSecs microseconds  
}

void CreateFile()
{

  //initilaise stuff
  byte PixelData = 0;
   byte PinVal7 = 0;
   byte PinVal6 = 0;
   byte PinVal5 = 0;
   byte PinVal4 = 0;
   byte PinVal3 = 0;
   byte PinVal2 = 0;
   byte PinVal1 = 0;
   byte PinVal0 = 0;
   // done initialising

   //create file
   File ImageFile;
   String FN = NFile();
   ImageFile=SD.open(FN, FILE_WRITE);

   digitalWrite(RRST, LOW);
   PulsePin(RCLK, 1); 
   PulsePin(RCLK, 1);
   PulsePin(RCLK, 1);
   digitalWrite(RRST, HIGH);

   unsigned long  ByteCounter = 0;
   for (int height = 0; height < PHOTO_HEIGHT; height++)
   {
     for (int width = 0; width < PHOTO_WIDTH; width++)
     {
       for (int bytenumber = 0; bytenumber < 2; bytenumber++)
       {
         // Pulse the read clock RCLK to bring in new byte of data.
         // 7ns for RCLK High Pulse Width and Low Pulse Width .007 micro secs
         PulsePin(RCLK, 1); 
             
         // Convert Pin values to byte values for pins 0-7 of incoming pixel byte
         PinVal7 =  digitalRead(DO7)<<7;
         PinVal6 =  digitalRead(DO6)<<6;
         PinVal5 =  digitalRead(DO5)<<5;
         PinVal4 =  digitalRead(DO4)<<4;
         PinVal3 =  digitalRead(DO3)<<3;
         PinVal2 =  digitalRead(DO2)<<2;
         PinVal1 =  digitalRead(DO1)<<1;
         PinVal0 =  digitalRead(DO0)<<0;
     
         // Combine individual data from each pin into composite data in the form of a single byte
         PixelData = PinVal7 | PinVal6 | PinVal5 | PinVal4 | PinVal3 | PinVal2 | PinVal1 | PinVal0;
         Serial.println(PinVal7);     
         delay(2000);
         /////////////////////////////  SD Card ////////////////////////////////
         ByteCounter = ByteCounter +  ImageFile.write(PixelData);  
         ///////////////////////////////////////////////////////////////////////
         
       }
     }
   }

   
   Serial.println("captured");
  
  }

void setup() {
  // Initialize Serial
     Serial.begin(9600); 
     Serial.println(F("Arduino SERIAL_MONITOR_CONTROLLED CAMERA ... Using ov7670 Camera"));
     Serial.println();
     
    
     // Setup the OV7670 Camera for use in taking still photos
     Wire.begin();
     Serial.println(F("----------------------------- Camera Registers ----------------------------"));
                              
    // Initialize SD Card
    Serial.println(F("\nInitializing SD card..."));
    pinMode(CS, OUTPUT);     
    digitalWrite(CS, HIGH);

    if (!SD.begin(CS))
    {     
      Serial.println(F("Initialization failed"));
      //return;
    } 
    else {
       Serial.println(F("Wiring is correct and a card is present ..."));
    }

    //Serial.println(COM13_VAL);
  
  //SetPins();
  writeReg();
  Serial.println("hi");
  Serial.println(COM13);
  Serial.println(COM13_VAL);
    Serial.println("hi");
    

  // put your setup code here, to run once:

}

void loop() {
  CreateFile();
  CaptureImage();
  // put your main code here, to run repeatedly:

}
