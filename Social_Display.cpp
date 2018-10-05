#include "Social_Display.h"        //Header file of library.

#include <Wire.h>                 //Arduino Wire library.
#include <Adafruit_GFX.h>         //Adafruit GFX library.
#include <Adafruit_IS31FL3731.h>  //Library for IS31FL3731 LED Matrix Driver.
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

Adafruit_IS31FL3731 matrixL = Adafruit_IS31FL3731(); //LED Matrix IC on the left side of board.
Adafruit_IS31FL3731 matrixR = Adafruit_IS31FL3731(); //LED Matrix IC on the right side of board.
Ticker updateLED = Ticker();                                     //Constructor for timmer that refreshes display.

Social_Display::Social_Display() {
}

//--------------------------------------LOW LEVEL FUNCTIONS FOR DISPLAYING A MESSAGE--------------------------------------
void writeMessage() {
  //This part of code scrolls message on display (also sends data to display).
  if (_position < _scroll) {
     _position = 32;                                   //If text has come to end, bring it back to start.
     if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
     _messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
  }
  
  if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
    updateLED.detach();
  }
  
  matrixL.setCursor(_position, 0);            //Set cursor at the start of the display.
  matrixL.print(msgBuffer);                   //Send text to the left side of display (becouse display is made form 2 seperate LED matrix drivers that are not chained together).
  matrixR.setCursor(_position - 16, 0);       //Set cursor offseted by the size of first display driver (on the left side).
  matrixR.print(msgBuffer);                   //Send text to the right side of display.
  for (int i = 0; i < _step / 6; i++); {      //If scroll step is too big, part of previous screen stays. So, we put some spaces at the end of string just to erase it. :)
    matrixL.print(" ");
    matrixR.print(" ");
  }
  _position -= _step;                         //Increment counter for scrolling.
}

void writePicture () {
  if (_position < _scroll) {
    _position = 32;                                 //If picture has come to the end of screen, bring it back to start.
    _messageRepeats++;                              //And also, increment variable that keeps track of how many times picture has been repeated on display.
  }
  
  matrixL.drawBitmap(_position + _step, 0, pictBuffer, 8, 8, _backBrightness);     //Erase previous position of picture on both displays making that picture same level of brightness as background.
  matrixR.drawBitmap(_position + _step - 16, 0, pictBuffer, 8, 8, _backBrightness);
  matrixL.drawBitmap(_position, 0, pictBuffer, 8, 8, _brightness);                 //Write picture to dispaly. Watchout! Picture that has to be written to right side of matrix should be shifted by the size of left LED matrix coltroler.
  matrixR.drawBitmap(_position - 16, 0, pictBuffer, 8, 8, _brightness);
  _position -= _step;                                                              //Increment counter for scrolling.                   
}

void writeTextAndPic() {
  if (_position < _scroll) {
     _position = 32;                                   //If text has come to end, bring it back to start.
     if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
     _messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
  }
  
  if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
    updateLED.detach();
  }
  
  matrixL.setCursor(_position, 0);            //Set cursor at the start of the display.
  matrixL.print(msgBuffer);                   //Send text to the left side of display (becouse display is made form 2 seperate LED matrix drivers that are not chained together).
  matrixR.setCursor(_position - 16, 0);       //Set cursor offseted by the size of first display driver (on the left side).
  matrixR.print(msgBuffer);                   //Send text to the right side of display.
  for (int i = 0; i < _step / 6; i++); {      //If scroll step is too big, part of previous screen stays. So, we put some spaces at the end of string just to erase it. :)
    matrixL.print(" ");
    matrixR.print(" ");
  }

  for (int i = 0; i < noOfPics; i++) {
    matrixL.drawBitmap(_position + _step + posBufferX[i], posBufferY[i], picTxtBuffer[i], 8, 8, _backBrightness);           //Erase previous position of picture on both displays making that picture same level of brightness as background.
    matrixR.drawBitmap(_position + _step - 16 + posBufferX[i], posBufferY[i], picTxtBuffer[i], 8, 8, _backBrightness);
    matrixL.drawBitmap(_position + posBufferX[i], posBufferY[i], picTxtBuffer[i], 8, 8, _brightness);                       //Write picture to dispaly. Watchout! Picture that has to be written to right side of matrix should be shifted by the size of left LED matrix coltroler.
    matrixR.drawBitmap(_position - 16 + posBufferX[i], posBufferY[i], picTxtBuffer[i], 8, 8, _brightness);                  //And also shift it by position of the desiered location of picture.
  }

  _position -= _step;
}

