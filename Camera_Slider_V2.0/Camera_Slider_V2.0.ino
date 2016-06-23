#include <SPI.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// SPI OLED Definition
#define OLED_MOSI     2   // SDA = Digital 8
#define OLED_CLK      3   // SCL = Digital 9
#define OLED_DC       4  // D/C = Digital 10
#define OLED_CS       1   // CS  = Not Used
#define OLED_RESET    5  // RES = Digital 11
                          // VCC = 5V
                          // GND = GND

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//Stepper Motor Driver Definition
#define STEPPER_ENABLE      8
#define STEPPER_DIRECTION   9
#define STEPPER_PULSE       10
#define STEPPER_MS1         11
#define STEPPER_MS2         12
#define STEPPER_MS3         13

//JoyStick 1 Definition
#define JOYSTICK_Y1     A0
#define JOYSTICK_X1     A1
#define JOYSTICK_Z1     6

#define JOYSTICK_X1_LEFT    1
#define JOYSTICK_X1_RIGHT   2
#define JOYSTICK_Y1_UP      3 
#define JOYSTICK_Y1_DOWN    4
#define JOYSTICK_Z1_BUTTON  5 

//JoyStick 2 Definition
#define JOYSTICK_Y2     A2
#define JOYSTICK_X2     A3
#define JOYSTICK_Z2     7

#define JOYSTICK_X2_LEFT    6
#define JOYSTICK_X2_RIGHT   7
#define JOYSTICK_Y2_UP      8 
#define JOYSTICK_Y2_DOWN    9
#define JOYSTICK_Z2_BUTTON  10 

#define ACTIVE_MOTOR_LED  22

//Limit Switch Definition
#define LIMIT_RIGHT   24
#define LIMIT_LEFT    25

#define L2R           1
#define R2L           2
#define NO_MO         3

#define SHOOTING_INTERVAL 1
#define CLIP_LENGTH       2
#define EVENT_DURATION    3

int lastButtonPushed = 0;

int joystickReading;
int limitReading;

int joystick_X1_LeftState = LOW;
int joystick_X1_RightState = LOW;
int joystick_Y1_UpState = LOW;
int joystick_Y1_DownState = LOW;
int joystick_Z1_ButtonState = HIGH;

int joystick_X2_LeftState = LOW;
int joystick_X2_RightState = LOW;
int joystick_Y2_UpState = LOW;
int joystick_Y2_DownState = LOW;
int joystick_Z2_ButtonState = HIGH;

int lastJoystick_Z1_ButtonState = HIGH;
int lastJoystick_Z2_ButtonState = HIGH;

long lastJoystick_Z1_ButtonDebounceTime = 0;
long lastJoystick_Z2_ButtonDebounceTime = 0;

long debounceDelay = 20;

byte mainMenuPage = 1;
byte mainMenuPageOld = 1;
byte minMainMenuPage = 1;
byte maxMainMenuPage = 3;

byte subMenu1Page = 1;
byte subMenu1PageOld = 1;
byte minSubMenu1Page = 1;
byte maxSubMenu1Page = 8;

byte subMenu2Page = 1;
byte subMenu2PageOld = 1;
byte minSubMenu2Page = 1;
byte maxSubMenu2Page = 3;

byte calculationMode = 1;
byte calculationModeOld = 1;
byte minCalculationMode = 1;
byte maxCalculationMode = 3;

byte sliderMotion = 1;
byte sliderMotionOld = 1;
byte minSliderMotion = 0;
byte maxSliderMotion = 3;

byte eventDuration = 60;
byte eventDurationOld = 60;
byte minEventDuration = 1;
byte maxEventDuration = 120;

byte clipLength = 20;
byte clipLengthOld = 20;
byte minClipLength = 1;
byte maxClipLength = 60;

byte frameRate = 30;
byte frameRateOld = 30;
byte minFrameRate = 24;
byte maxFrameRate = 60;

byte shootingInterval = 5;
byte shootingIntervalOld = 5;
byte minShootingInterval = 1;
byte maxShootingInterval = 30;

byte imageSize = 16;
byte imageSizeOld = 16;
byte minImageSize = 1;
byte maxImageSize = 50;

byte videoMode = 1;
byte videoModeOld = 1;
byte minVideoMode = 1;
byte maxVideoMode = 2;

byte sliderSpeed = 1;
byte sliderSpeedOld = 1;
byte minSliderSpeed = 1;
byte maxSliderSpeed = 10;

int numberPhotos = 0;
float memoryUsage = 0.0;
int calcShootingInterval = 0;

int sliderStepDistance = 0;
int totalSteps = 0;
int stepsPerPhoto = 0;
int photoCounter = 0;
int stepCounter = 0;
int cumulativeStepCounter = 0;

