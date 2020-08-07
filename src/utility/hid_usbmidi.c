//
// mi:muz:tuch#1 STM32 Version (ver.0.2 or later)
//
//  (CC at)2016 by D.F.Mac. @TripArts Music
//

#ifdef MIMUZ_TUCH_V020
// PIN Configuration (v0.2)
//
// PA0 : CH#1 Sensor (OUT)
// PA1 : CH#1 LED    (OUT)
// PA2 : CH#1 Sensor (IN)
// PA3 : CH#2 Sensor (OUT)
// PA4 : CH#2 LED    (OUT)
// PA5 : CH#2 Sensor (IN)
// PA6 : CH#3 Sensor (OUT)
// PA7 : CH#3 LED    (OUT)
// PB0 : CH#3 Sensor (IN)
// PB1 : Calibration Button (IN)
// PA8 : CH#4 Sensor (OUT)
// PA9 : CH#4 LED    (OUT)
// PA10: CH#4 Sensor (IN)
// PA11: USB D-
// PA12: USB D+
// PA13: SWDIO
// PA14: SWCLK
// PA15: CH#5 Sensor (OUT)
// PB3 : CH#5 LED    (OUT)
// PB4 : CH#5 Sensor (IN)
// PB5 : CH#6 Sensor (OUT)
// PB6 : CH#6 LED    (OUT)
// PB7 : CH#6 Sensor (IN)
// PB8 : BOOT0
// PF0 : USB DATA LED (OUT)
// PF1 : N/A

#else /* MIMUZ_TUCH_V030 */
// PIN Configuration (v0.3/v0.4)
//
// PA0 : CH#3 LED    (OUT)
// PA1 : CH#3 Sensor (OUT)
// PA2 : CH#3 Sensor (IN)
// PA3 : Calibration Button (IN)
// PA4 : CH#4 Sensor (OUT)
// PA5 : CH#4 Sensor (IN)
// PA6 : CH#4 LED    (OUT) 
// PA7 : CH#5 Sensor (OUT)
// PB0 : CH#5 Sensor (IN)
// PB1 : CH#5 LED    (OUT)
// PA8 : CH#6 Sensor (OUT)
// PA9 : CH#6 LED    (OUT)
// PA10: CH#6 Sensor (IN)
// PA11: USB D-
// PA12: USB D+
// PA13: SWDIO
// PA14: SWCLK
// PA15: CH#1 Sensor (OUT)
// PB3 : CH#1 Sensor (IN)
// PB4 : CH#1 LED    (OUT)
// PB5 : CH#2 LED    (OUT)
// PB6 : CH#2 Sensor (OUT)
// PB7 : CH#2 Sensor (IN)
// PB8 : BOOT0
// PF0 : USB DATA LED (OUT)
// PF1 : N/A
#endif

#include "stm32h7xx_hal.h"
#include "usb_device.h"
#include "usbd_midi_if.h"
#include "eeprom.h"
#include "hid_usbmidi.h"


// MIDI
uint16_t ledTime;

void led_on(void){
  ledTime = LEDTIMEMAX;
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);
}
void led_off(void){
  ledTime = 0;
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
}

static void onNoteOn(uint8_t ch, uint8_t note, uint8_t vel){
  led_on();
}

static void onNoteOff(uint8_t ch, uint8_t note, uint8_t vel){
  led_off();
}

int old_pin = GPIO_PIN_RESET;
uint8_t dbg_mode = 0; // 0:none 1:dbg mode
int dbg_cnt = 0;

// EEPROM
uint16_t VirtAddVarTab[NB_OF_VAR] = {
  0x1000, 0x1001, 0x1002, 0x1003,
  0x1004, 0x1005, 0x1006, 0x1007
};

uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0, 0, 0, 0, 0, 0};

////////////////
// touch Sensor
///////////////
float val[] = {0,0,0,0,0,0};
float comp[] = {-1,-1,-1,-1,-1,-1};
float compH[] = {-1,-1,-1,-1,-1,-1};
float compL[] = {0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff};

uint8_t notes[] = {36,38,40,43,45,48}; // C2 D2 E2 G2 A2 C3
uint8_t onoff[] = {0,0,0,0,0,0};
uint8_t seq[] = {0,0,0,0,0,0};

uint8_t mode = 0; // 0: none 1: write 2: read

