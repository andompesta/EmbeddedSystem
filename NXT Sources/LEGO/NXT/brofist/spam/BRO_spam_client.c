#include <stdio.h>
#include "headers/BRO_spam_fists.h"
#include "headers/BRO_spam_client.h"
#include "../my_settings.h"
#include <math.h>

#include "ecrobot_interface.h"
#include "kernel.h"

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(SysTimerCnt);
DeclareResource(lcd);
DeclareTask(BRO_Comm);
DeclareTask(PID_Controller);
DeclareTask(DisplayTask);

engines_t engines;

/**
 * NOTE: user-defined settings
 * NOTE: changed also file bro_fist.h
 */


bro_fist_t out_packet_buffer[BUFFER_SIZE];
int out_packet_buffer_index = 0;
float start_time = -1;
int turned_down = 0;
int count_experiment = 1;
int max_omega = 1000 * (PI) / (MILLIS_UPDATE);
float test_target_power = MIN_TARGET_POWER;


/*--------------------------------------------------------------------------*/
/* LEJOS OSEK hooks                                                         */
/*--------------------------------------------------------------------------*/
void ecrobot_device_initialize()
{
    ecrobot_init_bt_slave("1234");
    
    memset(&engines, 0, sizeof(engines_t));

    engines.first.port = NXT_PORT_A;
    engines.first.speed_control_type = NOT_USING;
    engines.first.speed_ref = 0;

    engines.second.port = NXT_PORT_B;
    engines.second.speed_control_type = NOT_USING;
    engines.second.speed_ref = 0;

    engines.third.port = NXT_PORT_C;
    engines.third.speed_control_type = NOT_USING;
    engines.third.speed_ref = 0;

    // Reset Memory
    memset (out_packet_buffer, 0, sizeof(bro_fist_t) * BUFFER_SIZE);

    if (CONN_SONAR) {
        ecrobot_init_sonar_sensor(SONAR_PORT);
    };
    if (CONN_LIGHT) {
        ecrobot_set_light_sensor_active(LIGHT_PORT);
    };
}