const unsigned char PROGMEM Camera_Shutter_Splash [] = 
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x18, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x7E, 0x03, 0x00, 0x00, 0xC0, 0x00, 0x00,
0x00, 0x7E, 0x06, 0x00, 0x00, 0x60, 0x00, 0x00, 0x07, 0xFF, 0xFC, 0x00, 0x00, 0x3F, 0xFF, 0xE0,
0x0F, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
0x30, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x31, 0xFC, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x0C,
0x31, 0xFC, 0x00, 0x78, 0x1E, 0x00, 0x00, 0x0C, 0x31, 0xFC, 0x01, 0xC0, 0x03, 0x80, 0x00, 0x0C,
0x31, 0xFC, 0x03, 0x80, 0x01, 0xC0, 0x00, 0x0C, 0x30, 0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x0C,
0x30, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x0C, 0x30, 0x00, 0x0C, 0x00, 0x00, 0x30, 0x00, 0x0C,
0x30, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x0C, 0x30, 0x00, 0x18, 0x07, 0xE0, 0x18, 0x00, 0x0C,
0x30, 0x00, 0x10, 0x0C, 0x30, 0x08, 0x00, 0x0C, 0x30, 0x00, 0x30, 0x18, 0x18, 0x0C, 0x00, 0x0C,
0x30, 0x00, 0x30, 0x30, 0x0C, 0x0C, 0x00, 0x0C, 0x30, 0x00, 0x30, 0x30, 0x0C, 0x0C, 0x00, 0x0C,
0x30, 0x00, 0x30, 0x30, 0x0C, 0x0C, 0x00, 0x0C, 0x30, 0x00, 0x30, 0x30, 0x0C, 0x0C, 0x00, 0x0C,
0x30, 0x00, 0x30, 0x30, 0x0C, 0x0C, 0x00, 0x0C, 0x30, 0x00, 0x30, 0x10, 0x08, 0x0C, 0x00, 0x0C,
0x30, 0x00, 0x30, 0x18, 0x18, 0x0C, 0x00, 0x0C, 0x30, 0x00, 0x10, 0x0F, 0xF0, 0x08, 0x00, 0x0C,
0x30, 0x00, 0x18, 0x03, 0xC0, 0x18, 0x00, 0x0C, 0x30, 0x00, 0x08, 0x00, 0x00, 0x10, 0x00, 0x0C,
0x30, 0x00, 0x0C, 0x00, 0x00, 0x30, 0x00, 0x0C, 0x30, 0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x0C,
0x30, 0x00, 0x03, 0x00, 0x00, 0xC0, 0x00, 0x0C, 0x30, 0x00, 0x01, 0xC0, 0x03, 0x80, 0x00, 0x0C,
0x30, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x0C,
0x30, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM Clock_BMP [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFC, 0x00,
0x00, 0x30, 0x03, 0x00, 0x00, 0x40, 0x00, 0xC0, 0x01, 0x80, 0x00, 0x60, 0x03, 0x00, 0x00, 0x10,
0x02, 0x00, 0x00, 0x18, 0x04, 0x00, 0xC0, 0x08, 0x08, 0x00, 0xC0, 0x04, 0x08, 0x00, 0xC0, 0x04,
0x10, 0x00, 0xC0, 0x02, 0x10, 0x00, 0xC0, 0x02, 0x10, 0x00, 0xC0, 0x02, 0x10, 0x00, 0xC0, 0x02,
0x10, 0x00, 0xFE, 0x03, 0x10, 0x00, 0x7E, 0x03, 0x10, 0x00, 0x00, 0x02, 0x10, 0x00, 0x00, 0x02,
0x10, 0x00, 0x00, 0x02, 0x10, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x04,
0x04, 0x00, 0x00, 0x08, 0x06, 0x00, 0x00, 0x18, 0x02, 0x00, 0x00, 0x10, 0x01, 0x80, 0x00, 0x60,
0x00, 0xC0, 0x00, 0xC0, 0x00, 0x30, 0x03, 0x00, 0x00, 0x0F, 0xFC, 0x00, 0x00, 0x00, 0xC0, 0x00
};

