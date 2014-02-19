#ifndef __MY_SETTINGS_H__
#define __MY_SETTINGS_H__

/**
 * NOTE: edit this file to select your own settings to cllect the data
 */

/*--------------------------------------------------------------------------*/

// Target Motor
#define TARGET_PORT NXT_PORT_B
// Type of Input Signal
enum {STEP_FUNCTION = 1, SINUSOID = 2};
#define INPUT_TYPE STEP_FUNCTION
// SINUSOID: settings
#define PI (3.14159265359)

// NUM TESTS
#define NUM_TESTS 50
// i.e.: number of times it repeats a test with the same target POWER / frequency
// settings;

// TARGET POWER SETTINGS
// NOTE: for some reasons after some time the bluetooth connection
// may be lost.. do not attempt to launch too many trials at the same time!
#define MIN_TARGET_POWER 30
#define MAX_TARGET_POWER 100
#define INTERVAL_TARGET_POWER 10
// i.e.: repeat the selected test for each power starting from MIN_TARGET_POWER
// up to MAX_TARGET_POWER at intervals of INTERVA_TARGET_POWER

// TARGET FREQUENCY SETTINGS
#define MIN_TARGET_FREQUENCY 1
#define MAX_TARGET_FREQUENCY 121
// i.e.: determine the frequency at which the SINUSOID function is sampled:
// MIN_TARGET_FREQUENCY + (count_experiment - 1) * (MAX_TARGET_FREQUENCY - MIN_TARGET_FREQUENCY) / NUM_TESTS

// Should be always 1 `ON' (other valid value: 0 `OFF')
#define BRAKE_SETTING 1

// DATA TO BE SENT OUT
#define BUFFER_SIZE 5
// Time limit of a data collection
#define LENGTH_WORKING_CYCLE 5
// Time Limit After which the brick rests
#define LENGTH_EMPTY_CYCLE 1
// Time after which update the data
#define MILLIS_UPDATE 5
// WARNING: if you change here the millis_update field, you should change it also in the BRO_spam_client.oil file!

/*--------------------------------------------------------------------------*/

// OPERATION TYPES DEFINITIONS
#define DATA_COLLECTION 1
#define WASTING_TIME 2

#endif