static void onCtlChange(uint8_t ch, uint8_t num, uint8_t value){
  uint8_t func;

  led_on();
  if(ch == 15){
    func = num - 110;
    if(func == 9){        // 119
      mode = value;
    }else if(func < 6){  // Preset-number 110-115
      if(mode == 1){
        EE_WriteVariable(VirtAddVarTab[func],value);
        notes[func] = value;
      }else{  // mode == 2
        sendCtlChange(15,119,1);
        processMidiMessage();
        sendCtlChange(15,num,notes[func]);
        processMidiMessage();
      }
      mode = 0;
    }
  }
}

void loadNums(){
  uint8_t cnt;
  uint8_t cntmax = TOUCH_CHANNELS; // note numbers
  uint16_t chk;
  chk = EE_ReadVariable(VirtAddVarTab[7],&VarDataTab[7]);
  if(chk == 0){
    for(cnt = 0;cnt < cntmax;cnt ++){
      EE_ReadVariable(VirtAddVarTab[cnt],&VarDataTab[cnt]);
      notes[cnt] = (uint8_t)VarDataTab[cnt];
    }
  }else{
    for(cnt = 0;cnt < cntmax;cnt ++){
      EE_WriteVariable(VirtAddVarTab[cnt],(uint16_t)(notes[cnt]));
    }
    EE_WriteVariable(VirtAddVarTab[7],8);
  }
}

// Tuch Sensor GPIO Table
TTouchGPIO ttouchGpio[TOUCH_CHANNELS]={
#ifdef MIMUZ_TUCH_V020
  {GPIOA, GPIOA, GPIOA, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2}, // PA0, PA1, PA2 (CH#1)
  {GPIOA, GPIOA, GPIOA, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5}, // PA3, PA4, PA5 (CH#2)
  {GPIOA, GPIOA, GPIOB, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_0}, // PA6, PA7, PB0 (CH#3)
  {GPIOA, GPIOA, GPIOA, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10},// PA8, PA9, PA10(CH#4)
  {GPIOA, GPIOB, GPIOB, GPIO_PIN_15,GPIO_PIN_3, GPIO_PIN_4}, // PA15,PB3, PB4 (CH#5)
  {GPIOB, GPIOB, GPIOB, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7}  // PB5, PB6, PB7 (CH#6)
#else /* MIMUZ_TUCH_V030 */
  {GPIOA, GPIOB, GPIOB, GPIO_PIN_15,GPIO_PIN_4, GPIO_PIN_3}, // PA15,PB4, PB3 (CH#1)
  {GPIOB, GPIOB, GPIOB, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_7}, // PB6, PB5, PB7 (CH#2)
  {GPIOA, GPIOA, GPIOA, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_2}, // PA1, PA0, PA2 (CH#3)
  {GPIOA, GPIOA, GPIOA, GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_5}, // PA4, PA6, PA5 (CH#4)
  {GPIOA, GPIOB, GPIOB, GPIO_PIN_7, GPIO_PIN_1, GPIO_PIN_0}, // PA7, PB1, PB0 (CH#5)
  {GPIOA, GPIOA, GPIOA, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10} // PA8, PA9, PA10(CH#6)
#endif
};

void sensorLEDOn(int ch){
  HAL_GPIO_WritePin(ttouchGpio[ch].pLEDGPIOPort, ttouchGpio[ch].LedPin, GPIO_PIN_SET);
}

void sensorLEDOff(int ch){
  HAL_GPIO_WritePin(ttouchGpio[ch].pLEDGPIOPort, ttouchGpio[ch].LedPin, GPIO_PIN_RESET);
}

void sensorPinOut(int ch){
  HAL_GPIO_WritePin(ttouchGpio[ch].pOutGPIOPort, ttouchGpio[ch].OutPin, GPIO_PIN_SET);
}

void sensorPinClear(int ch){
  HAL_GPIO_WritePin(ttouchGpio[ch].pOutGPIOPort, ttouchGpio[ch].OutPin, GPIO_PIN_RESET);
}

int sensorPinCheck(int ch){
  int pin = GPIO_PIN_RESET;
  pin = HAL_GPIO_ReadPin(ttouchGpio[ch].pInGPIOPort,ttouchGpio[ch].InPin);
  return(pin);
}