const unsigned char PROGMEM Med_LArrow_BMP [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x78, 0x00,
0x00, 0x00, 0xF0, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x07, 0x80, 0x00,
0x00, 0x0F, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00,
0x00, 0x78, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
0x00, 0x07, 0x80, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0xF0, 0x00,
0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x0E, 0x00,
0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM Med_RArrow_BMP [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
0x00, 0x70, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00,
0x00, 0x0F, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0xE0, 0x00,
0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x1E, 0x00,
0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0xF0, 0x00,
0x00, 0x01, 0xE0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x0F, 0x00, 0x00,
0x00, 0x1E, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00,
0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM X_BMP [] = {
0x00, 0x01, 0x80, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x01, 0xFF, 0xFF, 0x80,
0x03, 0xE0, 0x07, 0xC0, 0x07, 0x80, 0x01, 0xE0, 0x0F, 0x80, 0x01, 0xF0, 0x1F, 0xC0, 0x03, 0xF8,
0x1F, 0xE0, 0x07, 0xF8, 0x39, 0xF0, 0x0F, 0x9C, 0x38, 0xF8, 0x1F, 0x1C, 0x70, 0x7C, 0x3E, 0x0E,
0x70, 0x3E, 0x7C, 0x0E, 0x70, 0x1F, 0xF8, 0x0E, 0x70, 0x0F, 0xF0, 0x0E, 0xF0, 0x07, 0xE0, 0x0F,
0xF0, 0x07, 0xE0, 0x0F, 0x70, 0x0F, 0xF0, 0x0E, 0x70, 0x1F, 0xF8, 0x0E, 0x70, 0x3E, 0x7C, 0x0E,
0x70, 0x7C, 0x3E, 0x0E, 0x38, 0xF8, 0x1F, 0x1C, 0x39, 0xF0, 0x0F, 0x9C, 0x1F, 0xE0, 0x07, 0xF8,
0x1F, 0xC0, 0x03, 0xF8, 0x0F, 0x80, 0x01, 0xF0, 0x07, 0x80, 0x01, 0xE0, 0x03, 0xE0, 0x07, 0xC0,
0x01, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x01, 0x80, 0x00
};

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() 
{
  pinMode(JOYSTICK_X1, INPUT);
  pinMode(JOYSTICK_Y1, INPUT);
  pinMode(JOYSTICK_Z1, INPUT);

  pinMode(JOYSTICK_X2, INPUT);
  pinMode(JOYSTICK_Y2, INPUT);
  pinMode(JOYSTICK_Z2, INPUT);

  pinMode(STEPPER_ENABLE, OUTPUT);
  digitalWrite(STEPPER_ENABLE, LOW);
  pinMode(STEPPER_DIRECTION, OUTPUT);
  digitalWrite(STEPPER_DIRECTION, LOW);
  pinMode(STEPPER_PULSE, OUTPUT);
  digitalWrite(STEPPER_PULSE, LOW);

  pinMode(ACTIVE_MOTOR_LED, OUTPUT);
  digitalWrite(ACTIVE_MOTOR_LED, LOW);
  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.drawBitmap(32, 0, Camera_Shutter_Splash, 64, 64, 1);
  display.display();
  delay(2000);
  display.clearDisplay();
  
  mainMenuDisplay();
}

void loop() 
{
  readJoystick();  
  
  navigateMainMenu();
  
  delay(10);
}

void subMenu1()
{
  subMenu1Display();

  while(lastButtonPushed != JOYSTICK_Z1_BUTTON)
  {
    readJoystick();
    
    navigateSubMenu1();

    delay(10);
  }
  
  mainMenuDisplay();
  
  subMenu1Page = 1;
}

void subMenu2()
{
  subMenu2Display();

  while(lastButtonPushed != JOYSTICK_Z1_BUTTON)
  {
    readJoystick();
    
    navigateSubMenu2();

    delay(10);
  }
  
  mainMenuDisplay();
  
  subMenu2Page = 1; 
}

void mainMenuDisplay()
{
  switch(mainMenuPage)
  {
    case 1:
    {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(10,24);
      display.println("TIMELAPSE");
    }
    break;
    case 2:
    {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(34,24);
      display.println("VIDEO");
    }
    break;
    case 3:
    {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(34,24);
      display.println("ASTRO");
    }
    break;
  }

  display.display();
  delay(100);
  display.clearDisplay();
}

void subMenu1Display()
{
  switch(subMenu1Page)
  {
    case 1:
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(46,0);
      display.println("MOTION");
          
      switch(sliderMotion)
      {
        case L2R:
        {
          display.drawBitmap(48, 24, Med_RArrow_BMP, 32, 32, 1);
        }
        break;
        case R2L:
        {
          display.drawBitmap(48, 24, Med_LArrow_BMP, 32, 32, 1);
        }
        break;
        case NO_MO:
        {
          display.drawBitmap(48, 24, X_BMP, 32, 32, 1);
        }
        break;
      } 
      
      display.display();
      delay(100);
      display.clearDisplay();  
    }
    break;
    case 2:
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(31,0);
      display.println("CALCULATION");
          
      switch(calculationMode)
      {
        case SHOOTING_INTERVAL:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(13,28);
          display.println("SHOOTING INTERVAL");
        }
        break;
        case CLIP_LENGTH:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(31,28);
          display.println("CLIP LENGTH");  
        }
        break;
        case EVENT_DURATION:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(22,28);
          display.println("EVENT DURATION"); 
        }
        break;
      }
      display.display();
      delay(100);
      display.clearDisplay();  
    }
    break;
    case 3:
    {
      switch(calculationMode)
      {
        case SHOOTING_INTERVAL:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(31,0);
          display.println("CLIP LENGTH");
          
          if(clipLength < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(clipLength);
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println(clipLength); 
          }
         
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(85,34);
          display.println("Sec"); 
        }
        break;
        case CLIP_LENGTH:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(13,0);
          display.println("SHOOTING INTERVAL");

          if(shootingInterval < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(shootingInterval);
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println(shootingInterval); 
          }
          
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(85,34);
          display.println("Sec");  
        }
        break;
        case EVENT_DURATION:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(13,0);
          display.println("SHOOTING INTERVAL");

          if(shootingInterval < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(shootingInterval);
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println(shootingInterval); 
          }
          
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(85,34);
          display.println("Sec");  
        }
        break;
      }

      display.display();
      delay(100);
      display.clearDisplay();
    }
    break;
    case 4:
    {
      switch(calculationMode)
      {
        case SHOOTING_INTERVAL:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(22,0);
          display.println("EVENT DURATION");

          if(eventDuration < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(37,20);
            display.println("0");

            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(55,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(eventDuration);
          }
          else if((eventDuration > 10) & (eventDuration < 100))
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(37,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(55,20);
            display.println(eventDuration); 
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(37,20);
            display.println(eventDuration);  
          }
          
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(95,34);
          display.println("Min");
        }
        break;
        case CLIP_LENGTH:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(22,0);
          display.println("EVENT DURATION");

          if(eventDuration < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(37,20);
            display.println("0");

            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(55,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(eventDuration);
          }
          else if((eventDuration > 10) & (eventDuration < 100))
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(37,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(55,20);
            display.println(eventDuration); 
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(37,20);
            display.println(eventDuration);  
          }
          
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(95,34);
          display.println("Min"); 
        }
        break;
        case EVENT_DURATION:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(31,0);
          display.println("CLIP LENGTH");
          
          if(clipLength < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(clipLength);
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println(clipLength); 
          }
         
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(85,34);
          display.println("Sec");     
        }
        break;
      } 
      
      display.display();
      delay(100);
      display.clearDisplay(); 
    }
    break;
    case 5:
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(34,0);
      display.println("FRAME RATE");
      
      if(frameRate < 10)
      {
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(46,20);
        display.println("0");
        
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(64,20);
        display.println(frameRate);
      }
      else
      {
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(46,20);
        display.println(frameRate); 
      }
          
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(85,34);
      display.println("FPS");
      
      display.display();
      delay(100);
      display.clearDisplay();   
    }
    break;
    case 6:
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(34,0);
      display.println("IMAGE SIZE");
      
      if(imageSize < 10)
      {
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(46,20);
        display.println("0");
        
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(64,20);
        display.println(imageSize);
      }
      else
      {
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(46,20);
        display.println(imageSize); 
      }
      
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(85,34);
      display.println("MB");
      
      display.display();
      delay(100);
      display.clearDisplay();  
    }
    break;
    case 7:
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(43,0);
      display.println("SUMMARY");

      numberPhotos = (clipLength * frameRate);
      memoryUsage = ((float(imageSize) * float(numberPhotos))/1000);
      calcShootingInterval = ((eventDuration * 60)/ numberPhotos);
      
      switch(calculationMode)
      { 
        case SHOOTING_INTERVAL:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,10);
          display.println("Clip:");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(70,10);
          display.println(clipLength);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(100,10);
          display.println("Sec");

          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,19);
          display.println("Event:");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(70,19);
          display.println(eventDuration);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(100,19);
          display.println("Min");
          
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,28);
          display.println("Interval:");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(70,28);
          display.println(calcShootingInterval);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(100,28);
          display.println("Sec");

          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,37);
          display.println("FPS:");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(70,37);
          display.println(frameRate);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(100,37);
          display.println("FPS");

          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,46);
          display.println("Photos:");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(70,46);
          display.println(numberPhotos);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(100,46);
          display.println("Pic");

          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,55);
          display.println("Memory:");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(70,55);
          display.println(memoryUsage, 1);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(100,55);
          display.println("GB");
        }
        break;
        case CLIP_LENGTH:
        {
          
        }
        break;
        case EVENT_DURATION:
        {
          
        }
        break;
      } 

      display.display();
      delay(100);
      display.clearDisplay();  
    }
    break;
    case 8:
    {
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(19,20);
      display.println("START");
      display.display();
      delay(100);
      display.clearDisplay();  
    }
    break;
  }
}

