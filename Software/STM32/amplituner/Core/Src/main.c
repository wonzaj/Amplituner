/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "rng.h"
#include "rtc.h"
#include "sai.h"
#include "spi.h"
#include "tim.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "float.h"
#include "stdbool.h"
#include "stdint.h"
#include "EEPROM/eeprom.h"
#include "radio/RDA5807m.h"
#include "TDA7719/TDA7719.h"
#include "buttons/buttons.h"
#include "Encoders/encoders.h"
#include <Adc/arm_math.h>
#include <Adc/fft.h>
#include "Adc/UVmeter.h"
#include <Display_OLED/draws_display.h>
#include <Display_OLED/SSD1322_API.h>
#include <Display_OLED/SSD1322_GFX.h>
#include <Display_OLED/SSD1322_HW_Driver.h>
#include <Time/time.h>

/* Includes for czcionki */

#include "Display_OLED/Fonts/custom_chars.h"
#include "Display_OLED/Fonts/FreeMono24pt7b.h"
#include "Display_OLED/Fonts/FreeMono9pt7b.h"
#include "Display_OLED/Fonts/FreeMonoBold24pt7b.h"
#include "Display_OLED/Fonts/FreeMonoBold9pt7b.h"
#include "Display_OLED/Fonts/FreeMonoBoldOblique24pt7b.h"
#include "Display_OLED/Fonts/FreeMonoBoldOblique9pt7b.h"
#include "Display_OLED/Fonts/FreeMonoOblique24pt7b.h"
#include "Display_OLED/Fonts/FreeMonoOblique9pt7b.h" //git
#include "Display_OLED/Fonts/FreeSans24pt7b.h"       //zegar kilka pikseli nizej
#include "Display_OLED/Fonts/FreeSans9pt7b.h"
#include "Display_OLED/Fonts/FreeSansBold24pt7b.h" //zegar kilka pikseli nizej
#include "Display_OLED/Fonts/FreeSansBold9pt7b.h"
#include "Display_OLED/Fonts/FreeSansBoldOblique24pt7.h"
#include "Display_OLED/Fonts/FreeSansBoldOblique9pt7b.h"
#include "Display_OLED/Fonts/FreeSansOblique24pt7b.h"
#include "Display_OLED/Fonts/FreeSansOblique9pt7b.h"
#include "Display_OLED/Fonts/FreeSerif24pt7b.h" //git
#include "Display_OLED/Fonts/FreeSerif9pt7b.h"
#include "Display_OLED/Fonts/FreeSerifBold24pt7b.h"
#include "Display_OLED/Fonts/FreeSerifBold9pt7b.h"
#include "Display_OLED/Fonts/FreeSerifBoldItalic24pt7b.h"
#include "Display_OLED/Fonts/FreeSerifBoldItalic9pt7b.h"
#include "Display_OLED/Fonts/FreeSerifItalic24pt7b.h"
#include "Display_OLED/Fonts/FreeSerifItalic9pt7b.h"
#include "Display_OLED/Fonts/maciek_font.h"
#include "Display_OLED/Fonts/Org_01.h"        // 200hz 2k 20k
#include "Display_OLED/Fonts/Picopixel.h"     // 200hz 2k 200k ale troche gorsze
#include "Display_OLED/Fonts/Tiny3x3a2pt7b.h" //niewyrazne
#include "Display_OLED/Fonts/TomThumb.h"
///#include "creeper.h"
//#include "krecik.h"
//#include "pat_i_mat.h"
//#include "coolface.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFF_SIZE_UART_ESP 6
#define ROWS 64
#define COLUMS 256
#define FFT_SAMPLES 2048
#define ADC_NUMBER_OF_CHANNELS 4
#define ADC_NUMBER_OF_MEASUREMENT 32
#define FFT_NUMBER_OF_PRAZKI 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define STRINGIFY(a) #a   //zamiana na string
#define CONCAT(a, b) a##b //????czenie string??w
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t SPI5_TX_completed_flag = 1;     //flag indicating finish of SPI transmission
volatile uint8_t UART1_RX_completed_flag = 0;    //flag indicating receiving data on UART
volatile uint8_t ADC_Samples_FFT_ready_flag = 0; //flag indicating receiving enough samples for FFT
volatile uint8_t I2C_TX_completed_flag = 1;
volatile uint8_t ADC_IS_ON_flag = 0;
volatile uint8_t ADC_samples_ready = 0;
volatile uint8_t FFT_calc_done = 0;
volatile uint8_t RADIO_IS_ON_front_flag;
volatile uint8_t RADIO_IS_ON_back_flag;
volatile uint8_t POWER_device_state_flag = 0;

