#include <Arduino.h>

#include "mbed.h"
#include "Rtc_Ds1307.h"
#include "TM1636.h" //ClockShield
#include "glibr.h" // Color
#include "Servo.h" // Motor
#include "MFRC522.h" // RFID
#include <string>
#include <iostream>
//Serial/////////////////////////////// 
//Serial esp(D8,D2);
Serial pc(USBTX,USBRX);
//////////////////////////////////////
//Sensor//////////////////////////////
AnalogIn DistanceI(A0);
AnalogIn DistanceII(A1);
DigitalIn Proximity(D2);
glibr Color(D14,D15);//SDA, SCL
MFRC522 RfChip(D11, D12, D13, D10, D9);//MOSI, MISO, SCK, SDA, RST 
/////////////////////////////////////
//ClockShield////////////////////////
Rtc_Ds1307 rtc(D3, D6); //I2C
DigitalIn  button_K3(A5);
TM1636 tm1636(D7,D4);  

uint16_t color, red, green, blue, ambient;
string tagRFID;
//int goodProduct, badProduct, allProduct;
//int Quality = (goodProduct/allProduct)*100;
//int allProduct, goodProduct, badProduct;
float Performance, Ability;
//int8_t temp[4];
int main(){
    int8_t temp[4];
    button_K3.mode(PullUp);
    tm1636.init();
    Rtc_Ds1307::Time_rtc tm = {};
    tm.date = 1 ;
    tm.mon = 1 ;
    tm.year = 12 ;
    tm.hour = 0;
    tm.min = 0;
    tm.sec = 0;
    rtc.setTime(tm, false, false);
    rtc.startClock();
    char DataCode[];
    esp.baud(115200);
    pc.baud(115200);
    RfChip.PCD_Init();
    Color.setMode(0,1);
    Color.enableLightSensor(true);
    Color.enablePower();
    rtc.startClock();
    while(1){
            pc.printf(".");
            rtc.getTime(tm);
            pc.printf("%02d:%02d:%02d\n",tm.hour,tm.min,tm.sec);
            temp[0] = tm.min / 10;
            temp[1] = tm.min % 10;
            temp[2] = tm.sec / 10;
            temp[3] = tm.sec % 10;                 
            tm1636.display(temp);
        int Status = pc.read()
        if(Status == 1){//Online Checking              
            if(RfChip.PICC_IsNewCardPresent()){ 
                pc.printf("Card Read\n");
                RfChip.PICC_ReadCardSerial();
                //pc.printf("%02d:%02d:%02d\n",tm.hour,tm.min,tm.sec);
                string str = "";
                pc.printf("\nTAG SERIAL : %X02", RfChip.uid.uidByte[0]); pc.printf(" %X02",RfChip.uid.uidByte[1]); pc.printf(" %X02",RfChip.uid.uidByte[2]); pc.printf(" %X02\n",RfChip.uid.uidByte[3]);
                uint16_t newstr = (RfChip.uid.uidByte[0] += RfChip.uid.uidByte[1]) |= 0x1010;
                pc.printf("%d\n",newstr);
                if(newstr == 145){
                    pc.printf("Order B\n");
                    tagRFID = "B";      
                    DataCode = "$B032";              
                }
                if(newstr == 213){
                    pc.printf("Order W\n");
                    tagRFID = "W";
                    DataCode = "$W402";
                }
            }        
            if(tagRFID == "B"){
                    float distance = DistanceI.read()*100;
                    //pc.printf("%.2f",distance);
                    if(distance <= 65){
                        pc.printf("Distance read\n");
                        allProduct += 1;
                        wait(12);
                        uint16_t red, green, blue;
                        Color.readRedLight(red);
                        Color.readGreenLight(green);
                        Color.readBlueLight(blue);
                        pc.printf("Red = %d\n",red);
                        pc.printf("Green = %d\n",green);
                        pc.printf("Blue = %d\n",blue);
                        int Colors = red + green + blue;
                            if(Colors >= 4 && Colors <= 6 && blue >= 1){
                                pc.printf("Blue\n");
                                wait(1);
                                if(Proximity == 1){
                                    goodProduct+=1;
                                }
                            }
                            if(Colors >= 16 && Colors <= 21){
                                badProduct += 1;
                                if(Proximity != 1){
                                    badProduct+=1;
                                }
                            }
                    }
                }
                if(tagRFID == "W"){
                    float distance = DistanceI.read()*100;
                    //pc.printf("%.2f",distance);
                    if(distance <= 65){
                        pc.printf("Distance read\n");
                        allProduct += 1;
                        wait(12);
                        uint16_t red, green, blue;
                        Color.readRedLight(red);
                        Color.readGreenLight(green);
                        Color.readBlueLight(blue);
                        pc.printf("Red = %d\n",red);
                        pc.printf("Green = %d\n",green);
                        pc.printf("Blue = %d\n",blue);
                        int Colors = red + green + blue;
                            if(Colors >= 20){
                                pc.printf("White\n");
                                wait(1);
                                Proximity.read();
                                //pc.printf("%d\n",Proximity.read());
                                if(Proximity == 1){
                                    goodProduct+=1;
                                }
                            }
                            if(Colors >= 4 && Colors <= 6 && blue >= 1){
                                wait(1);
                                if(Proximity != 1){
                                    badProduct += 1;
                                }
                            }
                        }                                               
                    }
                    int Quality = (goodProduct/allProduct)*100;
                    //int Performance = RealTIme/StartTime;
                    //int Ability = (goodProduct*RealTime)/StartTime;
                    DataCode = DataCode + string(Quality) + string(Performance) + string(Ability);
                    int len_Data;
                    for(len_Data = 0;len_Data < 14;len_Data++){
                        esp.putc(DataCode[i]);
                        wait(0.01);    
                    }
            }
            if(Status == 0){             
                if(RfChip.PICC_IsNewCardPresent()){ 
                    pc.printf("Card Read\n");
                    RfChip.PICC_ReadCardSerial();
                    //pc.printf("%02d:%02d:%02d\n",tm.hour,tm.min,tm.sec);
                    string str = "";
                    pc.printf("\nTAG SERIAL : %X02", RfChip.uid.uidByte[0]); pc.printf(" %X02",RfChip.uid.uidByte[1]); pc.printf(" %X02",RfChip.uid.uidByte[2]); pc.printf(" %X02\n",RfChip.uid.uidByte[3]);
                    uint16_t newstr = (RfChip.uid.uidByte[0] += RfChip.uid.uidByte[1]) |= 0x1010;
                    pc.printf("%d\n",newstr);
                    if(newstr == 145){
                        pc.printf("Order B\n");
                        tagRFID = "B";      
                        DataCode = "$B032";              
                    }
                    if(newstr == 213){
                        pc.printf("Order W\n");
                        tagRFID = "W";
                        DataCode = "$W402";
                    }
                }        
                if(tagRFID == "B"){
                        float distance = DistanceI.read()*100;
                        //pc.printf("%.2f",distance);
                        if(distance <= 65){
                            pc.printf("Distance read\n");
                            allProduct += 1;
                            wait(12);
                            uint16_t red, green, blue;
                            Color.readRedLight(red);
                            Color.readGreenLight(green);
                            Color.readBlueLight(blue);
                            pc.printf("Red = %d\n",red);
                            pc.printf("Green = %d\n",green);
                            pc.printf("Blue = %d\n",blue);
                            int Colors = red + green + blue;
                                if(Colors >= 4 && Colors <= 6 && blue >= 1){
                                    pc.printf("Blue\n");
                                    wait(1);
                                    if(Proximity == 1){
                                        goodProduct+=1;
                                    }
                                }
                                if(Colors >= 16 && Colors <= 21){
                                    badProduct += 1;
                                    if(Proximity != 1){
                                        badProduct+=1;
                                    }
                                }
                        }
                    }
                    if(tagRFID == "W"){
                        float distance = DistanceI.read()*100;
                        //pc.printf("%.2f",distance);
                        if(distance <= 65){
                            pc.printf("Distance read\n");
                            allProduct += 1;
                            wait(12);
                            uint16_t red, green, blue;
                            Color.readRedLight(red);
                            Color.readGreenLight(green);
                            Color.readBlueLight(blue);
                            pc.printf("Red = %d\n",red);
                            pc.printf("Green = %d\n",green);
                            pc.printf("Blue = %d\n",blue);
                            int Colors = red + green + blue;
                                if(Colors >= 20){
                                    pc.printf("White\n");
                                    wait(1);
                                    Proximity.read();
                                    //pc.printf("%d\n",Proximity.read());
                                    if(Proximity == 1){
                                        goodProduct+=1;
                                    }
                                }
                                if(Colors >= 4 && Colors <= 6 && blue >= 1){
                                    wait(1);
                                    if(Proximity != 1){
                                        badProduct += 1;
                                    }
                                }
                            }                                               
                        }
                        int Quality = (goodProduct/allProduct)*100;
                        //int Performance = RealTIme/StartTime;
                        //int Ability = (goodProduct*RealTime)/StartTime;
                        DataCode = DataCode + string(Quality) + string(Performance) + string(Ability); 
                }//while
        }//int(main)  
        