void subMenu2Display()
{
  switch(subMenu2Page)
  {
    case 1:
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(52,0);
      display.println("MODE"); 

      switch(videoMode)
      {
        case 1:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(37,28);
          display.println("AUTOMATIC");
        }
        break;
        case 2:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(46,28);
          display.println("MANUAL");
        }
        break;
      }

      display.display();
      delay(100);
      display.clearDisplay();
    }
    break;
    case 2:
    {
      switch(videoMode)
      {
        case 1:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(46,0);
          display.println("MOTION");
      
          switch(sliderMotion)
          {
            case L2R:
            {
              display.drawBitmap(48, 24, Med_RArrow_BMP, 32, 32, 1);
            }
            break;
            case R2L:
            {
              display.drawBitmap(48, 24, Med_LArrow_BMP, 32, 32, 1);
            }
            break;
            case NO_MO:
            {
              display.drawBitmap(48, 24, X_BMP, 32, 32, 1);
            }
            break;
          } 
          
          display.display();
          delay(100);
          display.clearDisplay();
        }
        break;
        case 2:
        {
          
        }
        break;
      }  
    }
    break;
    case 3:
    {
      switch(videoMode)
      {
        case 1:
        {
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(49,0);
          display.println("SPEED");  

          if(sliderSpeed < 10)
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println("0");
            
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(64,20);
            display.println(sliderSpeed);
          }
          else
          {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(46,20);
            display.println(sliderSpeed); 
          }

          display.display();
          delay(100);
          display.clearDisplay(); 
        }
        break;
      }
    }
    break;
  }
}