extern encoder_t encoderVolFront;
extern encoder_t encoderVolBack;
extern FFT_channel_source_e FFT_channel_source;
extern RTC_typeOfAlarm_t RTC_typeOfAlarm;
extern SettingsUserMenu_t SettingsUserMenu;
extern SSD1322_Screen_t SSD1322_Screen_State;
extern SSD1322_Screen_t SSD1322_Screen_State_Saved;
extern Clock_Data_Change_t Clock_Data_Time;
extern AlarmDataChange_t AlarmDataChange;

_Bool volatile button_debou_state = true;
extern _Bool alarm_set_A_or_B;
extern _Bool IS_ALARM_SET_A;
extern _Bool IS_ALARM_SET_B;
uint16_t gGPIO_Pin;

/* Main buffer for OLED display
 * Each byte hold info for 2 pixels */
uint8_t DisplayOLEDBuffer[COLUMS * ROWS / 2];

/* FFT variables */
uint8_t OutFreqArray[21];
arm_rfft_fast_instance_f32 FFTHandler;
uint16_t ADC_In[FFT_SAMPLES];
float FFTInBuffer[FFT_SAMPLES];
float FFTOutBuffer[FFT_SAMPLES];
volatile uint8_t Samples_ready;

/* Alarms variables */
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
RTC_AlarmTypeDef Alarm_A;
RTC_AlarmTypeDef Alarm_B;
RTC_AlarmTypeDef Alarm;
extern uint8_t RTC_typeOfAlarm_A;
extern uint8_t RTC_typeOfAlarm_B;

/* ADC variables */
uint16_t ADC_SamplesTEST[4];
uint32_t ADC_SamplesSUM[4];
uint8_t UV_meter_front_back;
uint8_t UV_meter_numb_of_chan = 4;

typedef struct
{
  uint16_t Freq;
  uint16_t RSSI;
  uint16_t Station_saved1;
  uint16_t Station_saved2;
  uint16_t Station_saved3;
  uint16_t volumeMaster;
  //add CRC
} ESP_UART_frame_t;
ESP_UART_frame_t ESP_UART_frame;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void set_init_values(void); //init with declaration (does it work after software restart of MCU?)
void display_init(void);
float calc_pwm(float val);
void change_power_led_brigh(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC2_Init();
  MX_ADC1_Init();
  MX_DAC1_Init();
  MX_I2C2_Init();
  MX_RTC_Init();
  MX_SPI3_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_TIM8_Init();
  MX_TIM15_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_RNG_Init();
  MX_TIM4_Init();
  MX_LPUART1_UART_Init();
  MX_SAI1_Init();
  MX_USB_OTG_FS_HCD_Init();
  MX_TIM1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  //doda?? generowanie sinusoidy z wybieran?? HZ
  //zasygnalizowa?? wy????czenie urz??dzenia
  //przy starcie systemu glo??no???? stopniowo zwi??ksza??
  //mo??liwo???? w????czania i wy????czania maksymalnie z cz??stotliwo??ci?? 0,5s
  //prze????czanie przeka??nika z back do s??uchawek
  HAL_TIM_OC_Start_IT(&htim15, TIM_CHANNEL_1);
  display_init_on_start(DisplayOLEDBuffer);
  set_init_values();
  encoders_init();
  TDA7719_init();
  RDA5807_Init();
  //RDA5807_PowerOff();
  read_user_settings_on_init();

  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, SET);
  arm_rfft_fast_init_f32(&FFTHandler, FFT_SAMPLES);
  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    change_power_led_brigh();
    check_FFT_flag(); //if fft flag == 0 then do fft, move to ssd1322_sreen_FFT
    Refresh_display(SSD1322_Screen_State);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* ADC1_2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(ADC1_2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* EXTI9_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  /* TIM1_BRK_TIM15_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
  /* TIM1_UP_TIM16_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
  /* TIM1_TRG_COM_TIM17_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* TIM4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM4_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* RTC_Alarm_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* TIM8_CC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM8_CC_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
  /* TIM5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  /* SPI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SPI3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(SPI3_IRQn);
  /* TIM6_DAC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

/* USER CODE BEGIN 4 */

/* Timer output compare callback
 * Is used for:
 * - TIM15 - if set, changing screen state back to saved screen
 * - TIM16 - PWM timer for power LED
 * - TIM17 - Used for debouncing all buttons
 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Interupt callback after timer counts 3 secs.
	 * Timer is started when any encoder is moved
	 * Refresh display timer or change display mode timer
	 */
  if (htim->Instance == TIM15)
  {

    /* Check if interupt is from change display */
    if (htim->ChannelState[0] == HAL_TIM_CHANNEL_STATE_BUSY)
    {
      change_display_state(htim);
    }

    /* Check if interupt is from return to last saved display state */
    if (htim->ChannelState[1] == HAL_TIM_CHANNEL_STATE_BUSY)
    {
      set_saved_display(htim);
    }
  }

  /* Routine after getting a interupt from GPIO ports */
  if (htim->Instance == TIM17) //timer 17 is used only for debouncing
  {
    any_buttons_pressed_routine();
  }
}