//--------------------------------------FUNCTIONS FOR INIT--------------------------------------

int Social_Display::begin() {
    _brightness = 64;                             //Set default levels for font brightness, background brightness, delay between scrolls and scroll step.
    _backBrightness = 2;
    _pause = 150;
    _step = 2;
    return initLEDMatrix();                      //Set up everything for our board and return value (1 - Success, 0- Fail).
}

int Social_Display::initLEDMatrix() {
  Wire.begin(4, 5);                              //Set up SDA ands SCL Pins.
  twi_setClock(400000);                          //Set maximum possible clock for this LED controller (400kHz).      
  if (!matrixL.begin(0x74)) return 0; //Initalize library for left LED matrix driver.
  if (!matrixR.begin(0x77)) return 0; //Initalize library for right LED matrix driver.
  matrixL.clear();                    //Clear everything that is in RAM od LED matrix Driver.
  matrixR.clear();
  matrixR.setTextWrap(false);         //Turn of text wrapin, we do not need that on this screen.
  matrixL.setTextWrap(false);
  matrixL.setTextColor(_brightness, _backBrightness);   //Setup font and background brightness.
  matrixR.setTextColor(_brightness, _backBrightness);
  matrixL.setRotation(0);                               //Set rotation to display messages and pictures horizontaly.
  matrixR.setRotation(0);
  return 1;                                             //Everything went ok? Return 1 for success!.      
}

void Social_Display::brightness(uint8_t _fontLight, uint8_t _backingLight) {
    _brightness = _fontLight;                               //If user want to change brightness of screen, they can use this function.
    _backBrightness = _backingLight;
    matrixL.setTextColor(_brightness, _backBrightness);
    matrixR.setTextColor(_brightness, _backBrightness);
}

//--------------------------------------FUNCTION FOR WRITING MESSAGE--------------------------------------
void Social_Display::message(char* msg, int _ms, int _stp, int _rep) {
  if(_ms < 10 || _stp < 1 || _rep < -1) return;   //It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, return.
  if(strlen(msg) > _BUFFERSIZE) return;   //If size of string is much bigger of size of buffer, do not do anything more, it will overflow.
  _step = _stp;                                   //Save what user want for delay and step for scrolling.
  _pause = _ms;
  _repeats = _rep;
  _dispMode = 1;
  
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  
  _messageRepeats = 0;                //If there is a new message, reset counter!
  _position = 32;                     //Calling this function means that user whats to write some new message, so bring scroll counter to start and clear buffer for message.
  
  for (int i = 0; i < _BUFFERSIZE; i++) {
    msgBuffer[i] = 0;
  }

  memcpy(msgBuffer, msg, strlen(msg));    //Copy new message to the buffer.
  _msgSize = strlen(msgBuffer);           //Calculate how many letters there is in new message (size of message).
  _scroll = ((-6) * _msgSize) - 1;        //Calculate how many steps of scrolling we have to make to scroll out whole message.
  

  updateLED.detach();                                 //Remove timer, just in case.
  
  if(_repeats) {                                        //If number of repeats is zero, that means that text is not scrolling, do not update screen.
    updateLED.attach_ms(_pause, writeMessage);          //Setup timer with new parameters for new message.
  }
  matrixL.drawLine(0, 8, 15, 8, _backBrightness);     //Draw line at the end of screen. This is because Adafruit library uses 8x6 pixels for fonts and height of out display is 9 pixels.
  matrixR.drawLine(0, 8, 15, 8, _backBrightness);
  if(!_repeats) {                                     //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
    _position = 0;                                    //In that case, set position to start of the screen.
    writeMessage();
  }
}

void Social_Display::stopScroll() {        //Function that stops message from scrolling (just remove timmer).
  updateLED.detach();
}

void Social_Display::resumeScroll() {         //Function that resume scrolling message on the screen.         
  if(_dispMode == 1) updateLED.attach_ms(_pause, writeMessage);
  if(_dispMode == 2) updateLED.attach_ms(_pause, writePicture);
  if(_dispMode == 3) updateLED.attach_ms(_pause, writePicture);
}

