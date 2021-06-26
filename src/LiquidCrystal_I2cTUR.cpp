// Based on the work by DFRobot

#include "LiquidCrystal_I2cTUR.h"
#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#define printIIC(args)  Wire.write(args)
inline size_t LiquidCrystal_I2cTUR::write(uint8_t val) {
  bool sent = 0;
  if (val == 195) sent = 0;
  else if (val == 182) { sent = 1; val = 239;}
  else if (val == 156) { sent = 1; val = 245;}
  else if (val == 188) { sent = 1; val = 245;}
  else if (val == 150) { sent = 1; val = 239;}
  else if (val == 135) { sent = 1; val = byte(0);}//Ã‡
  else if (val == 167) { sent = 1; val = byte(0);}//Ã§
  else if (val == 196) { sent = 0; f196 = 1; }
  else if (val == 197) { sent = 0; f197 = 1; }
  else if (val == 158) { sent = 1; if(f196==1) { val = byte(1); f196=0;} else if(f197==1) {val = byte(2); f197=0;} else sent = 0; }
  else if (val == 159) { sent = 1; if(f196==1) { val = byte(3); f196=0;} else if(f197==1) {val = byte(2); f197=0;} else sent = 0; }
  else if (val == 176) { if(f196==1) {sent = 1; val = byte(4); f196=0;} else sent=0;}
  else if (val == 177) { if(f196==1) {sent = 1; val = byte(5); f196=0;} else sent=0;}
  else sent=1;
  if(sent)send(val, Rs);
  return 1;
}

#else
#include "WProgram.h"

#define printIIC(args)  Wire.send(args)
inline void LiquidCrystal_I2cTUR::write(uint8_t value) {
  send(value, Rs);
}

#endif
#include "Wire.h"



// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal_I2cTUR::LiquidCrystal_I2cTUR(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

void LiquidCrystal_I2cTUR::init(){
  init_priv();
  createChar(0, cTr, false);
  createChar(1, GTr, false);
  createChar(2, STr, false);
  createChar(3, _gTr, false);
  createChar(4, ITr, false);
  createChar(5, _iTr, false);
}

void LiquidCrystal_I2cTUR::init_priv()
{
  Wire.begin();
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  begin(_cols, _rows);  
}

void LiquidCrystal_I2cTUR::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  delay(50); 
  
  // Now we pull both RS and R/W low to begin commands
  expanderWrite(_backlightval); // reset expanderand turn backlight off (Bit 8 =1)
  delay(1000);

    //put the LCD into 4 bit mode
  // this is according to the hitachi HD44780 datasheet
  // figure 24, pg 46
  
    // we start in 8bit mode, try to set 4 bit mode
   write4bits(0x03 << 4);
   delayMicroseconds(4500); // wait min 4.1ms
   
   // second try
   write4bits(0x03 << 4);
   delayMicroseconds(4500); // wait min 4.1ms
   
   // third go!
   write4bits(0x03 << 4); 
   delayMicroseconds(150);
   
   // finally, set to 4-bit interface
   write4bits(0x02 << 4); 


  // set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  
  
  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();
  
  // clear it off
  clear();
  
  // Initialize to default text direction (for roman languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
  
  home();
  
}

/********** high level commands, for the user! */
void LiquidCrystal_I2cTUR::clear(){
  command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2cTUR::home(){
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2cTUR::setCursor(uint8_t col, uint8_t row){
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2cTUR::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2cTUR::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2cTUR::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2cTUR::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2cTUR::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2cTUR::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2cTUR::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2cTUR::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2cTUR::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2cTUR::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2cTUR::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2cTUR::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2cTUR::createChar(uint8_t location, uint8_t charmap[], bool userDefined) {
   if(userDefined == true && location <6) return;
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2cTUR::noBacklight(void) {
  _backlightval=LCD_NOBACKLIGHT;
  expanderWrite(0);
}

void LiquidCrystal_I2cTUR::backlight(void) {
  _backlightval=LCD_BACKLIGHT;
  expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2cTUR::command(uint8_t value) {
  send(value, 0);
}


/************ low level data pushing commands **********/

// write either command or data
void LiquidCrystal_I2cTUR::send(uint8_t value, uint8_t mode) {
  uint8_t highnib=value&0xf0;
  uint8_t lownib=(value<<4)&0xf0;
       write4bits((highnib)|mode);
  write4bits((lownib)|mode); 
}

void LiquidCrystal_I2cTUR::write4bits(uint8_t value) {
  expanderWrite(value);
  pulseEnable(value);
}

void LiquidCrystal_I2cTUR::expanderWrite(uint8_t _data){                                        
  Wire.beginTransmission(_Addr);
  printIIC((int)(_data) | _backlightval);
  Wire.endTransmission();   
}

void LiquidCrystal_I2cTUR::pulseEnable(uint8_t _data){
  expanderWrite(_data | En);  // En high
  delayMicroseconds(1);   // enable pulse must be >450ns
  
  expanderWrite(_data & ~En); // En low
  delayMicroseconds(50);    // commands need > 37us to settle
} 


// Alias functions

void LiquidCrystal_I2cTUR::cursor_on(){
  cursor();
}

void LiquidCrystal_I2cTUR::cursor_off(){
  noCursor();
}

void LiquidCrystal_I2cTUR::blink_on(){
  blink();
}

void LiquidCrystal_I2cTUR::blink_off(){
  noBlink();
}

void LiquidCrystal_I2cTUR::load_custom_character(uint8_t char_num, uint8_t *rows){
    createChar(char_num, rows);
}

void LiquidCrystal_I2cTUR::setBacklight(uint8_t new_val){
  if(new_val){
    backlight();    // turn backlight on
  }else{
    noBacklight();    // turn backlight off
  }
}

void LiquidCrystal_I2cTUR::printstr(const char c[]){
  //This function is not identical to the function used for "real" I2C displays
  //it's here so the user sketch doesn't have to be changed 
  print(c);
}


// unsupported API functions
void LiquidCrystal_I2cTUR::off(){}
void LiquidCrystal_I2cTUR::on(){}
void LiquidCrystal_I2cTUR::setDelay (int cmdDelay,int charDelay) {}
uint8_t LiquidCrystal_I2cTUR::status(){return 0;}
uint8_t LiquidCrystal_I2cTUR::keypad (){return 0;}
uint8_t LiquidCrystal_I2cTUR::init_bargraph(uint8_t graphtype){return 0;}
void LiquidCrystal_I2cTUR::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void LiquidCrystal_I2cTUR::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void LiquidCrystal_I2cTUR::setContrast(uint8_t new_val){}

  