void readJoystick()
{
  joystick_X1_LeftState = LOW;
  joystick_X1_RightState = LOW;
  joystick_Y1_UpState = LOW;
  joystick_Y1_DownState = LOW;
  joystick_Z1_ButtonState = HIGH;

  joystick_X2_LeftState = LOW;
  joystick_X2_RightState = LOW;
  joystick_Y2_UpState = LOW;
  joystick_Y2_DownState = LOW;
  joystick_Z2_ButtonState = HIGH;
  
  //JoyStick X1 ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  joystickReading = analogRead(JOYSTICK_X1); 

  if(joystickReading < 10)
  {
    joystick_X1_LeftState = HIGH;
  }
  else if(joystickReading > 1000)
  {
    joystick_X1_RightState = HIGH;
  }
  else
  {
    joystick_X1_LeftState = LOW;
    joystick_X1_RightState = LOW;
  }
  delay(10);

  //JoyStick Y1 ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  joystickReading = analogRead(JOYSTICK_Y1); 

  if(joystickReading < 10)
  {
    joystick_Y1_DownState = HIGH;
  }
  else if(joystickReading > 1000)
  {
    joystick_Y1_UpState = HIGH;
  }
  else
  {
    joystick_Y1_DownState = LOW;
    joystick_Y1_UpState = LOW;
  }
  delay(10);
  
  //JoyStick Z1 Button///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  joystickReading = digitalRead(JOYSTICK_Z1); 

  if(joystickReading != lastJoystick_Z1_ButtonState)
  {
    lastJoystick_Z1_ButtonDebounceTime = millis();  
  }

  if((millis() - lastJoystick_Z1_ButtonDebounceTime) > debounceDelay) 
  {
    if(joystickReading != joystick_Z1_ButtonState)
    {
      joystick_Z1_ButtonState = joystickReading;
      lastJoystick_Z1_ButtonDebounceTime = millis();
    }
  }
  lastJoystick_Z1_ButtonState = joystickReading;

  //JoyStick X2 ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  joystickReading = analogRead(JOYSTICK_X2); 

  if(joystickReading < 10)
  {
    joystick_X2_LeftState = HIGH;
  }
  else if(joystickReading > 1000)
  {
    joystick_X2_RightState = HIGH;
  }
  else
  {
    joystick_X2_LeftState = LOW;
    joystick_X2_RightState = LOW;
  }
  delay(10);

  //JoyStick Y2 ///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  joystickReading = analogRead(JOYSTICK_Y2); 

  if(joystickReading < 10)
  {
    joystick_Y2_DownState = HIGH;
  }
  else if(joystickReading > 1000)
  {
    joystick_Y2_UpState = HIGH;
  }
  else
  {
    joystick_Y2_DownState = LOW;
    joystick_Y2_UpState = LOW;
  }
  delay(10);
  
  //JoyStick Z2 Button///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  joystickReading = digitalRead(JOYSTICK_Z2); 

  if(joystickReading != lastJoystick_Z2_ButtonState)
  {
    lastJoystick_Z2_ButtonDebounceTime = millis();  
  }

  if((millis() - lastJoystick_Z2_ButtonDebounceTime) > debounceDelay) 
  {
    if(joystickReading != joystick_Z2_ButtonState)
    {
      joystick_Z2_ButtonState = joystickReading;
      lastJoystick_Z2_ButtonDebounceTime = millis();
    }
  }
  lastJoystick_Z2_ButtonState = joystickReading;


  //Record Which Button Has Been Pressed
  if(joystick_X1_LeftState == HIGH)
  {
    lastButtonPushed = JOYSTICK_X1_LEFT;
  }
  else if(joystick_X1_RightState == HIGH)
  {
    lastButtonPushed = JOYSTICK_X1_RIGHT;
  }
  else if(joystick_Y1_UpState == HIGH)
  {
    lastButtonPushed = JOYSTICK_Y1_UP;
  }
  else if(joystick_Y1_DownState == HIGH)
  {
    lastButtonPushed = JOYSTICK_Y1_DOWN;
  }
  else if(joystick_Z1_ButtonState == LOW)
  {
    lastButtonPushed = JOYSTICK_Z1_BUTTON;
  }
  else if(joystick_X2_LeftState == HIGH)
  {
    lastButtonPushed = JOYSTICK_X2_LEFT;
  }
  else if(joystick_X2_RightState == HIGH)
  {
    lastButtonPushed = JOYSTICK_X2_RIGHT;
  }
  else if(joystick_Y2_UpState == HIGH)
  {
    lastButtonPushed = JOYSTICK_Y2_UP;
  }
  else if(joystick_Y2_DownState == HIGH)
  {
    lastButtonPushed = JOYSTICK_Y2_DOWN;
  }
  else if(joystick_Z2_ButtonState == LOW)
  {
    lastButtonPushed = JOYSTICK_Z2_BUTTON;
  }
  else
  {
    lastButtonPushed = 0;
  }
}

