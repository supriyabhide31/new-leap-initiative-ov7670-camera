#include<Wire.h>
#include<SPI.h>
#include<SD.h>

#define WRST  0xa      // Output Write Pointer Reset
#define RRST  0x8      // Output Read Pointer Reset
#define WR    0x9      // Output Write Enable
#define VSYNC 0x3      // Input Vertical Sync marking frame capture
#define RCLK  0x1      // Output FIFO buffer output clock
// set OE to low gnd

// FIFO Ram input pins
#define DO7   0x7     
#define DO6   0x6   
#define DO5   0x5   
#define DO4   0x4   
#define DO3   0x2  
#define DO2   0xd  
#define DO1   0xc
#define DO0   0xb

//Photo dimensions
#define PHOTO_HEIGHT 10
#define PHOTO_WIDTH  10

//EO Definitions

/*_______________________________________________________________________________________________________________________________________
_______________________________________________________________________________________________________________________________________*/


int PhotoTakenCount=0;

void setupOV7670()
{
  setReg();
  setPins();
  }

void setReg()
{
#define CLKRC 0x11
#define CLKRC_VAL 80  // ALSO TRY 10H

#define COM7 0x12
#define COM7_VAL 0x04

#define COM3 0x0C
#define COM3_VAL 0x00

#define COM14 0x3E
#define COM14_VAL 0x00

#define SCALING_XSC 0x70
#define SCALING_XSC_VAL 4AH

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

  }

