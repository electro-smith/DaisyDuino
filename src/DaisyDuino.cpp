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
  case DAISY_PATCH_SM:
    InitPatchSM(control_update_rate);
    break;
  default:
    break;
  }
}

void DaisyHardware::InitPod(float control_update_rate) {
  buttons[0].Init(control_update_rate, true, PIN_POD_SWITCH_1, INPUT_PULLUP);
  buttons[1].Init(control_update_rate, true, PIN_POD_SWITCH_2, INPUT_PULLUP);

  encoder.Init(control_update_rate, PIN_POD_ENC_A, PIN_POD_ENC_B,
               PIN_POD_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);

  leds[0].Init(PIN_POD_LED_1_RED, PIN_POD_LED_1_GREEN, PIN_POD_LED_1_BLUE,
               true);
  leds[1].Init(PIN_POD_LED_2_RED, PIN_POD_LED_2_GREEN, PIN_POD_LED_2_BLUE,
               true);

  controls[0].Init(PIN_POD_POT_1, control_update_rate);
  controls[1].Init(PIN_POD_POT_2, control_update_rate);

  numSwitches = numLeds = numControls = 2;
}

void DaisyHardware::InitPatch(float control_update_rate) {
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

  //cv outs
  pinMode(PIN_PATCH_CV_1, OUTPUT);
  pinMode(PIN_PATCH_CV_1, OUTPUT);
  pinMode(PIN_PATCH_GATE_OUT, OUTPUT);
}

void DaisyHardware::InitPetal(float control_update_rate) {
  numSwitches = 7;
  numLeds = 12;
  num_channels = 2;
  numControls = 6;

  // init the led driver
  uint8_t addr[2] = {0x00, 0x01};
  led_driver_.Init(addr, petal_led_dma_buffer_a, petal_led_dma_buffer_b);
  ClearLeds();
  UpdateLeds();

  encoder.Init(control_update_rate, PIN_PETAL_ENC_A, PIN_PETAL_ENC_B,
               PIN_PETAL_ENC_CLICK, INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLUP);

  controls[0].Init(PIN_PETAL_POT_1, control_update_rate);
  controls[1].Init(PIN_PETAL_POT_2, control_update_rate);
  controls[2].Init(PIN_PETAL_POT_3, control_update_rate);
  controls[3].Init(PIN_PETAL_POT_4, control_update_rate);
  controls[4].Init(PIN_PETAL_POT_5, control_update_rate);
  controls[5].Init(PIN_PETAL_POT_6, control_update_rate);

  expression.Init(PIN_PETAL_EXPRESSION, control_update_rate);

  buttons[0].Init(control_update_rate, true, PIN_PETAL_SWITCH_1, INPUT_PULLUP);
  buttons[1].Init(control_update_rate, true, PIN_PETAL_SWITCH_2, INPUT_PULLUP);
  buttons[2].Init(control_update_rate, true, PIN_PETAL_SWITCH_3, INPUT_PULLUP);
  buttons[3].Init(control_update_rate, true, PIN_PETAL_SWITCH_4, INPUT_PULLUP);
  buttons[4].Init(control_update_rate, true, PIN_PETAL_SWITCH_5, INPUT_PULLUP);
  buttons[5].Init(control_update_rate, true, PIN_PETAL_SWITCH_6, INPUT_PULLUP);
  buttons[6].Init(control_update_rate, true, PIN_PETAL_SWITCH_7, INPUT_PULLUP);
}

