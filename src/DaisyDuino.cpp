#include "DaisyDuino.h"

void DaisyHardware::Init(float control_update_rate, DaisyDuinoDevice device) {
  device_ = device;
  num_channels = 2;
  numControls = 0;
  numGates = 0;
  numSwitches = 0;
  numLeds = 0;

  switch (device) {
  case DAISY_SEED:
    break;
  case DAISY_POD:
	InitPod(control_update_rate);
    break;
  case DAISY_PETAL:
	InitPetal(control_update_rate);
    break;
  case DAISY_FIELD:
    InitField(control_update_rate);
    break;
  case DAISY_PATCH:
	InitPatch(control_update_rate);
	break;
	
  default:
    break;
  }
}

void DaisyHardware::InitPod(float control_update_rate){
    buttons[0].Init(control_update_rate, true, PIN_POD_SWITCH_1, INPUT_PULLUP);
    buttons[1].Init(control_update_rate, true, PIN_POD_SWITCH_2, INPUT_PULLUP);

    encoder.Init(control_update_rate, PIN_POD_ENC_A, PIN_POD_ENC_B,
                 PIN_POD_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);

    leds[0].Init(PIN_POD_LED_1_RED, PIN_POD_LED_1_GREEN, PIN_POD_LED_1_BLUE, true);
    leds[1].Init(PIN_POD_LED_2_RED, PIN_POD_LED_2_GREEN, PIN_POD_LED_2_BLUE,  true);

	controls[0].Init(PIN_POD_POT_1, control_update_rate);
	controls[1].Init(PIN_POD_POT_2, control_update_rate);

    numSwitches = numLeds = numControls = 2;
}

void DaisyHardware::InitPatch(float control_update_rate){
    encoder.Init(control_update_rate, PIN_PATCH_ENC_A, PIN_PATCH_ENC_B,
                 PIN_PATCH_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);
    
	gateIns[0].Init(20, INPUT, true);
    gateIns[1].Init(19, INPUT, true);
    numGates = 2;
    num_channels = 4;
	numControls = 4;
	
	controls[0].Init(PIN_PATCH_CTRL_1, control_update_rate, true);
	controls[1].Init(PIN_PATCH_CTRL_2, control_update_rate, true);
	controls[2].Init(PIN_PATCH_CTRL_3, control_update_rate, true);
	controls[3].Init(PIN_PATCH_CTRL_4, control_update_rate, true);
}

void DaisyHardware::InitPetal(float control_update_rate){
    numSwitches = 7;
    numLeds = 12;
    num_channels = 2;
	numControls = 6;

	//init the led driver
	//uint8_t   addr[2] = {0x00, 0x01};
    //I2CHandle i2c;
    //i2c.Init(petal_led_i2c_config);
    //led_driver_.Init(i2c, addr, petal_led_dma_buffer_a, petal_led_dma_buffer_b);
    //ClearLeds();
    //UpdateLeds();

	encoder.Init(control_update_rate, PIN_PETAL_ENC_A, PIN_PETAL_ENC_B,
                 PIN_PETAL_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);

	controls[0].Init(PIN_PETAL_POT_1, control_update_rate);
	controls[1].Init(PIN_PETAL_POT_2, control_update_rate);
	controls[2].Init(PIN_PETAL_POT_3, control_update_rate);
	controls[3].Init(PIN_PETAL_POT_4, control_update_rate);
	controls[4].Init(PIN_PETAL_POT_5, control_update_rate);
	controls[5].Init(PIN_PETAL_POT_6, control_update_rate);

	expression.Init(PIN_PETAL_EXPRESSION, control_update_rate);

	buttons[0].Init(control_update_rate, false, PIN_PETAL_SWITCH_1, INPUT_PULLUP);
	buttons[1].Init(control_update_rate, false, PIN_PETAL_SWITCH_2, INPUT_PULLUP);
	buttons[2].Init(control_update_rate, false, PIN_PETAL_SWITCH_3, INPUT_PULLUP);
	buttons[3].Init(control_update_rate, false, PIN_PETAL_SWITCH_4, INPUT_PULLUP);
	buttons[4].Init(control_update_rate, false, PIN_PETAL_SWITCH_5, INPUT_PULLUP);
	buttons[5].Init(control_update_rate, false, PIN_PETAL_SWITCH_6, INPUT_PULLUP);
	buttons[6].Init(control_update_rate, false, PIN_PETAL_SWITCH_7, INPUT_PULLUP);
}

