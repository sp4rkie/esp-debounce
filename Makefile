ifeq ($(MAKECMDGOALS),esp-1)
#   CHIP = esp8266 #(default)
    BOARD = d1_mini
    UPLOAD_PORT = /dev/ttyUSB3
    OTA_ADDR = $(MAKECMDGOALS)
    esp-debounce.ino.cpp_CFLAGS = -DESP_1 -fpermissive
else ifeq ($(MAKECMDGOALS),esp32-0)
    CHIP = esp32
    BOARD = d1_mini32
    UPLOAD_PORT = /dev/ttyUSB3
    OTA_ADDR = $(MAKECMDGOALS)
    esp-debounce.ino.cpp_CFLAGS = -DESP32_0 -fpermissive
else ifeq ($(MAKECMDGOALS),clean)
else
    $(error no valid option given)
endif

#ESP_ROOT=$(HOME)/.arduino15/packages/esp8266/hardware/esp8266/3.0.2/       # default
#ESP_ROOT=$(HOME)/.arduino15/packages/esp32/hardware/esp32/2.0.4/           # default
#SKETCH = $(ESP_ROOT)/libraries/Ticker/examples/TickerBasic/TickerBasic.ino
#SKETCH = $(HOME)/Arduino/pq/pq.ino                                         # SKETCH not required if in current dir
#EXCLUDE_DIRS = $(HOME)/Arduino/libraries/TFT_eSPI
#BUILD_DIR = /tmp/mkESP                                                     # default
#BUILD_EXTRA_FLAGS =                                                        # overrides all (because it's issued last)

#USE_CCACHE = 0     # massive speed up if active (0: disable, 1: enable (default))
#VERBOSE = 1        # massive debug (0: disable (default), 1: enable)
UPLOAD_SPEED = 921600

include $(HOME)/makeEspArduino/x.mk

esp-1: FRC run
#esp-1: FRC ota
esp32-0: FRC run
#esp32-0: FRC ota

# must recompile it always since last flags were unknown
FRC:
	touch esp-debounce.ino
