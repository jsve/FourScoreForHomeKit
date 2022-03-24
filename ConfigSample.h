/* This config sample is not read by the application. Save it as Config.h
 * and update values below as needed.
 */
#ifndef CONFIG_H
#define CONFIG_H

#define LOG_LEVEL 0 // either 0, 1, 2 where 2 is the most verbose (prints LOG1 and LOG2 commands)

/*
 * credentials and passwords
 */
#define PAIRING_CODE "78932190" //must be exactly eight numerical digits (no dashes)
#define WIFI_SSID "some_ssid" // you can leave these out and use enableAutoStartAP() in startup if you don't have the credentials
#define WIFI_PASS "some_pass"


/*
 * params to configure the device and how it is presented to HomeKit
 */
#define SAMPLE_INTERVAL 1000; // sample channel every x ms
#define NUMBER_OF_PLAYERS 6; // aka. keys
#define DEFAULT_PLAYER_NAMES {"Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6"}; // ordered as controller pin (staggered counted from top left on female) connected for the player (key)
#define DEFAULT_CONTROLLER_PORT_NAMES {"Port 1", "Port 2", "Port 3", "Port 4"};
// NOTE: button-names are configured directly in the buttonFactory()
#define USE_BUTTONS true; // if buttons should be exposed to HomeKit
#define ACCESSORY_MANUFACTURER "Nontendo"
#define ACCESSORY_SERIAL_NUMBER "FCKH-001"
#define ACCESSORY_VERSION "1.0"


/*
 * ESP configuration (pins, measurements and math)
 */
#define ANALOGUE_READ_GPIO_PINS {34, 35, 32, 33}; // order from back of card (opposite USB). Only first is used when multiplexer is used.
#define ANALOGUE_WRITE_GPIO_PINS {25, 26}; // order from back of card (opposite USB). [0] controls bit 0 and [1] controls bit 1 in multiplexing.

/* the values for player identification are accepted if they differ by
 * the PLAYER_READING_VARIANCE (+-). The value should be small enough to
 * not trigger other players, but large enought to account for signal
 * interference and differences between components. Higher
 * NUMBER_OF_READINGS_FOR_AVERAGE means that the variance can be somewhat
 * reduced, since interference is "filtered" but means more calculations
 * and any disadvantages that that brings.
 */
#define NUMBER_OF_READINGS_FOR_AVERAGE 75;
#define INTERFERENCE_READING 100; // the highest reading when channel in multiplex is closed. Needs to be above the lowest relevant reading since these are ignored.
#define READING_VARIANCE 15; // different ports using different resistors might vary a little.

#define GPIO_READINGS {3775, 3274, 2983, 2730, 2535, 2354}; // readings for "player" 1-6. for buttons [0] means not pressed and [1] is pressed.

#endif