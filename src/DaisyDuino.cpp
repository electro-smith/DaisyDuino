#include "DaisyDuino.h"

   void DaisyHardware::Init(float control_update_rate, DaisyDuinoDevice device)
    {
	device_ = device;
	num_channels = 2;
	numGates = 0;
	
	switch (device){
	    case DAISY_SEED:
		break;
		
	    case DAISY_POD:
	        buttons[0].Init(control_update_rate, true, PIN_POD_SWITCH_1, INPUT_PULLUP);
	        buttons[1].Init(control_update_rate, true, PIN_POD_SWITCH_2, INPUT_PULLUP);
				
			encoder.Init(control_update_rate, PIN_POD_ENC_A, PIN_POD_ENC_B, PIN_POD_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);

			leds[0].Init(PIN_POD_LED_1_RED, PIN_POD_LED_1_GREEN, PIN_POD_LED_1_BLUE);    
	        leds[1].Init(PIN_POD_LED_2_RED, PIN_POD_LED_2_GREEN, PIN_POD_LED_2_BLUE);

			numSwitches = numLeds = 2;
			break;

	    case DAISY_PETAL:
		break;
	    
		case DAISY_FIELD:
		break;
	    
		case DAISY_PATCH:
			encoder.Init(control_update_rate, PIN_PATCH_ENC_A, PIN_PATCH_ENC_B, PIN_PATCH_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);
			gateIns[0].Init(20, INPUT, true);
			gateIns[1].Init(19, INPUT, true);
			numSwitches = numLeds = 0;
			numGates = 2;
			num_channels = 4;
		break;
	    default:
		break;
	}
    }

void DaisyHardware::DebounceControls()
    {
	for (int i = 0; i < numSwitches; i++)
	{
	    buttons[i].Debounce();
	}
	
		encoder.Debounce();
		for (int i = 0; i < numGates; i++)
		{
			gateIns[i].Debounce();
		}
	}