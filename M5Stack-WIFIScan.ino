//M5Stack-Core-ESP32 on ttyUSB0
#include <M5Stack.h>
#include "WiFi.h"

#define TFTW  320  // screen width
#define TFTH  240  // screen height
#define MAXWFDISP  13  // maximum to display
#define MAXSSIDLEN 32

//GLOBAL
int tmpNumberNetworks;
int iPage = 0;
int iTPages = 0;
int iScanned = 0;
int iNumberNetworks = 0;
int SSIDISPLEN = 15; // SSID chars to display in table
char EXT[] = "~"; // what to display as "cut"

// convert AUTH to text
String translateEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "OPEN";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENT";
  }
}

void displayScanB(){
  M5.Lcd.setCursor(45, 230);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print("((");
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.print("(");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.print("*");
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.print(")");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print("))");
}

void displayNextB(){
  M5.Lcd.setCursor(145, 230);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print("[ ");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.print("<");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print(" ]");
}

void displayPrevB(){
  M5.Lcd.setCursor(235, 230);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print("[ ");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.print(">");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print(" ]");
}

void setup() {
    M5.begin();
    M5.Power.begin();
    WiFi.mode(WIFI_STA);  // Set WiFi to station mode and disconnect from an AP
    WiFi.disconnect();  // Turn off all wifi connections
    delay(100);         // 100 ms delay

    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(96, 100);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.print("((");
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.print("(");
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.print(" wi-fi scanner ");
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.print(")");
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.print("))");

    char MACbuffer[18];
    for (int li=0; li < 18 ; li++){
      MACbuffer[li]=WiFi.macAddress()[li];
    }
    M5.Lcd.setCursor(93, 120);
    M5.Lcd.printf("MAC: %*s", 17, MACbuffer);
    //delay(3000);         // 3000 ms delay

    displayScanB();
}

void displayPage(int ilNN, int ilP, int ilTPs){
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print("Total networks: ");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("%02d", ilNN);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print(" | Displaying page: ");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("%02d", ilP);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.print(" of ");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("%02d\n\n", ilTPs);

  int illNN; //page breaking to display the right amount of networks
  if (MAXWFDISP*ilP <= ilNN) {
    illNN = MAXWFDISP*ilP;
  } else {
    illNN = ilNN;
  }

  uint16_t BGCL; // background color
  uint16_t GTXT; // green text color
  uint16_t BTXT; // blue text color
  uint16_t RTXT; // red text color
  uint16_t YTXT; // yellow text color

  for (int i = MAXWFDISP*(ilP-1); i < illNN; ++i) {
    
    //line color in table
    if ((i + 1)%2) {
      BGCL = 0x10a3; // low light
      GTXT = 0x0540;
      BTXT = 0x0015;
      RTXT = 0x9800;
      YTXT = 0xbd44;
    } else {
      BGCL = 0x2945; // high light
      GTXT = 0x07e0;
      BTXT = 0x001a;
      RTXT = 0xc000;
      YTXT = 0xfe00;
    }
        
    //PRINT SSID
    char * SSIDbuffer = new char [WiFi.SSID(i).length()+1]; //str to char
    strcpy (SSIDbuffer, WiFi.SSID(i).c_str());
    int SSIDISPPRN; // printable chars in SSID (temp value)

    if (strlen(SSIDbuffer) <= SSIDISPLEN) {
     SSIDISPPRN = SSIDISPLEN; 
    } else {
      SSIDISPPRN = SSIDISPLEN - strlen(EXT);
      for (int li=SSIDISPPRN; li < strlen(SSIDbuffer) ; li++){
        SSIDbuffer[li]= 0;
      }
    }

    M5.Lcd.setTextColor(GTXT,BGCL);
    if (SSIDISPPRN == SSIDISPLEN) {
      M5.Lcd.printf("%-*s", SSIDISPPRN, SSIDbuffer);
    } else {
      M5.Lcd.print(SSIDbuffer);
      M5.Lcd.setTextColor(WHITE,BGCL);
      M5.Lcd.print(EXT);
    }

    M5.Lcd.print(" ");

    //PRINT MAC
    M5.Lcd.setTextColor(YTXT,BGCL);
    char * MACbuffer = new char [WiFi.BSSIDstr(i).length()+1]; //str to char
    strcpy (MACbuffer, WiFi.BSSIDstr(i).c_str());
    M5.Lcd.printf("%*s", 17, MACbuffer);

    M5.Lcd.print(" ");

    //PRINT CHANNEL
    M5.Lcd.setTextColor(GTXT,BGCL);
    M5.Lcd.printf("%02d",WiFi.channel(i));

    M5.Lcd.print(" ");

    //PRINT SIGNAL
    if ((WiFi.RSSI(i) <= -35) and (WiFi.RSSI(i) >= -65)) {
      M5.Lcd.setTextColor(WHITE,BGCL);
      M5.Lcd.print("*)]");
    } else {
      if ((WiFi.RSSI(i) <= -66) and (WiFi.RSSI(i) >= -75)) {
        M5.Lcd.setTextColor(WHITE,BGCL);
        M5.Lcd.print("*) ");
      } else {
        M5.Lcd.setTextColor(WHITE,BGCL);
        M5.Lcd.print("*  ");
      }
    }
    ///M5.Lcd.printf("%3d", WiFi.RSSI(i));

    M5.Lcd.printf(" ");

    //PRINT AUTH
    M5.Lcd.setTextColor(BTXT,BGCL);
    if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
      M5.Lcd.setTextColor(GTXT,BGCL);
    }
    M5.Lcd.printf("%-*s\n",12,translateEncryptionType(WiFi.encryptionType(i)));

    if (((i+1)%MAXWFDISP) and ((i+1)<illNN)){
      M5.Lcd.printf("\n");
    }
  }
  
  // buttons menu 
  displayScanB();
  
  if ((iScanned) and (ilTPs > 1)) {
    if (ilP > 1) {
      displayNextB();
    }
    if (ilP < ilTPs) {
      displayPrevB();
    }
  }
}

