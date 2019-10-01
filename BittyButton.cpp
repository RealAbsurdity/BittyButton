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

void BittyButton::attach(int pin)
{
  _pin = pin;
  pinMode(_pin, INPUT_PULLUP);
}

int BittyButton::refresh(int threshold)
{
  _threshold = threshold;

  //read the button value into a local varible
  _buttonRead = digitalRead(_pin);

  //store the history of the button as a stream of bits (into an 16-bit byte data type)
  //bitShift the buffer value so 0B1111111111111111 becomes 0B111111111111110 if the button is pushed
  _buttonBits = _buttonBits << 1;

  //write the button value into the open (rightmost) bit
  _buttonBits = _buttonBits | _buttonRead;

  //needs a very quick delay (50 - 100 microseconds on UNO) to stabilize the buffer (too fast otherwise!)
  delayMicroseconds(100);

  if (_threshold > 0) {
    if (down() || isDown() || up()) {

      _pushTime = millis() - _timeStart;

      if (_pushed) {
        if (_pushTime < _threshold && _released) {
          shortPush = true;
          _pushed = false;
          _released = false;
          return SHORT_PUSH;
        }
        else if (_pushTime >= _threshold) {
          longPush = true;
          _pushed = false;
          _released = false;
          return LONG_PUSH;
        }
      }
    } else {
      shortPush = false;
      longPush = false;
      return NO_PUSH;
    }
  }
  else if (_threshold == 0) {
    if (down()) {
      shortPush = true;
      _pushed = false;
      _released = false;
      return SHORT_PUSH;
    } else {
      shortPush = false;
      longPush = false;
      return NO_PUSH;
    }
  }
}


bool BittyButton::down() {
  if (_buttonBits == 0B1000000000000000) {
    _timeStart = millis();
    _pushed = true;
    _released = false;
    return true;
  }
  else {
    return false;
  }
}

bool BittyButton::up() {
  if (_buttonBits == 0B0000000000000001) {
    _released = true;
    return true;
  }
  else {
    return false;
  }
}

bool BittyButton::isDown() {
  if (_buttonBits == 0B0000000000000000) {
    return true;
  }
  else {
    return false;
  }
}

bool BittyButton::isUp() {
  if (_buttonBits == 0B1111111111111111) {
    return true;
  }
  else {
    return false;
  }
}
