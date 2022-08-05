esp-debounce
============

inspired by an idea I found here

    * The simplest button debounce solution – E-Tinkers
    https://www.e-tinkers.com/2021/05/the-simplest-button-debounce-solution/

I wrote these couple of lines.

what is it
----------

- a small sketch for ESP8266/ESP32 to debounce a key the most optimal way with minimum effort

features
--------

- adjustable rate to take samples for a key status
- adjustable over all time to debounce a key (or to ignore it after all)
- CPU resources claimed only if a key is pressed/released
- key press and release status available

