/*
   BittyButton.cpp - Arduino Library for easy bitWise based debouncing of a momentary switch
   Created by Arnold Martin, August 28, 2019.
   Released into the wild
   Attribuation, share-alike
   V2

   Fairly slim profile at 136 bytes of program storage and 20 bytes for global variables used by the library's object instance

   Create an instance as usual with the constuctor, a pin number agument and an option for INPUT_PULLUP:

    BittyButton <instance name>;

   Initialize the object in the setup with the attach() method:

    <instance name>.attach(int <pin number>, int <option>)

    by default (without int <option>) the pin will initialize as a normal input
    with int <option> as the system definition INPUT_PULLUP it will initialize with pullup.

   Depending on how you use the following methods the button will have a different "feel"

    You can always "read" the pin and refresh with the refresh() method:

    <instance name>.refresh(int <optional press timer threshold>);

    optional int argument is a timer threshold to detect a long press
    including this argument will make the button feel like it activates on RELEASE (long press activates on release)
    without this argument the button will feel like it activates on PRESS (no long press capability)

   the refresh() method returns 3 possible int values with macro definitions (found in BittyButton.h)

    LONG_PUSH   2 (only with optional threshold argument)
    SHORT_PUSH  1
    NO_PUSH     0

   You can also use these PUBLIC bool varibles to test the button state after refresh() is called (redundant but more flexible):
    <instance name>.longPush
    <instance name>.shortPush

   You can also check to see if the button is fully depressed or fully released with these bool type methods
    <instance name>.isDown()
    <instance name>.isUp()
*/

#include "Arduino.h"
#include "BittyButton.h"

BittyButton::BittyButton()
{
}

void BittyButton::attach(int pin, int inputType, int longPushTime) //NEED TO ADD TWO ARGUMENTS: INPUT_PULLUP and LONG_PUSH_THRESHOLD
{
  _pin = pin;
  if (longPushTime >= 500)
  {
    _threshold = longPushTime;
  }
  else
  {
    _threshold = 0;
  }

  if (inputType == INPUT_PULLUP)
  {
    pinMode(_pin, INPUT_PULLUP);
  }
  else
  if (inputType == INPUT)
  {
    pinMode(_pin, INPUT);
  }
  else
  if (inputType == INPUT_PULLDOWN)
  {
    pinMode(_pin, INPUT_PULLDOWN);
  }

  int temp = digitalRead(_pin);
  // Serial.print("defaultPinRead = "); Serial.println(temp);
  if (temp == 0)
  {
    _downMatch = 0B0111111111111111;
    _upMatch = 0B1111111111111110;
    _isDownMatch = 0B1111111111111111;
    _isUpMatch = 0B0000000000000000;
  }
  else
  if (temp == 1)
  {
    _downMatch = 0B1000000000000000;
    _upMatch = 0B0000000000000001;
    _isDownMatch = 0B0000000000000000;
    _isUpMatch = 0B1111111111111111;
  }
}

void BittyButton::refresh(void)
{
  //read the button value into a local varible
  _buttonRead = digitalRead(_pin);

  //store the history of the button as a stream of bits (into an 16-bit byte data type)
  //bitShift the buffer value so 0B1111111111111111 becomes 0B111111111111110 if the button is pushed
  _buttonBits = _buttonBits << 1;

  //write the button value into the open (rightmost) bit
  _buttonBits = _buttonBits | _buttonRead;
  // Serial.println(_buttonBits);

  //needs a very quick delay (50 - 100 microseconds on UNO) to stabilize the buffer (too fast otherwise!)
  delayMicroseconds(100);

  if (_threshold >= 500) {
    if (down() || isDown() || up()) {

      _pushTime = millis() - _timeStart;

      if (_pushed) {
        if (_pushTime < _threshold && _released) {
          shortPush = true;
          _pushed = false;
          _released = false;
          return;
        }
        else if (_pushTime >= _threshold) {
          longPush = true;
          _pushed = false;
          _released = false;
          return;
        }
      }
    }
    else {
      shortPush = false;
      longPush = false;
      return;
    }
  }
  else
  {
    if (down()) {
      shortPush = true;
      _pushed = false;
      _released = false;
      return;
    }
    else {
      shortPush = false;
      longPush = false;
      return;
    }
  }
}


bool BittyButton::down()
{
  if (_buttonBits == _downMatch)
  {
    _timeStart = millis();
    _pushed = true;
    _released = false;
    return true;
  }
  else {
    return false;
  }
}

bool BittyButton::up()
{
  if (_buttonBits == _upMatch) {
    _released = true;
    return true;
  }
  else {
    return false;
  }
}

bool BittyButton::isDown() {
  if (_buttonBits == _isDownMatch) {
    return true;
  }
  else {
    return false;
  }
}

bool BittyButton::isUp()
{
  if (_buttonBits == _isUpMatch) {
    return true;
  }
  else {
    return false;
  }
}