void navigateMainMenu()
{ 
  switch(lastButtonPushed)
  {
    case JOYSTICK_X1_LEFT:
    {
      mainMenuPage--;
      if(mainMenuPage == 0)
      {
        mainMenuPage = maxMainMenuPage;
      }
    }
    break;
    case JOYSTICK_X1_RIGHT:
    {
      mainMenuPage++;
      if(mainMenuPage > maxMainMenuPage)
      {
        mainMenuPage = 1;
      }
    }
    break;
    case JOYSTICK_Z2_BUTTON:
    {
      switch(mainMenuPage)
      {
        case 1:
        {
          subMenu1();
        }
        break;
        case 2:
        {
          subMenu2();
        }
      }  
    }
    break;
  }
  
  if(mainMenuPage != mainMenuPageOld)
  {
    mainMenuDisplay();
    mainMenuPageOld = mainMenuPage;
  } 
  lastButtonPushed = 0;
}

void navigateSubMenu1()
{ 
  switch(lastButtonPushed)
  {
    case JOYSTICK_X1_LEFT:
    {
      subMenu1Page--;
      if(subMenu1Page == 0)
      {
        subMenu1Page = minSubMenu1Page;
      }
    }
    break;
    case JOYSTICK_X1_RIGHT:
    {
      subMenu1Page++;
      if(subMenu1Page > maxSubMenu1Page)
      {
        subMenu1Page = maxSubMenu1Page;
      }
    }
    break;
    case JOYSTICK_Y2_DOWN:
    {
      switch(subMenu1Page)
      {
        case 1:
        {
          sliderMotion--;
          if(sliderMotion == 0)
          {
            sliderMotion = maxSliderMotion;
          }
        }
        break;
        case 2:
        {
          calculationMode--;
          if(calculationMode == 0)
          {
            calculationMode = maxCalculationMode;
          }
        }
        break;
        case 3:
        {
          switch(calculationMode)
          {
            case SHOOTING_INTERVAL:
            {
              clipLength--;
              if(clipLength == 4)
              {
                clipLength = minClipLength;
              } 
            }
            break;
            case CLIP_LENGTH:
            {
              shootingInterval--;
              if(shootingInterval == 0)
              {
                shootingInterval = minShootingInterval;  
              }
            }
            break;
            case EVENT_DURATION:
            {
              shootingInterval--;
              if(shootingInterval == 0)
              {
                shootingInterval = minShootingInterval;  
              } 
            }
            break;
          }  
        }
        break;
        case 4:
        {
          switch(calculationMode)
          {
            case SHOOTING_INTERVAL:
            {
              eventDuration--;
              if(eventDuration == 4)
              {
                eventDuration = minEventDuration;
              } 
            }
            break;
            case CLIP_LENGTH:
            {
              eventDuration--;
              if(eventDuration == 4)
              {
                eventDuration = minEventDuration;
              } 
            }
            break;
            case EVENT_DURATION:
            {
              clipLength--;
              if(clipLength == 4)
              {
                clipLength = minClipLength;
              } 
            }
            break;
          }
        }
        break;
        case 5:
        {
          frameRate--;
          if(frameRate == 23)
          {
            frameRate = minFrameRate;
          }
        }
        break;
        case 6:
        {
          imageSize--;
          if(imageSize == 0)
          {
            imageSize = minImageSize;
          }
        }
        break;
      }   
    }
    break;
    case JOYSTICK_Y2_UP:
    {
      switch(subMenu1Page)
      {
        case 1:
        {
          sliderMotion++;
          if(sliderMotion > maxSliderMotion)
          {
            sliderMotion = 1;
          } 
        }
        break;
        case 2:
        {
          calculationMode++;
          if(calculationMode > maxCalculationMode)
          {
            calculationMode = 1;
          }
        }
        break;
        case 3:
        {
          switch(calculationMode)
          {
            case SHOOTING_INTERVAL:
            {
              clipLength++;
              if(clipLength > maxClipLength)
              {
                clipLength = maxClipLength;
              } 
            }
            break;
            case CLIP_LENGTH:
            {
              shootingInterval++;
              if(shootingInterval > maxShootingInterval)
              {
                shootingInterval = maxShootingInterval;  
              }
            }
            break;
            case EVENT_DURATION:
            {
              shootingInterval++;
              if(shootingInterval > maxShootingInterval)
              {
                shootingInterval = maxShootingInterval;  
              } 
            }
            break;
          }
        }
        break;
        case 4:
        {
          switch(calculationMode)
          {
            case SHOOTING_INTERVAL:
            {
              eventDuration++;
              if(eventDuration > maxEventDuration)
              {
                eventDuration = maxEventDuration;
              } 
            }
            break;
            case CLIP_LENGTH:
            {
              eventDuration++;
              if(eventDuration > maxEventDuration)
              {
                eventDuration = maxEventDuration;
              } 
            }
            break;
            case EVENT_DURATION:
            {
              clipLength++;
              if(clipLength > maxClipLength)
              {
                clipLength = maxClipLength;
              } 
            }
            break;
          }
        }
        break;
        case 5:
        {
          frameRate++;
          if(frameRate > maxFrameRate)
          {
            frameRate = maxFrameRate;
          }
        }
        break;
        case 6:
        {
          imageSize++;
          if(imageSize > maxImageSize)
          {
            imageSize = maxImageSize;
          }
        }
        break;
      }
    }
    break;
    case JOYSTICK_Z2_BUTTON:
    {
      switch(subMenu1Page)
      {
        case 8:
        {
          switch(calculationMode)
          {
            case SHOOTING_INTERVAL:
            {
              runShootingInterval();   
              Serial.println("hello");
            }
            break;
            case CLIP_LENGTH:
            {
              
            }
            break;
            case EVENT_DURATION:
            {
              
            }
            break;
          }
        }
        break;
      }
    }
    break;
  }

  if(subMenu1Page != subMenu1PageOld)
  {
    subMenu1Display();
    subMenu1PageOld = subMenu1Page;
  }
  if(sliderMotion != sliderMotionOld)
  {
    subMenu1Display();
    sliderMotionOld = sliderMotion;
  }
  if(calculationMode != calculationModeOld)
  {
    subMenu1Display();
    calculationModeOld = calculationMode;
  }
  if(clipLength != clipLengthOld)
  {
    subMenu1Display();
    clipLengthOld = clipLength;
  }
  if(eventDuration != eventDurationOld)
  {
    subMenu1Display();
    eventDurationOld = eventDuration;
  }
  if(shootingInterval != shootingIntervalOld)
  {
    subMenu1Display();
    shootingIntervalOld = shootingInterval;
  }
  if(frameRate != frameRateOld)
  {
    subMenu1Display();
    frameRateOld = frameRate;
  }
  if(imageSize != imageSizeOld)
  {
    subMenu1Display();
    imageSizeOld = imageSize;
  }
}