void sensorMeasure(int ch){
  float cnt;
  float calc;

  cnt = 0;
  sensorPinOut(ch);
  while(1){
    if(sensorPinCheck(ch) !=GPIO_PIN_RESET){
      break;
    }
    cnt ++;
  }
  sensorPinClear(ch);
  while(1){
    if(sensorPinCheck(ch) !=GPIO_PIN_SET){
      break;
    }
    cnt ++;
  }
  calc = (cnt - val[ch])/SENSOR_FILTER_DIV;
  val[ch] += calc;
}


void sensorLedBlink(){
  int cnt;
  int ledno = 0;

  for(cnt = 0;cnt < 8;cnt ++){
    if((cnt%2) == 0){
      ledno = 0;
    }else{
      ledno = 5;
    }
    sensorLEDOn(ledno);
    HAL_Delay(50);
    sensorLEDOff(ledno);
    HAL_Delay(50);
  }
}

void sensorCalibration(void){
  int ch;
  int cnt;

  sensorLedBlink();

  for(ch = 0;ch < TOUCH_CHANNELS; ch ++){
    val[ch] = 0;
    comp[ch] = -1;
    compH[ch] = -1;
    compL[ch] = 0x7fff;
  }
  for(cnt = 0;cnt < SENSOR_CALIB_TIMES;cnt ++){
    for(ch = 0;ch < TOUCH_CHANNELS; ch ++){
      sensorMeasure(ch);
      comp[ch] = val[ch];
      if(comp[ch] > compH[ch]){
        compH[ch] = comp[ch];
      }
      if(comp[ch] < compL[ch]){
        compL[ch] = comp[ch];
      }
      processMidiMessage();
    }
  }
  for(ch = 0;ch < TOUCH_CHANNELS; ch ++){
    comp[ch] = compH[ch]+((compH[ch]-compL[ch])/4);
  }
  HAL_Delay(100);

  sensorLedBlink();
}

int old_button = GPIO_PIN_RESET;

int checkCalibrationButton(){
  int pin = GPIO_PIN_RESET;
  int res = 0;
#ifdef MIMUZ_TUCH_V020
  pin = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
#else
  pin = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3);
#endif
  if((pin == GPIO_PIN_SET)&&(pin != old_button)){
    res = 1;
  }
  old_button = pin;
  return(res);

}

void trigNoteOff(){
  if(ledTime > 0){
    ledTime --;
    if(ledTime == 0){
      led_off();
    }
  }
}

int main(void){
  int ch;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  HAL_Delay(700);
  HAL_FLASH_Unlock();

  EE_Init();
  loadNums();

  // mimuz
  mimuz_init();
  setHdlNoteOff(onNoteOff);
  setHdlNoteOn(onNoteOn);
  setHdlCtlChange(onCtlChange);

  if(checkCalibrationButton() != 0){
    dbg_mode = 1;
  }

  sensorCalibration();

  sendDbgData(SEND_DBG_KIND_COMP);

  while (1){
    // touch Sensor
    for(ch = 0;ch < TOUCH_CHANNELS; ch ++){
      sensorMeasure(ch);
      sendDbgData(SEND_DBG_KIND_VAL);
      if(val[ch] > comp[ch]){
        if(seq[ch] < SENSOR_CHECK_NUM){
          seq[ch]++;
          if(seq[ch] > SENSOR_UP_VALUE){
            if(onoff[ch] == 0){
              sensorLEDOn(ch);
              onoff[ch] = 1;
              led_on();
              sendNoteOn(0,notes[ch],100);
              processMidiMessage();
            }
          }
        }
      }else{
        if(seq[ch] > 0){
          seq[ch]--;
          if(seq[ch] < SENSOR_DOWN_VALUE){
            if(onoff[ch] == 1){
              sensorLEDOff(ch);
              onoff[ch] = 0;
              led_on();
              sendNoteOff(0,notes[ch]);
              processMidiMessage();
            }
          }
        }
      }
    }

    // Calibration
    if(checkCalibrationButton() != 0){
      sensorCalibration();
      sendDbgData(SEND_DBG_KIND_COMP);
    }

    trigNoteOff();

    // for dbg mode
    if(dbg_mode == 1){
      dbg_cnt ++;
      if(dbg_cnt >= DBG_SEND_INTERVAL){
        dbg_cnt = 0;
      }
    }

  }
}