void Social_Display::deleteScroll() {                //Function removes message form display and memory.
  updateLED.detach();                                //Remove timmer (do not refresh display anymore).
    if(_dispMode == 1 || _dispMode == 3) {           //If it's mode 1 od 3 that means that we need to delete some text.
    for (int i = 0; i < 500; i++) {                  //Clean up buffer.
      msgBuffer[i] = 0;
    }
  }
  
  if(_dispMode == 2 || _dispMode == 3) {         //But, if we are in mode 2 or 3, that means we have some pictures to delete.
    pictBuffer = NULL;
    noOfPics = 0;
  }
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);    //Delete everything from screen by writing filled rect on screen that has same intensity as background.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
}

int Social_Display::repeatCount() {                  //Function retrun how many times message has been displayed and repeated. Useful when user want to know that message is displayed till the end.
  return _messageRepeats;  
}

//---------------------------------------FUNCTION FOR DISPLAYING PICTURES--------------------------------------

void Social_Display::picture(uint8_t* p, int posX, int posY) {
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);              //Delete everything from screen by writing filled rect on screen that has same intensity as background.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  matrixL.drawBitmap(posX, posY, p, 8, 8, _brightness);        //Write picture on screen (keeping on mind that picture has to be written on both LED Matrix drivers with offset on one).
  matrixR.drawBitmap(posX - 16, posY, p, 8, 8, _brightness);
}

void Social_Display::scrollPicture(uint8_t* p, int _ms, int _stp) {
  if(_ms < 10 || _stp < 1) return;                   //It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, return.
  int xSize = 8;                                     //Picture that has to be written on screen has to be 8x8 pixels.
  _dispMode = 2;
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);    //Delete everything from screen by writing filled rect on screen that has same intensity as background.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);
  _step = _stp;                                      //Save value for scroll step and delay between steps.
  _pause = _ms;

  _messageRepeats = 0;                               //If there is a new picture, reset counter!
  pictBuffer = p;                                    //Save picture address.
  _position = 32;                                //Set variable for current position on screen at start of screen.
  _scroll = -xSize - _step;
  updateLED.detach();                            //Remove timer, just in case.
  updateLED.attach_ms(_pause, writePicture);     //Setup timer with new parameters for new picture.
}


//--------------------------------------FUNCTIONS FOR DISPLAYING TEXT AND PICTURES AT THE SAME TIME--------------------------------------
void Social_Display::scrollTxtAndPics(char* txt, uint8_t** p, uint16_t* picsPos_x, uint16_t* picsPos_y, uint8_t n, int _ms, int _stp, int _rep) {
  if(_ms < 10 || _stp < 1 || _rep < -1 || n < 1) return;   //It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, return.
  if(strlen(txt) > _BUFFERSIZE) return;   //If size of string is much bigger of size of buffer, do not do anything more, it will overflow.
  _step = _stp;                                            //Save what user want for delay and step for scrolling.
  _pause = _ms;
  _repeats = _rep;
  _dispMode = 3;                                           //Set mode for displaying message and thex at the same time.     
  _messageRepeats = 0;                                     //If there is a new message, reset counter!
  int16_t _max;                                            //Variable that we are using for calculation of maximal scroll position.
  
  matrixL.fillRect(0, 0, 16, 9, _backBrightness);          //Delete everything from screen by writing filled rect on screen that has same intensity as background.
  matrixR.fillRect(0, 0, 16, 9, _backBrightness);

  _position = 32;                     //Calling this function means that user whats to write some new message, so bring scroll counter to start and clear buffer for message.
  for (int i = 0; i < _BUFFERSIZE; i++) {
    msgBuffer[i] = 0;
  }

  memcpy(msgBuffer, txt, strlen(txt));    //Copy new message to the buffer.
  _msgSize = strlen(msgBuffer);           //Calculate how many letters there is in new message (size of message).
  _scroll = ((-6) * _msgSize);            //Calculate how many steps of scrolling we have to make to scroll out whole message.

  _max = _scroll;                         //Now, we are trying to fing what needs longer scrolling, text or picture.

  for (int i = 0; i < n; i++) {
    if (-int16_t((picsPos_x[i] + 16)) < _max) _max = -int16_t((picsPos_x[i]) + 16);      //And how much exactly we have to scroll.
  }

  picTxtBuffer = p;                        //Just copy everything in library variables.
  posBufferX = picsPos_x;
  posBufferY = picsPos_y;
  noOfPics = n;                            //Determines how may pictures are in this text.
  _scroll = _max;

  updateLED.detach();                                      //Remove timer, just in case.
  if(_repeats) {                                           //If number of repeats is zero, that means that text is not scrolling, do not update screen.
    updateLED.attach_ms(_pause, writeTextAndPic);          //Setup timer with new parameters for new message.
  }
  
  if(!_repeats) {                                          //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
    _position = 0;                                         //In that case, set position to start of the screen.
    writeTextAndPic();
  }
}