void setPins()
{
   // Set mode for pins wither input or output
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

void PulseLowEnabledPin(int PinNumber, int DurationMicroSecs)
{
  // For Low Enabled Pins , 0 = on and 1 = off
  digitalWrite(PinNumber, LOW);            // Sets the pin on
  delayMicroseconds(DurationMicroSecs);    // Pauses for DurationMicroSecs microseconds      
  
  digitalWrite(PinNumber, HIGH);            // Sets the pin off
  delayMicroseconds(DurationMicroSecs);     // Pauses for DurationMicroSecs microseconds  
}

void CaptureOV7670()
{
   unsigned long DurationStart = 0;
   unsigned long DurationStop = 0;
   unsigned long TimeForCaptureStart = 0;
   unsigned long TimeForCaptureEnd = 0;
   unsigned long ElapsedTime = 0;
   
   //Capture one frame into FIFO memory
   
   // 0. Initialization. 
   Serial.println();
   Serial.println(F("Capturing of Photo ..."));
   TimeForCaptureStart = millis();
 
   // 1. Wait for VSync to pulse to indicate the start of the image
   DurationStart = pulseIn(VSYNC, LOW);
 
   // 2. Reset Write Pointer to 0. Which is the beginning of frame
   PulseLowEnabledPin(WRST, 6); // 3 microseconds + 3 microseconds for error factor on Arduino

   // 3. Set FIFO Write Enable to active (high) so that image can be written to ram
   digitalWrite(WR, HIGH);
  
   // 4. Wait for VSync to pulse again to indicate the end of the frame capture
   DurationStop = pulseIn(VSYNC, LOW);
  
   // 5. Set FIFO Write Enable to nonactive (low) so that no more images can be written to the ram
   digitalWrite(WR, LOW);
     
   // 6. Print out Stats
   TimeForCaptureEnd = millis();
   ElapsedTime = TimeForCaptureEnd - TimeForCaptureStart;
   
   Serial.print(F("Time for Frame Capture (milliseconds) = "));
   Serial.println(ElapsedTime);
   
   Serial.print(F("VSync beginning duration (microseconds) = "));
   Serial.println(DurationStart);
   
   Serial.print(F("VSync end duration (microseconds) = "));
   Serial.println(DurationStop);
   
   // 7. WAIT so that new data can appear on output pins Read new data.
   delay(2); 
   PhotoTakenCount++;
   CreatePhotoFilename();
  }

void CreateFile()
{
  File sdFile;
  
  String Fname = "";
  String Ext = ".raw"; 
  // Create Fname from
  // Resolution + PhotoNumber + Extension
  Fname = RGB + PhotoTakenCount + Ext;
  
  sdFile=SD.open(Fname.c_str(),FILE_WRITE);
  if (!sdFile)
   {
     Serial.println(F("\nCritical ERROR ... Can not open Photo Info File for output ... "));
     return;
   } 
  
   // Write Info to file
   String Data = CreatePhotoInfo();
   sdFile.println(Data);
   
   // Close SD Card File
   sdFile.close();
  }

  void ReadTransmitCapturedFrame()
{
   // Read captured frame from FIFO memory and send each byte as it is read to the Android controller
   // via bluetooth.
    
   // Set Output Enable OE to active (low).
   // * Make sure to connect the OE output to ground.
   
   // Reset the FIFO read pointer by setting RRST to active (low) then bringing it back to high.
   // *Done from previous CaptureOV7670Frame() assuming WRST and RRST are tied together.
   
   // Read in the QQVGA image that is captured in the camera buffer by reading in the 38400 bytes that make up the 
   //   YUV photo 
   
   byte PixelData = 0;
   byte PinVal7 = 0;
   byte PinVal6 = 0;
   byte PinVal5 = 0;
   byte PinVal4 = 0;
   byte PinVal3 = 0;
   byte PinVal2 = 0;
   byte PinVal1 = 0;
   byte PinVal0 = 0;
   
   Serial.println(F("Starting Transmission of Photo To SDCard ..."));
   
   
   //////////////////// Code for SD Card /////////////////////////////////
   
   // Image file to write to
   File ImageOutputFile;
   
   // Create name of Photo To save based on certain parameters
   String Filename = CreatePhotoFilename();
   
   // Check if file already exists and remove it if it does.
   CheckRemoveFile(Filename);
  
   ImageOutputFile = SD.open(Filename.c_str(), FILE_WRITE);
   
   // Test if file actually open
   if (!ImageOutputFile)
   {
     Serial.println(F("\nCritical ERROR ... Can not open Image Ouput File for output ... "));
     return;
   } 
   ////////////////////////////////////////////////////////////////////////
   
   // Set Read Buffer Pointer to start of frame
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
       for (int bytenumber = 0; bytenumber < PHOTO_BYTES_PER_PIXEL; bytenumber++)
       {
         // Pulse the read clock RCLK to bring in new byte of data.
         // 7ns for RCLK High Pulse Width and Low Pulse Width .007 micro secs
         PulsePin(RCLK, 1); 
             
         // Convert Pin values to byte values for pins 0-7 of incoming pixel byte
         PinVal7 = ConvertPinValueToByteValue(digitalRead(DO7), 7);
         PinVal6 = ConvertPinValueToByteValue(digitalRead(DO6), 6);
         PinVal5 = ConvertPinValueToByteValue(digitalRead(DO5), 5);
         PinVal4 = ConvertPinValueToByteValue(digitalRead(DO4), 4);
         PinVal3 = ConvertPinValueToByteValue(digitalRead(DO3), 3);
         PinVal2 = ConvertPinValueToByteValue(digitalRead(DO2), 2);
         PinVal1 = ConvertPinValueToByteValue(digitalRead(DO1), 1);
         PinVal0 = ConvertPinValueToByteValue(digitalRead(DO0), 0);
     
         // Combine individual data from each pin into composite data in the form of a single byte
         PixelData = PinVal7 | PinVal6 | PinVal5 | PinVal4 | PinVal3 | PinVal2 | PinVal1 | PinVal0;
        
         /////////////////////////////  SD Card ////////////////////////////////
         ByteCounter = ByteCounter + ImageOutputFile.write(PixelData);       
         ///////////////////////////////////////////////////////////////////////
       }
     }
   }
   
   // Close SD Card File
   ImageOutputFile.close();
   
   Serial.print(F("Total Bytes Saved to SDCard = "));
   Serial.println(ByteCounter);
   
   // Write Photo's Info File to SDCard.
   Serial.println(F("Writing Photo's Info file (.txt file) to SD Card ..."));}

void setup() {
  setupOV7670();
  // put your setup code here, to run once:
}

void loop() {
  CaptureOV7670();
  // put your main code here, to run repeatedly:
}
