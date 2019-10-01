/*
   BittyButton.h - Arduino Library to easily use one or more bit-stream debounced momentary switch objects
   Created by Arnold Martin, August 28, 2019.
   
   Based in part on Elliot Williams excellent article on debouncing noisy buttons:
   https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/

   Released into the wild knowing this has been done at least 1.6 millions times...
   Attribuation, share-alike
   
   V2

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

#ifndef BittyButton_h
#define BittyButton_h

#define LONG_PUSH   2
#define SHORT_PUSH  1
#define NO_PUSH     0

class BittyButton
{
  public:
    BittyButton();
    void attach(int pin);
    int refresh(int threshold = 0);
    bool isDown();
    bool isUp();
    bool shortPush;
    bool longPush;
  private:
    bool down();
    bool up();
    int _pin;
    int _threshold;
    uint16_t _buttonBits;
    bool _buttonRead;
    unsigned long _timeStart;
    unsigned long _pushTime;
    bool _shortPush;
    bool _pushed;
    bool _released;
};

#endif