void navigateSubMenu2()
{ 
  switch(lastButtonPushed)
  {
    case JOYSTICK_X1_LEFT:
    {
      subMenu2Page--;
      if(subMenu2Page == 0)
      {
        subMenu2Page = minSubMenu2Page;
      }  
    }
    break;
    case JOYSTICK_X1_RIGHT:
    {
      subMenu2Page++;
      if(subMenu2Page > maxSubMenu2Page)
      {
        subMenu2Page = maxSubMenu2Page;
      }  
    }
    break;
    case JOYSTICK_Y1_DOWN:
    {
      switch(subMenu2Page)
      {
        case 1:
        {
          videoMode--;
          if(videoMode == 0)
          {
            videoMode = maxVideoMode;
          }
        }
        break;
        case 2:
        {
          switch(videoMode)
          {
            case 1:
            {
              sliderMotion--;
              if(sliderMotion == 0)
              {
                sliderMotion = maxSliderMotion;
              }  
            }
            break;
            case 2:
            {
              
            }
            break;
          }
        }
        break;
        case 3:
        {
          switch(videoMode)
          {
            case 1:
            {
              sliderSpeed--;
              if(sliderSpeed == 0)
              {
                sliderSpeed = minSliderSpeed;
              }  
            }
            break;
            case 2:
            {
              
            }
            break;
          }
        }
        break;
      }
    }
    break;
    case JOYSTICK_Y1_UP:
    {
      switch(subMenu2Page)
      {
        case 1:
        {
          videoMode++;
          if(videoMode > maxVideoMode)
          {
            videoMode = 1;
          }
        }
        break;
        case 2:
        {
          switch(videoMode)
          {
            case 1:
            {
              sliderMotion++;
              if(sliderMotion > maxSliderMotion)
              {
                sliderMotion = 1;
              }  
            }
            break;
            case 2:
            {
              
            }
            break;
          }
        }
        break;
        case 3:
        {
          switch(videoMode)
          {
            case 1:
            {
              sliderSpeed++;
              if(sliderSpeed > maxSliderSpeed)
              {
                sliderSpeed = maxSliderSpeed;
              }  
            }
            break;
            case 2:
            {
              
            }
            break;
          }
        }
        break;
      }
    }
    break;
  }

  if(subMenu2Page != subMenu2PageOld)
  {
    subMenu2Display();
    subMenu2PageOld = subMenu2Page;
  }
  if(videoMode != videoModeOld)
  {
    subMenu2Display();
    videoModeOld = videoMode;
  }
  if(sliderMotion != sliderMotionOld)
  {
    subMenu2Display();
    sliderMotionOld = sliderMotion;
  }
  if(sliderSpeed != sliderSpeedOld)
  {
    subMenu2Display();
    sliderSpeedOld = sliderSpeed;
  }
}