/* Time input capture callback
 * Is called whenever any encoder change it's position (expect for radio encoder)
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  display_on_standby_mode();
  check_volumes_ranges();

  /* Volume front encoder */
  if (htim->Instance == TIM2)
  {
    volume_front_encoder_used();
  }
  /* Volume back encoder */
  if (htim->Instance == TIM8)
  {
    volume_back_encoder_used();
  }
  /* Loundness encoder */
  if (htim->Instance == TIM1)
  {
    loudness_encoder_used();
  }

  /* Bass encoder */
  if (htim->Instance == TIM4)
  {
    bass_encoder_used();
  }

  /* Middle encoder */
  if (htim->Instance == TIM3)
  {
    middle_encoder_used();
  }

  /* Treble encoder */
  if (htim->Instance == TIM5)
  {
    treble_encoder_used();
  }

  /* Radio  encoder */
  //if(htim->Instance == TIM1)
  {
    //radio_encoder_used();
  }
}

// Constains operations needed to calculate FFT and UVmeter - it's using ADC1 perph
// ADC2 is used for monitoring battery voltage while disconnected from main power source
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC2)
  {
    if (SSD1322_Screen_State == SSD1322_SCREEN_UVMETER)
    {
      calc_UVmeter_scale();
    }
    else if (SSD1322_Screen_State == SSD1322_SCREEN_FFT)
    {
      calc_FFT_scale();
    }
  }

  //adc1 is used for vbat monitoring after power is set off
  if (hadc->Instance == ADC1)
  {
  }
}

// Set init values to specified variables
// temporary function used while prototyping
void set_init_values(void)
{
  SSD1322_Screen_State = SSD1322_SCREEN_TIME;
  AlarmDataChange = 1;
  Clock_Data_Time = 1;
  RTC_typeOfAlarm = 1;
  Alarm.AlarmDateWeekDay = 1;
  encoderVolFront.audioOutputState = MASTER;
  encoderVolBack.audioOutputState = NORMAL;
  encoderVolFront.volumeMaster = 79;
  encoderVolFront.volumeLeftRight = 79;
  encoderVolFront.volumeLeft = 79;
  encoderVolFront.volumeRight = 79;
  encoderVolBack.volumeLeftRight = 79;
  encoderVolBack.volumeLeft = 79;
  encoderVolBack.volumeRight = 79;
  encoderFilterLoudness.gain = 15;
  encoderFilterLoudness.buttonControl = SET_GAIN;
  encoderFilterTreble.gain = 10;
  encoderFilterTreble.buttonControl = SET_GAIN;
  encoderFilterMiddle.gain = 10;
  encoderFilterMiddle.buttonControl = SET_GAIN;
  TDA7719_config.set_input_front = JACK_1;
  TDA7719_config.set_input_back = JACK_2;
  savedUserSettings.displayRefreshTime = 100;
  RADIO_IS_ON_front_flag = false;
  RADIO_IS_ON_back_flag = false;
  SettingsUserMenu.AlarmSource_A = JACK_1;
  SettingsUserMenu.AlarmSource_B = RADIO;
  SettingsUserMenu.RefreshScreenTime = 65535;
  SettingsUserMenu.Power_LED = 5;
  SettingsUserMenu.Display_mode = 1;
}

// change linear scale to more eye-friendly
float calc_pwm(float val)
{
  const float k = 0.1f;
  const float x0 = 60.0f;
  return 300.0f / (1.0f + exp(-k * (val - x0)));
}

// change power led brightness according to settings
void change_power_led_brigh(void)
{
  static uint32_t led_counter;
  //static uint32_t timer_counter;
  static float r;

  //add timer from SysTick

  switch (SettingsUserMenu.Power_LED)
  {
  case POWER_OFF:
    //if(power_button_state == OFF)
    {
      //			float r = 50 * (1.0f + sin(led_counter / 100.0f));
      //			__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, calc_pwm(r));
      //			led_counter++;
    }
    break;
  case POWER_ON:
    //if(power_button_state == OFF)
    {
      //			float r = 50 * (1.0f + sin(led_counter / 100.0f));
      //			__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, calc_pwm(r));
      //			led_counter++;
    }
    break;
  case Always_OFF:
    __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);
    break;

  case Always_ON:
    __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 255);
    break;

  case Change_brigh:
    r = 50 * (1.0f + sin(led_counter / 100.0f));
    __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, calc_pwm(r));
    led_counter++;

    break;
  //mo??e case gdzie u??ytkownik podaje parametr okre??laj??cy jasno????
  default:
    break;
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  __NOP();
  //__bswap16();
  __NVIC_SystemReset();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