// kind 0: val[], 1: comp[]
void sendDbgData(int kind){
  if(dbg_mode == 1){
    if((dbg_cnt / TOUCH_CHANNELS) == 0){
      int base;
      float *pData = &val[0];
      if(kind == SEND_DBG_KIND_COMP){
        int out_ch;
        pData = &comp[0];
        base = 0;
        for(out_ch = 0;out_ch < TOUCH_CHANNELS;out_ch ++){
          int data = (int)*(pData + out_ch);
          sendCtlChange(0,out_ch*2+base,(uint8_t)(data&0x007f));
          sendCtlChange(0,(out_ch*2)+base+1,(uint8_t)((data>>7)&0x007f));
          processMidiMessage();
        }
      }else{
        pData = &val[0];
        base = 12;
        int out_ch = dbg_cnt % TOUCH_CHANNELS;
        int data = (int)*(pData + out_ch);
        sendCtlChange(0,out_ch*2+base,(uint8_t)(data&0x007f));
        sendCtlChange(0,(out_ch*2)+base+1,(uint8_t)((data>>7)&0x007f));
        processMidiMessage();
      }
    }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLP = 0x00000001U; // RCC_PLL1_DIVP   PREDIV input clock / 2
  RCC_OscInitStruct.PLL.PLLN = 0x00000000U; // PLL input clock * 2

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#ifdef MIMUZ_TUCH_V020
// PIN Configuration (v0.2)
//
// PA0 : CH#1 Sensor (OUT)
// PA1 : CH#1 LED    (OUT)
// PA2 : CH#1 Sensor (IN)
// PA3 : CH#2 Sensor (OUT)
// PA4 : CH#2 LED    (OUT)
// PA5 : CH#2 Sensor (IN)
// PA6 : CH#3 Sensor (OUT)
// PA7 : CH#3 LED    (OUT)
// PB0 : CH#3 Sensor (IN)
// PB1 : Calibration Button (IN)
// PA8 : CH#4 Sensor (OUT)
// PA9 : CH#4 LED    (OUT)
// PA10: CH#4 Sensor (IN)
// PA11: USB D-
// PA12: USB D+
// PA13: SWDIO
// PA14: SWCLK
// PA15: CH#5 Sensor (OUT)
// PB3 : CH#5 LED    (OUT)
// PB4 : CH#5 Sensor (IN)
// PB5 : CH#6 Sensor (OUT)
// PB6 : CH#6 LED    (OUT)
// PB7 : CH#6 Sensor (IN)
// PB8 : BOOT0
// PF0 : USB DATA LED (OUT)
// PF1 : N/A

#else /* MIMUZ_TUCH_V030 */
// PIN Configuration (v0.3/v0.4)
//
// PA0 : CH#3 LED    (OUT)
// PA1 : CH#3 Sensor (OUT)
// PA2 : CH#3 Sensor (IN)
// PA3 : Calibration Button (IN)
// PA4 : CH#4 Sensor (OUT)
// PA5 : CH#4 Sensor (IN)
// PA6 : CH#4 LED    (OUT) 
// PA7 : CH#5 Sensor (OUT)
// PB0 : CH#5 Sensor (IN)
// PB1 : CH#5 LED    (OUT)
// PA8 : CH#6 Sensor (OUT)
// PA9 : CH#6 LED    (OUT)
// PA10: CH#6 Sensor (IN)
// PA11: USB D-
// PA12: USB D+
// PA13: SWDIO
// PA14: SWCLK
// PA15: CH#1 Sensor (OUT)
// PB3 : CH#1 Sensor (IN)
// PB4 : CH#1 LED    (OUT)
// PB5 : CH#2 LED    (OUT)
// PB6 : CH#2 Sensor (OUT)
// PB7 : CH#2 Sensor (IN)
// PB8 : BOOT0
// PF0 : USB DATA LED (OUT)
// PF1 : N/A
#endif

static void MX_GPIO_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PF0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

#ifdef MIMUZ_TUCH_V020

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA3 PA4 
                           PA6 PA7 PA8 PA9 
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA5 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB3 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB4 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


#else /* MIMUZ_TUCH_V030 */

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA4 
                          PA6 PA7 PA8 PA9 
                          PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA5 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,GPIO_PIN_RESET);

  /*Configure GPIO pins : PB1 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB3 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

#endif
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){}
#endif