void DaisyHardware::InitField(float control_update_rate) {
  numSwitches = 2;
  num_channels = 2;

  buttons[0].Init(control_update_rate, false, PIN_FIELD_SWITCH_1, INPUT_PULLUP);
  buttons[1].Init(control_update_rate, false, PIN_FIELD_SWITCH_2, INPUT_PULLUP);

  // init the led driver
  uint8_t addr[2] = {0x00, 0x02};
  led_driver_.Init(addr, field_led_dma_buffer_a, field_led_dma_buffer_b);
  ClearLeds();
  UpdateLeds();

  // control MUX
  pinMode(PIN_FIELD_MUX_SEL_0, OUTPUT);
  pinMode(PIN_FIELD_MUX_SEL_1, OUTPUT);
  pinMode(PIN_FIELD_MUX_SEL_2, OUTPUT);
  controls[0].Init(PIN_FIELD_ADC_POT_MUX, control_update_rate);

  // Keyboard
  ShiftRegister4021<2>::Config keyboard_cfg;
  keyboard_cfg.clk = PIN_FIELD_CD4021_CLK;
  keyboard_cfg.latch = PIN_FIELD_CD4021_CS;
  keyboard_cfg.data[0] = PIN_FIELD_CD4021_D1;
  keyboard_sr_.Init(keyboard_cfg);

  //cv ins
  cv[0].InitBipolarCv(PIN_FIELD_ADC_CV_1, control_update_rate);
  cv[1].InitBipolarCv(PIN_FIELD_ADC_CV_2, control_update_rate);
  cv[2].InitBipolarCv(PIN_FIELD_ADC_CV_3, control_update_rate);
  cv[3].InitBipolarCv(PIN_FIELD_ADC_CV_4, control_update_rate);

  //cv outs
  pinMode(PIN_FIELD_DAC_1, OUTPUT);
  pinMode(PIN_FIELD_DAC_2, OUTPUT);
}

void DaisyHardware::InitPatchSM(float control_update_rate) {
  //the patch sm only works with the generic variant for now
  #ifdef ARDUINO_DAISY_PATCH_SM
  num_channels = 2;
  numControls = 12;
  numGates = 2;

  gateIns[0].Init(PIN_PATCH_SM_GATE_IN_1, INPUT, true);
  gateIns[1].Init(PIN_PATCH_SM_GATE_IN_2, INPUT, true);

  controls[0].InitBipolarCv(PIN_PATCH_SM_CV_1, control_update_rate);
  controls[1].InitBipolarCv(PIN_PATCH_SM_CV_2, control_update_rate);
  controls[2].InitBipolarCv(PIN_PATCH_SM_CV_3, control_update_rate);
  controls[3].InitBipolarCv(PIN_PATCH_SM_CV_4, control_update_rate);
  controls[4].InitBipolarCv(PIN_PATCH_SM_CV_5, control_update_rate);
  controls[5].InitBipolarCv(PIN_PATCH_SM_CV_6, control_update_rate);
  controls[6].InitBipolarCv(PIN_PATCH_SM_CV_7, control_update_rate);
  controls[7].InitBipolarCv(PIN_PATCH_SM_CV_8, control_update_rate);
  controls[8].Init(PIN_PATCH_SM_ADC_9, control_update_rate);
  controls[9].Init(PIN_PATCH_SM_ADC_10, control_update_rate);
  controls[10].Init(PIN_PATCH_SM_ADC_11, control_update_rate);
  controls[11].Init(PIN_PATCH_SM_ADC_12, control_update_rate);

  pinMode(PIN_PATCH_SM_CV_OUT_1, OUTPUT);
  pinMode(PIN_PATCH_SM_CV_OUT_2, OUTPUT);
  pinMode(PIN_PATCH_SM_GATE_OUT_1, OUTPUT);
  pinMode(PIN_PATCH_SM_GATE_OUT_2, OUTPUT);
  #endif
}

float DaisyHardware::GetKnobValue(uint8_t idx) {
  uint8_t pots[8] = {0, 3, 1, 4, 2, 5, 6, 7};
  digitalWrite(PIN_FIELD_MUX_SEL_0, pots[idx] & 1);
  digitalWrite(PIN_FIELD_MUX_SEL_1, (pots[idx] & 2) >> 1);
  digitalWrite(PIN_FIELD_MUX_SEL_2, (pots[idx] & 4) >> 2);

  return controls[0].Process();
}

bool DaisyHardware::KeyboardState(size_t idx) {
  return keyboard_state_[idx] == 0x00;
}

bool DaisyHardware::KeyboardRisingEdge(size_t idx) {
  return keyboard_state_[idx] == 0x80;
}

bool DaisyHardware::KeyboardFallingEdge(size_t idx) {
  return keyboard_state_[idx] == 0x7F;
}

void DaisyHardware::ProcessAnalogControls() {
  for (int i = 0; i < numControls; i++) {
    controls[i].Process();
  }

  if (device_ == DAISY_PETAL) {
    expression.Process();
  }
}