void loop() {
    M5.update();  // Check the status of the keys
    if (M5.BtnA.isPressed()) {  // button A (((*)))
        M5.Lcd.clear();
        M5.Lcd.setCursor(115, 100);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.print("((");
        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.print("(");
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.print(" scaning ");
        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.print(")");
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.print("))");

        iScanned = 1;
        iNumberNetworks = WiFi.scanNetworks();  // return the number of networks found

        if (iNumberNetworks == 0) {  // If no network is found
          M5.Lcd.clear();
          M5.Lcd.setCursor(135, 100);
          M5.Lcd.setTextColor(WHITE);
          M5.Lcd.print(")");
          M5.Lcd.setTextColor(YELLOW);
          M5.Lcd.print(")");
          M5.Lcd.setTextColor(RED);
          M5.Lcd.print(")-X-(");
          M5.Lcd.setTextColor(YELLOW);
          M5.Lcd.print("(");
          M5.Lcd.setTextColor(WHITE);
          M5.Lcd.print("(");

          displayScanB();

        } else {
          if (iNumberNetworks >= MAXWFDISP) {
              if (iNumberNetworks%MAXWFDISP) {
                iTPages = iNumberNetworks/MAXWFDISP + 1;
              } else {
                iTPages = iNumberNetworks/MAXWFDISP;
              }
            } else {
              iTPages = iNumberNetworks/MAXWFDISP + 1;
            }
          iPage = 1;
          displayPage(iNumberNetworks,iPage,iTPages);
        }
      delay (200);
    }

    if ((M5.BtnC.isPressed()) and (iScanned)) {  // button C [ > ]
      iPage++;
      if (iPage > iTPages) {
        iPage = iTPages;
      } else {
        displayPage(iNumberNetworks,iPage,iTPages);
      }
      delay (200);
    }

    if ((M5.BtnB.isPressed()) and (iScanned)) {  // button B [ < ]
      iPage--;
      if (iPage < 1) {
        iPage = 1;
      } else{
        displayPage(iNumberNetworks,iPage,iTPages);
      }
      delay (200);
    }
}