void DaisyHardware::InitField(float control_update_rate){
	numSwitches = 2;
	numControls = 8;
	
	/*//pot MUX
	AdcChannelConfig adc_cfg;
	adc_cfg.InitMux({DSY_GPIOC, 0}, 8, 
		{DSY_GPIOA, 6}, 
		{DSY_GPIOA, 7}, 
		{DSY_GPIOB, 1});

	adc.Init(&adc_cfg, 5);

	// Order of pots on the hardware connected to mux.
    size_t pot_order[8] = {0, 3, 1, 4, 2, 5, 6, 7};
    for(size_t i = 0; i < 8; i++)
    {
        controls[i].InitMux(adc.GetMuxPtr(4, pot_order[i]), control_update_rate);
    }

    cv[0].InitBipolarCv(PIN_FIELD_ADC_CV_1, control_update_rate);
    cv[1].InitBipolarCv(PIN_FIELD_ADC_CV_2, control_update_rate);
    cv[2].InitBipolarCv(PIN_FIELD_ADC_CV_3, control_update_rate);
    cv[3].InitBipolarCv(PIN_FIELD_ADC_CV_4, control_update_rate);
	
	adc.Start();*/
}


void DaisyHardware::ProcessAnalogControls(){
	for(int i = 0; i < numControls; i++){
		controls[i].Process();
	}
	
	if(device_ == DAISY_PETAL){
		expression.Process();
	}
}

void DaisyHardware::ProcessDigitalControls(){
  if(device_ == DAISY_PATCH || device_ == DAISY_POD){
	encoder.Debounce();
  }

  for (int i = 0; i < numSwitches; i++) {
    buttons[i].Debounce();
  }
  
  for (int i = 0; i < numGates; i++) {
    gateIns[i].Debounce();
  }
}

void DaisyHardware::ProcessAllControls(){
	ProcessAnalogControls();
	ProcessDigitalControls();
}

//petal led setters
void DaisyHardware::SetRingLed(uint8_t idx, float r, float g, float b)
{
	if(idx < 0 || idx > 7 || device_ != DAISY_PETAL){
		return; //bad idx or wrong board
	}
	
    uint8_t r_addr[8] = {LED_RING_1_R,
                                     LED_RING_2_R,
                                     LED_RING_3_R,
                                     LED_RING_4_R,
                                     LED_RING_5_R,
                                     LED_RING_6_R,
                                     LED_RING_7_R,
                                     LED_RING_8_R};
    uint8_t g_addr[8] = {LED_RING_1_G,
                                     LED_RING_2_G,
                                     LED_RING_3_G,
                                     LED_RING_4_G,
                                     LED_RING_5_G,
                                     LED_RING_6_G,
                                     LED_RING_7_G,
                                     LED_RING_8_G};
    uint8_t b_addr[8] = {LED_RING_1_B,
                                     LED_RING_2_B,
                                     LED_RING_3_B,
                                     LED_RING_4_B,
                                     LED_RING_5_B,
                                     LED_RING_6_B,
                                     LED_RING_7_B,
                                     LED_RING_8_B};


    led_driver_.SetLed(r_addr[idx], r);
    led_driver_.SetLed(g_addr[idx], g);
    led_driver_.SetLed(b_addr[idx], b);
}

void DaisyHardware::SetFootswitchLed(uint8_t idx, float bright)
{
	if(idx < 0 || idx > 3 || device_ != DAISY_PETAL){
		return;
	}
	
    uint8_t fs_addr[4]
        = {LED_FS_1, LED_FS_2, LED_FS_3, LED_FS_4};
    led_driver_.SetLed(fs_addr[idx], bright);
}

void DaisyHardware::ClearLeds()
{
    for(size_t i = 0; i < 8; i++)
    {
        SetRingLed(i, 0.0f, 0.0f, 0.0f);
    }
    for(size_t i = 0; i < 4; i++)
    {
        SetFootswitchLed(i, 0.0f);
    }
}

void DaisyHardware::UpdateLeds()
{
    led_driver_.SwapBuffersAndTransmit();
}