void runShootingInterval()
{
  sliderCalibration();

  finalCalculations();
  
  digitalWrite(STEPPER_ENABLE, HIGH);
  digitalWrite(STEPPER_DIRECTION, HIGH);

  while(photoCounter < numberPhotos)
  {
    while(stepCounter < stepsPerPhoto)
    {
      digitalWrite(STEPPER_PULSE, HIGH);
      delayMicroseconds(1000);
      digitalWrite(STEPPER_PULSE, LOW); 
      delayMicroseconds(1000);

      stepCounter = stepCounter + 1;
    }
    cumulativeStepCounter = (cumulativeStepCounter + stepCounter);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Interval Time");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(90,0);
    display.println(calcShootingInterval);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,10);
    display.println("Photos Taken");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(90,10);
    display.println(photoCounter);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,20);
    display.println("Total Photos");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(90,20);
    display.println(numberPhotos);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,35);
    display.println("Step Size");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(90,35);
    display.println(stepsPerPhoto);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,45);
    display.println("Steps Taken");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(90,45);
    display.println(cumulativeStepCounter);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,55);
    display.println("Total Steps");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(90,55);
    display.println(totalSteps);
    
    display.display();
    display.clearDisplay();
    
    delay(calcShootingInterval*1000);
    stepCounter = 0;

    photoCounter = photoCounter + 1;
  }
  digitalWrite(STEPPER_ENABLE, LOW);
}

void finalCalculations()
{
  stepsPerPhoto = (totalSteps/numberPhotos);  
}

void sliderCalibration()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(31,0);
  display.println("CALIBRATING");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,20);
  display.println("DETERMINING ORIGIN");
  display.display();
  delay(100);
  display.clearDisplay(); 

  switch(sliderMotion)
  {
    case L2R:
    {
      digitalWrite(STEPPER_ENABLE, HIGH);
      digitalWrite(STEPPER_DIRECTION, LOW);
      
      //limitReading = digitalRead(LIMIT_LEFT); 
      limitReading = digitalRead(JOYSTICK_Z2);
      while(limitReading != 0)
      {
        //Read The State Of The Button Into A Local Variable
        //limitReading = digitalRead(LIMIT_LEFT); 
        limitReading = digitalRead(JOYSTICK_Z2);

        //Move Slider To The Origin (To The Left)
        digitalWrite(STEPPER_PULSE, HIGH);
        delayMicroseconds(250);
        digitalWrite(STEPPER_PULSE, LOW); 
        delayMicroseconds(250);
      }
      delay(500);
      
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(31,0);
      display.println("CALIBRATING");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10,20);
      display.println("CALCULATING LENGTH");
      display.display();
      display.clearDisplay();

      digitalWrite(STEPPER_DIRECTION, HIGH);
      
      //limitReading = digitalRead(LIMIT_RIGHT);
      limitReading = digitalRead(JOYSTICK_Z2);
      while(limitReading != 0)
      {
        //Read The State Of The Button Into A Local Variable
        //limitReading = digitalRead(LIMIT_RIGHT);
        limitReading = digitalRead(JOYSTICK_Z2); 

        //Move Slider To The RightTo Count The Amount Of Steps
        digitalWrite(STEPPER_PULSE, HIGH);
        delayMicroseconds(250);
        digitalWrite(STEPPER_PULSE, LOW); 
        delayMicroseconds(250); 

        totalSteps = totalSteps + 1;
      }

      delay(500);
      sliderStepDistance = totalSteps;

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(31,0);
      display.println("CALIBRATING");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(7,20);
      display.println("RETURNING TO ORIGIN");
      display.display();
      display.clearDisplay();

      digitalWrite(STEPPER_DIRECTION, LOW);

      while(sliderStepDistance != 0)
      {
        //Move Slider Back To The Origin Using Total Number Of Steps
        digitalWrite(STEPPER_PULSE, HIGH);
        delayMicroseconds(250);
        digitalWrite(STEPPER_PULSE, LOW); 
        delayMicroseconds(250); 

        sliderStepDistance = sliderStepDistance - 1;
      }
      
      digitalWrite(STEPPER_ENABLE, LOW);

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(31,0);
      display.println("CALIBRATING");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(40,20);
      display.println("COMPLETE");
      display.display();
      delay(5000);
      display.clearDisplay();
    }
    break;
    case R2L:
    {

    }
    break;
  }
}


