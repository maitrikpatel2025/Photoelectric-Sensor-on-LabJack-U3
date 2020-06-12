//Program: Final_Project
//Author:   Patel, Maitrik
//Date: 7-April-2020
//Purpose: Optical Sensor Alarm 

#include <stdio.h> 
#include <Windows.h> 
#include <stdlib.h> 
#include "c:\program files (x86)\labjack\drivers\LabJackUD.h"

int main() {

	// Section 1 – Set up variables for the program. 

		LJ_ERROR lj_cue;
		LJ_HANDLE lj_handle = 0;
		int counter = 0;
		double dblValueAIN0 = 0,  dblValueFIO4 = 0;

    // Section 2 – Open a handle, then reset all ports on the LabJack U3. 

		lj_cue = OpenLabJack(LJ_dtU3, LJ_ctUSB, "1", 1, &lj_handle);
		lj_cue = ePut(lj_handle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);
		lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0, 0);

	// Section 4 - ADD: Use the 48 MHz timer clock base, with divisor enabled.
		lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_tc48MHZ_DIV, 0, 0);

	// Section 5 - ADD: Set the actual timer clock. (= 48 MHz / divisor)
		lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, 48, 0, 0);

	// Section 6 - ADD: Enable the timer.
		lj_cue = AddRequest(lj_handle, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 1, 0, 0);

	// Section 7 - ADD: Configure the timer to 8-bit PWM. (Freq = timer clock / 256)
		lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_MODE, 0, LJ_tmPWM8, 0, 0);

	// Section 8 - GO: Complete all requests up to this point.
		lj_cue = Go();


	while (counter < 60) {
		lj_cue = AddRequest(lj_handle, LJ_ioGET_AIN, 0, 0, 0, 0);
		lj_cue = Go();
		lj_cue = GetResult(lj_handle, LJ_ioGET_AIN, 0, &dblValueAIN0);
		printf("AIN0 = %lf\n", dblValueAIN0);

		// Section 9 - If the sensor is not interrupted 
		if (dblValueAIN0 <= 3.0) {

			// Section 10 - Turn of the buzzer
			lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 0, 0, 0);
			lj_cue = Go();
			
			// Section 11 - Turn off the LED
			lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 0, 0, 0);
			lj_cue = Go();

		}

		//  Section 12 - If sensor interrupted
		else if (dblValueAIN0 > 4.0) {

			// Section 13 - Turn on the Led 
			lj_cue = AddRequest(lj_handle, LJ_ioPUT_DAC, 0, 4.0, 0, 0);
			lj_cue = Go();

			// Section 14 - Turn on the buzzer 
			lj_cue = AddRequest(lj_handle, LJ_ioPUT_TIMER_VALUE, 0, 32768, 0, 0);
			lj_cue = Go();

		}

		// Section 15 - Delay for 0.25 Seconds 
		Sleep(250);
		counter++;

	}



	// Section 16 – Pause the system and wait for user to respond. 

		system("pause");

	// Section 17 – Reset all pins to factory settings, reset FIO4 back to an input, then exit. 

		lj_cue = ePut(lj_handle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);
		lj_cue = eGet(lj_handle, LJ_ioGET_DIGITAL_BIT, 4, 0, 0);

		Close();

		return 0;

}

