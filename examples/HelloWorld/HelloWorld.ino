//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
//Türkçe versiyon HakkanR tarafından düzenlenmiştir. => https://www.youtube.com/HakkanR
#include <Wire.h> 
#include <LiquidCrystal_I2cTUR.h>

LiquidCrystal_I2cTUR lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Merhaba Dünya");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Türkçe HakkanR");
}


void loop()
{
}