void ecrobot_device_terminate()
{
  
    memset(&engines, 0, sizeof(engines_t));

    nxt_motor_set_speed(NXT_PORT_A, 0, 1);
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
        
    ecrobot_set_light_sensor_inactive(LIGHT_PORT);
    ecrobot_term_sonar_sensor(SONAR_PORT);

    bt_reset();

    ecrobot_term_bt_connection();
}

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(void)
{
    StatusType ercd;

    /*
     *  Increment OSEK Alarm System Timer Count
    */
    ercd = SignalCounter( SysTimerCnt );
    if ( ercd != E_OK ) {
        ShutdownOS( ercd );
    }
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : PID_Controller                                                 */
/* Priority: 3                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: preemptive                                                     */
/*--------------------------------------------------------------------------*/
//TASK(PID_Controller)
//{
//    /*  We will now update the powers for each motor.
//     *  If a motor is set as RAW_POWER then the first "powers" value is used
//     *  directly for the speed update.
//     *  Oblviously it will be possible for the users to not install a motor,
//     *  so we won't do anything with the ports on which there is nothing.
//     */
//
//    if (engines.first.speed_control_type != NOT_USING) {
//        //  Now we will set the powers using the data inside the motor's
//        //  structure.
//        nxt_motor_set_speed(engines.first.port, engines.first.powers[0], 1);
//    };
//
//    //  Doing the same thing for the second motor
//    if (engines.second.speed_control_type != NOT_USING) {
//        nxt_motor_set_speed(engines.second.port, engines.second.powers[0], 1);
//    };
//
//    //  And, guess what? We are doing it even for the third motor.
//    if (engines.third.speed_control_type != NOT_USING) {
//        nxt_motor_set_speed(engines.third.port, engines.third.powers[0], 1);
//    };
//
//    TerminateTask();
//}

TASK(BRO_Comm)
{
	// Variables
	int operation_t = WASTING_TIME;
	float time_f = 0;
	float omega_f = 0;
	float target_power_f = 0;
	// Determine Time
	float current_time = ((float) systick_get_ms()) / 1000;
	float current_rot = (float) nxt_motor_get_count(TARGET_PORT);
	if (start_time == -1) {
		start_time = current_time; // ((float) systick_get_ms()) / 1000;
	}
	time_f = current_time - start_time;
	// Determine Behaviour
	if ((turned_down == 0) && ((time_f) <= LENGTH_WORKING_CYCLE))
	{
		operation_t = DATA_COLLECTION;
		// Active Cycle
		switch(INPUT_TYPE)
		{
			case STEP_FUNCTION:
			{
				if (count_experiment <= NUM_TESTS)
				{
					target_power_f = test_target_power;
					break;
				} else {
					target_power_f = 0;
				}
			}
			case SINUSOID:
			{
				if (count_experiment <= NUM_TESTS) {
					// WARNING: this formula frequency samples distribution is poor
					//omega_f = 2 * PI * powf(count_experiment, 1.50) / (float) LENGTH_WORKING_CYCLE;
					omega_f = 2 * PI * (MIN_TARGET_FREQUENCY
						+ (count_experiment - 1) * (MAX_TARGET_FREQUENCY - MIN_TARGET_FREQUENCY) / NUM_TESTS);
					target_power_f = (test_target_power) * sinf(omega_f * time_f);
					if (omega_f > max_omega)
						target_power_f = 0;
				} else {
					target_power_f = 0;
				}
				break;
			}
			default:
			{
				target_power_f = 0;
				break;
			}
		}
	} else {
		if (turned_down == 0) {
			turned_down = 1;
			time_f = 0;
			start_time = -1;
		}
		// Force Motor Count to Reset
		nxt_motor_set_count(TARGET_PORT, 0);
		current_rot = -1;
	}
	// Set Outgoing packet
	out_packet_buffer[out_packet_buffer_index].port = TARGET_PORT;
	out_packet_buffer[out_packet_buffer_index].operation = operation_t;
	out_packet_buffer[out_packet_buffer_index].data = current_rot;
	out_packet_buffer[out_packet_buffer_index].time = time_f;
	out_packet_buffer[out_packet_buffer_index].omega = omega_f;
	out_packet_buffer[out_packet_buffer_index].target_power = target_power_f;
	// Increase Buffer Index, if out of bounds then send the data to the PC
	++out_packet_buffer_index;
	// Send the Packet if the Buffer is Full
	if (out_packet_buffer_index == BUFFER_SIZE) {
		// Send Data only if it has a meaning to do so;
		if (count_experiment <= NUM_TESTS)
		{
			bt_send((U8*) out_packet_buffer, sizeof(bro_fist_t) * BUFFER_SIZE);
		}
		out_packet_buffer_index = 0;
		memset (out_packet_buffer, 0, sizeof(bro_fist_t) * BUFFER_SIZE);
	}
	// Set LCD content
	// Warning: when you enable this code, comment the first line in DISPLAY_TASK
//	display_clear(0);
//	display_goto_xy(0,0);
//	display_string("Time:");
//	display_goto_xy(0,1);
//	display_int((int)time_f, 0);
//	display_goto_xy(0,2);
//	display_string("Sin:");
//	display_goto_xy(0,3);
//	display_int((int)sin_f, 0);
//	display_goto_xy(0,4);
//	display_string("Target Speed:");
//	display_goto_xy(0,5);
//	display_int((int)target_power_f, 0);
//	display_update();
	// Set Motor Speed
	switch(INPUT_TYPE) {
		case STEP_FUNCTION:
			if (time_f == 0) { // SET the SPEED only ONCE
				nxt_motor_set_speed(TARGET_PORT, target_power_f, BRAKE_SETTING);
			}
			break; // Otherwise we don't change it!
		case SINUSOID:
			nxt_motor_set_speed(TARGET_PORT, target_power_f, BRAKE_SETTING);
			break;
	}
	// Start New Good Cycle
	if ((turned_down == 1) && (time_f >= LENGTH_EMPTY_CYCLE))
	{
		// Start a new Data Collection Cycle
		start_time = -1;
		turned_down = 0;
		// Increment Counter
		++count_experiment;
		if ((NUM_TESTS + 1) == count_experiment)
		{
			test_target_power = test_target_power + INTERVAL_TARGET_POWER;
			if (test_target_power <= MAX_TARGET_POWER)
			{
				count_experiment = 1; // RESET the counter
			} else {
				// else: send out the current buffer!
				if (out_packet_buffer_index > 0)
				{
					bt_send((U8*) out_packet_buffer, sizeof(bro_fist_t) * BUFFER_SIZE);
				}
				// then it will stop sending data :)
			}
		} // else: do a new test with the same target_power settings
	}
	TerminateTask();
}

TASK(DisplayTask)
{
    ecrobot_status_monitor("BROFist Client");
    TerminateTask();
}