void DaisyHardware::ProcessDigitalControls() {
  if (device_ == DAISY_PATCH || device_ == DAISY_POD || device_ == DAISY_PETAL) {
    encoder.Debounce();
  }

  else if (device_ == DAISY_FIELD) {
    keyboard_sr_.Update();
    for (size_t i = 0; i < 16; i++) {
      uint8_t keyidx, keyoffset;
      keyoffset = i > 7 ? 8 : 0;
      keyidx = (7 - (i % 8)) + keyoffset;
      keyboard_state_[keyidx] =
          keyboard_sr_.State(i) | (keyboard_state_[keyidx] << 1);
    }
  }

  for (int i = 0; i < numSwitches; i++) {
    buttons[i].Debounce();
  }

  for (int i = 0; i < numGates; i++) {
    gateIns[i].Debounce();
  }
}

void DaisyHardware::ProcessAllControls() {
  ProcessAnalogControls();
  ProcessDigitalControls();
}

// petal led setters
void DaisyHardware::SetRingLed(uint8_t idx, float r, float g, float b) {
  if (idx < 0 || idx > 7 || device_ != DAISY_PETAL) {
    return; // bad idx or wrong board
  }

  uint8_t r_addr[8] = {PETAL_LED_RING_1_R, PETAL_LED_RING_2_R,
                       PETAL_LED_RING_3_R, PETAL_LED_RING_4_R,
                       PETAL_LED_RING_5_R, PETAL_LED_RING_6_R,
                       PETAL_LED_RING_7_R, PETAL_LED_RING_8_R};
  uint8_t g_addr[8] = {PETAL_LED_RING_1_G, PETAL_LED_RING_2_G,
                       PETAL_LED_RING_3_G, PETAL_LED_RING_4_G,
                       PETAL_LED_RING_5_G, PETAL_LED_RING_6_G,
                       PETAL_LED_RING_7_G, PETAL_LED_RING_8_G};
  uint8_t b_addr[8] = {PETAL_LED_RING_1_B, PETAL_LED_RING_2_B,
                       PETAL_LED_RING_3_B, PETAL_LED_RING_4_B,
                       PETAL_LED_RING_5_B, PETAL_LED_RING_6_B,
                       PETAL_LED_RING_7_B, PETAL_LED_RING_8_B};

  led_driver_.SetLed(r_addr[idx], r);
  led_driver_.SetLed(g_addr[idx], g);
  led_driver_.SetLed(b_addr[idx], b);
}

void DaisyHardware::SetFootswitchLed(uint8_t idx, float bright) {
  if (idx < 0 || idx > 3 || device_ != DAISY_PETAL) {
    return;
  }

  uint8_t fs_addr[4] = {PETAL_LED_FS_1, PETAL_LED_FS_2, PETAL_LED_FS_3,
                        PETAL_LED_FS_4};
  led_driver_.SetLed(fs_addr[idx], bright);
}

// field led setters
void DaisyHardware::SetKeyboardLed(uint8_t row, uint8_t idx, float value) {
  if (idx < 0 || idx > 7 || device_ != DAISY_FIELD) {
    return; // bad idx, row, or wrong board
  }

  // row A
  if (row == 0) {
    led_driver_.SetLed(idx, value);
  }
  // row B
  else if (row == 1) {
    led_driver_.SetLed(15 - idx, value);
  }
}

void DaisyHardware::SetKnobLed(uint8_t idx, float bright) {
  if (idx < 0 || idx > 7 || device_ != DAISY_FIELD) {
    return;
  }

  led_driver_.SetLed(16 + idx, bright);
}

void DaisyHardware::ClearLeds() {
  if (device_ == DAISY_PETAL) {
    for (size_t i = 0; i < 8; i++) {
      SetRingLed(i, 0.0f, 0.0f, 0.0f);
    }
    for (size_t i = 0; i < 4; i++) {
      SetFootswitchLed(i, 0.0f);
    }
  } else if (device_ == DAISY_FIELD) {
    for (size_t i = 0; i < 8; i++) {
      SetKeyboardLed(0, i, 0.f);
      SetKeyboardLed(1, i, 0.f);
      SetKnobLed(i, 0.f);
    }
  }
}

void DaisyHardware::UpdateLeds() {
  if (device_ != DAISY_PETAL && device_ != DAISY_FIELD) {
    return;
  }

  led_driver_.SwapBuffersAndTransmit();
}