//--------------------------------------FUNCTIONS FOR WEB--------------------------------------
int Social_Display::wifiNetwork(const char* _ssid, const char* _pass) {
    int retry = 10;                              //Number of retrys for connectin on WLAN network.
    WiFi.mode(WIFI_STA);                         //Setting up WiFi module mode.
    WiFi.begin(_ssid, _pass);                    //Try to connect to desiered network.
    do {                                         //Wait and keep checking if it's connected to network until we are really connected or we run out of retrys
       delay(250);
       retry--;
       } while (WiFi.status() != WL_CONNECTED && retry != 0);
    if(retry) {                                  //If we still have any more retrys left, that means we are successfuly connected to WiFi network.
       wlanSuccess = 1;
       return 1;                                 //Return 1 for successful conection to WiFi.
    }else{
       return 0;                                 //But if we are not connected, return 0 fro fail.
    }
}

int Social_Display::webPage(char* web, char* url, int port, int _ms, int _stp, int _rep) {
  if (!wlanSuccess) return 0;                          //If we are not connected to WiFi, return 0, becouse we can't loar web page if we do not have at least WiFi connection.
  if(_ms < 10 || _stp < 1) return 0;                   //It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, return.
  
  WiFiClient client;
  char webText[_BUFFERSIZE];                                           //Buffer for text that we downloaded from internet.
  uint8_t webSuccess = client.connect(web, port);                      //Try to connect to specified web client and specific port.
  if (webSuccess) {                                                    //If that was successful, try grabbing web page (or text from web page that will be shown on display).
    client.print(String("GET ") + String(url) + "\r\n\r\n\r\n");       //Setup a GET Requiest.
    while (client.connected()) {                                       //As longh as we have connection to client and as long as there is some data, keep saving that.
      if (client.available()) {
        String line = client.readStringUntil('\n');                    //Copy that into string.
        if(line.length() > _BUFFERSIZE) {
          client.stop();                                                     //Disconnect from client.
          return 0;
        }else{
          line.toCharArray(webText, line.length());                      //Copy that into buffer.
        }
      }
    }
    client.stop();                                                     //Disconnect from client.

    int webTextSize = strlen(webText);                                 //Calculate the lenght of string.
    //This part of code finds Croatian letters and change them into ASCII letters. Not really nesscery for this library.
    for (int16_t i = 0; i < webTextSize; i++) {
      if (webText[i] == 196 || webText[i] == 197) memmove(&webText[i], &webText[i + 1], webTextSize - i);
      if (webText[i] == 141 || webText[i] == 135) webText[i] = 'c';
      if (webText[i] == 161) webText[i] = 's';
      if (webText[i] == 190) webText[i] = 'z';
    }
    message(webText, _ms, _stp, _rep);        //Send message to display with desiered delay and step for scrolling
    return 1;                           //Everything went ok? Return 1 for success!
  } else {
    return 0;                           //Something went wrong? Send zero.
  }
}

int Social_Display::webPageText(char* web, char* url, int port, char* txt, int _n) {  //This function grabs textz form web page and saves it in user defined string that is send as function argument.
  if (!wlanSuccess) return 0;                                                         //Everything else is basicly the same as previous function, with only one exception, it does not write anything on dipslay. Useful if we want to parse something and than display it.
  WiFiClient client;
  uint8_t webSuccess = client.connect(web, port);
  if (webSuccess) {
    client.print(String("GET ") + String(url) + "\r\n\r\n\r\n");
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        line.toCharArray(txt, _n - 1);
      }
    }
    for (int16_t i = 0; i < _n; i++) {
      if (txt[i] == 196 || txt[i] == 197) memmove(&txt[i], &txt[i + 1], _n - i);
      if (txt[i] == 141 || txt[i] == 135) txt[i] = 'c';
      if (txt[i] == 161) txt[i] = 's';
      if (txt[i] == 190) txt[i] = 'z';
    }
    client.stop();
    return 1;
  } else {
    return 0;
  }
}
