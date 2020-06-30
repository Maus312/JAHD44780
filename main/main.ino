#define WRITE_TO_DR (1)
#define WRITE_TO_IR (0)
#define LCD_CLEAR     (0x01)
#define LCD_CURSOR_HOME (0x02)
#define LCD_DISPLAY_ON  (0x0C)    
#define LCD_DISPLAY_INIT    (0x38)
#define LCD_DISPLAY_SHIFT_RIGHT (0x18)
#define LCD_DISPLAY_SHIFT_LEFT  (0x1C)

// Describes physical connection of different pins
struct LCD_PINOUT {
  unsigned char data07 : 4;
  unsigned char data06 : 4;
  unsigned char data05 : 4;
  unsigned char data04 : 4;
  unsigned char data03 : 4;
  unsigned char data02 : 4;
  unsigned char data01 : 4;
  unsigned char data00 : 4;

  unsigned char enable : 4;
  unsigned char rs : 4;
};

class Custom_LCD {
private:
  struct LCD_PINOUT _pinOut;
  unsigned char _customDelayBtwSends;

public :
  Custom_LCD(){}

  Custom_LCD(struct LCD_PINOUT pinOut, unsigned char delayBtwSends = 100) {
    _pinOut = pinOut;
    _customDelayBtwSends = delayBtwSends;

    pinMode(_pinOut.data07, OUTPUT);
    pinMode(_pinOut.data06, OUTPUT);
    pinMode(_pinOut.data05, OUTPUT);
    pinMode(_pinOut.data04, OUTPUT);
    pinMode(_pinOut.data03, OUTPUT);
    pinMode(_pinOut.data02, OUTPUT);
    pinMode(_pinOut.data01, OUTPUT);
    pinMode(_pinOut.data00, OUTPUT);

    pinMode(_pinOut.enable, OUTPUT);
    pinMode(_pinOut.rs, OUTPUT);
  }

  void writeSymbol(unsigned char data) {
    writeToLCD(data, WRITE_TO_DR);
  }

  void writeInstruction(unsigned char instruction) {
    writeToLCD(instruction, WRITE_TO_IR);
  }

  void writeArrayOfSymbols(unsigned char *pData, unsigned char count) {
    writeArrayToLCD(pData, count, WRITE_TO_DR);
  }

  void writeArrayOfInstructions(unsigned char *pData, unsigned char count) {
    writeArrayToLCD(pData, count, WRITE_TO_IR);
  }

// General writing array of data or instruction to lcd
  void writeArrayToLCD(unsigned char *pData, unsigned char count, unsigned char reg) {
    digitalWrite(_pinOut.rs, reg & 0x01);
    for (unsigned char i = 0; i < count; i++) {
      digitalWrite(_pinOut.enable, HIGH);
      
      setDataPins(pData[i]);
      
      digitalWrite(_pinOut.enable, LOW);
      delay(_customDelayBtwSends);
    }
  }

// General writing data or instruction to lcd
  void writeToLCD(unsigned char data, unsigned char reg) {
    digitalWrite(_pinOut.enable, HIGH);
    digitalWrite(_pinOut.rs, reg & 0x01);

    setDataPins(data);

    digitalWrite(_pinOut.enable, LOW);
    delay(_customDelayBtwSends);
  }

  void setDelayBtwSends(unsigned char newDelay) {
    _customDelayBtwSends = newDelay;
  }

private:
  void setDataPins(unsigned char data) {
    digitalWrite(_pinOut.data00, (data & 0x01));
    digitalWrite(_pinOut.data01, (data & 0x02));
    digitalWrite(_pinOut.data02, (data & 0x04));
    digitalWrite(_pinOut.data03, (data & 0x08));
    digitalWrite(_pinOut.data04, (data & 0x10));
    digitalWrite(_pinOut.data05, (data & 0x20));
    digitalWrite(_pinOut.data06, (data & 0x40));
    digitalWrite(_pinOut.data07, (data & 0x80));
  }
};

Custom_LCD lcd;

void setup() {
  struct LCD_PINOUT newPinout = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  lcd = Custom_LCD(newPinout, 1);

// Start initialization, set 8 bit mode, 2 line and 5x8 dots matrix
  lcd.writeInstruction(LCD_DISPLAY_INIT);
  delay(100);
  lcd.writeInstruction(LCD_DISPLAY_INIT);
  delay(100);
  lcd.writeInstruction(LCD_DISPLAY_ON);
  lcd.writeInstruction(LCD_CLEAR);
// LCD_DISPLAY_ON but with blink cursor
  lcd.writeInstruction(0x0F);

  delay(200);

// Set pointer on cgram address 0x00, first symbol 
  lcd.writeInstruction(0x40);
  unsigned char burger[] = {0x00, 0x0E, 0x11, 0x1F, 0x00, 0x1F, 0x00, 0x00};
  lcd.writeArrayOfSymbols(burger, 8);
// Set pointer on cgram address 0x08, second symbol
  lcd.writeInstruction(0x48);
  unsigned char soda[]   = {0x00, 0x11, 0x11, 0x0A, 0x0A, 0x0A, 0x0E, 0x00};
  lcd.writeArrayOfSymbols(soda, 8);
  
  delay(200);
  
  lcd.writeInstruction(0x80); // SetCursor 0,0
  unsigned char firstString[] = {0x2d, 0x2d, 0x2d, 0x20, 0x01, 0x20, 0x00, 0x20, 0x4d, 0x65, 0x67, 0x61, 0x20, 0x42, 0x75, 0x72, 0x67, 0x65, 0x72, 0x73, 0x20, 0x00, 0x20, 0x01, 0x20, 0x2d, 0x2d, 0x2d};
  lcd.writeArrayOfSymbols(firstString,29);
  lcd.writeInstruction(0xC0); // SetCursor 0,1
  unsigned char secondString[] = {0x2d, 0x2d, 0x2d, 0x21, 0x21, 0x21, 0x42, 0x55, 0x59, 0x20, 0x31, 0x20, 0x47, 0x45, 0x54, 0x20, 0x31, 0x20, 0x46, 0x52, 0x45, 0x45, 0x21, 0x21, 0x21, 0x2d, 0x2d, 0x2d};
  lcd.writeArrayOfSymbols(secondString,29);

  delay(200);
}

void loop() {
  unsigned char direction = 0x00;
  unsigned char instDir = LCD_DISPLAY_SHIFT_RIGHT;
  while(1) {
    instDir = direction ? LCD_DISPLAY_SHIFT_LEFT : LCD_DISPLAY_SHIFT_RIGHT;
    
    for(int i = 0; i < 12; i++) {
      lcd.writeInstruction(instDir);  
      delay(500);
    }
    
    direction = (~direction); 
  }
}
