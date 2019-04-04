/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if defined(__GNUC__)
#include <errno.h>
#include <sys/unistd.h> /* STDOUT_FILENO, STDERR_FILENO */
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define LCD_FRAME_BUFFER_LAYER0                 (LCD_FRAME_BUFFER+0x130000)
#define LCD_FRAME_BUFFER_LAYER1                 LCD_FRAME_BUFFER
#define CONVERTED_FRAME_BUFFER                  (LCD_FRAME_BUFFER+0x260000)
#define INPUT_IMAGE_WIDTH						28
#define INPUT_IMAGE_HEIGHT 						28
#define LCD_INPUT_IMAGE_WIDTH					240
#define LCD_INPUT_IMAGE_HEIGHT 					240
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

SPI_HandleTypeDef hspi5;

UART_HandleTypeDef huart1;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */
TS_StateTypeDef screen_state;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
static void MX_LTDC_Init(void);
static void MX_FMC_Init(void);
void MX_USART1_UART_Init(void);
static void MX_I2C3_Init(void);
static void MX_SPI5_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
static void Lcd_Init();
static void Touch_init();
//void Print_Image(ai_float *image);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int fd, const void *buff, int count)
{
    HAL_StatusTypeDef status;

    if ((count < 0) && (fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    status = HAL_UART_Transmit(&huart1, (uint8_t *)buff, count,
            HAL_MAX_DELAY);

    return (status == HAL_OK ? count : 0);
}



void Print_Image(ai_float *image[INPUT_IMAGE_WIDTH][INPUT_IMAGE_HEIGHT][1]){
	printf("\e[1;1H\e[2J");
	for(int i = 0; i < INPUT_IMAGE_WIDTH; ++i) {
		for (int j = 0; j < INPUT_IMAGE_HEIGHT;  ++j) {
			BSP_LCD_DrawPixel(i,j+240,(int)image[i][j][0]);
			printf("%0.2f,", image[i][j][0]);
		}
		printf("\r\n");
	}
}


void Lcd_Init(){
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER_LAYER1);
  BSP_LCD_SelectLayer(1);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetColorKeying(1, LCD_COLOR_WHITE);
  BSP_LCD_SetLayerVisible(1, DISABLE);
  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_DisplayOn();
  HAL_Delay(100);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  HAL_Delay(100);
}
void Touch_init(){
  if (BSP_TS_Init(240, 320) == TS_ERROR) {
	printf("BSP_TS_Init error\r\n");
  }
  else {
	printf("BSP_TS_Init OK!\r\n");
  }
  HAL_Delay(100);
}
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

  /* USER CODE BEGIN SysInit */
  Lcd_Init();
  BSP_PB_Init(BUTTON_KEY,0);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
//  MX_DMA2D_Init();
//  MX_LTDC_Init();
//  MX_FMC_Init();
//  MX_I2C3_Init();
//  MX_SPI5_Init();
  MX_CRC_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */
  Touch_init();
  /* initialize input/output buffer handlers */
  const ai_u16 batch_size = 1;
  ai_float in_data[28][28]= {{0}};
  ai_float in_data_cov[28][28][1] = {{0}};
//  ai_float in_data[28][28] =  {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.011764705882352941,0.07058823529411765,0.07058823529411765,0.07058823529411765,0.49411764705882355,0.5333333333333333,0.6862745098039216,0.10196078431372549,0.6509803921568628,1.0,0.9686274509803922,0.4980392156862745,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.11764705882352941,0.1411764705882353,0.3686274509803922,0.6039215686274509,0.6666666666666666,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.8823529411764706,0.6745098039215687,0.9921568627450981,0.9490196078431372,0.7647058823529411,0.25098039215686274,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.19215686274509805,0.9333333333333333,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.984313725490196,0.36470588235294116,0.3215686274509804,0.3215686274509804,0.2196078431372549,0.15294117647058825,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.07058823529411765,0.8588235294117647,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.7764705882352941,0.7137254901960784,0.9686274509803922,0.9450980392156862,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.9137254901960784,0.611764705882353,0.4196078431372549,0.9921568627450981,0.9921568627450981,0.803921568627451,0.043137254901960784,0.0,0.16862745098039217,0.6039215686274509,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.054901960784313725,0.00392156862745098,0.9039215686274509,0.9921568627450981,0.35294117647058826,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.1450980392156862,0.7921568627450981,0.7450980392156863,0.00784313725490196,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.093137254901960784,0.9150980392156863,0.9921568627450981,0.27450980392156865,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.13725490196078433,0.9450980392156862,0.8823529411764706,0.6274509803921569,0.4235294117647059,0.00392156862745098,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.3176470588235294,0.9411764705882353,0.9921568627450981,0.9921568627450981,0.4666666666666667,0.09803921568627451,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.19647058823529413,0.7294117647058823,0.9921568627450981,0.9921568627450981,0.5882352941176471,0.10588235294117647,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.09274509803921569,0.36470588235294116,0.9882352941176471,0.9921568627450981,0.7333333333333333,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.9764705882352941,0.9921568627450981,0.9764705882352941,0.25098039215686274,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.1803921568627451,0.5098039215686274,0.7176470588235294,0.9921568627450981,0.9921568627450981,0.8117647058823529,0.00784313725490196,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.95294117647058825,0.5803921568627451,0.8980392156862745,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9803921568627451,0.7137254901960784,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.09411764705882353,0.4470588235294118,0.8666666666666667,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.788235294117647,0.3058823529411765,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.09019607843137255,0.25882352941176473,0.8352941176470589,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.7764705882352941,0.3176470588235294,0.00784313725490196,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.07058823529411765,0.6705882352941176,0.8588235294117647,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.7647058823529411,0.3137254901960784,0.03529411764705882,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.21568627450980393,0.6745098039215687,0.8862745098039215,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9921568627450981,0.9568627450980393,0.5215686274509804,0.043137254901960784,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.9333333333333333,0.1921568627450981,0.9921568627450981,0.1921568627450981,0.8313725490196079,0.5294117647058824,0.5176470588235295,0.06274509803921569,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}};
//




//  ai_float in_data[28][28] = {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.2,0.6235294117647059,0.9921568627450981,0.6235294117647059,0.19607843137254902,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.18823529411764706,0.9333333333333333,0.9882352941176471,0.9882352941176471,0.9882352941176471,0.9294117647058824,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.21176470588235294,0.8901960784313725,0.9921568627450981,0.9882352941176471,0.9372549019607843,0.9137254901960784,0.9882352941176471,0.2235294117647059,0.023529411764705882,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0392156862745098,0.23529411764705882,0.8784313725490196,0.9882352941176471,0.9921568627450981,0.9882352941176471,0.792156862745098,0.32941176470588235,0.9882352941176471,0.9921568627450981,0.47843137254901963,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.6392156862745098,0.9882352941176471,0.9882352941176471,0.9882352941176471,0.9921568627450981,0.9882352941176471,0.9882352941176471,0.3764705882352941,0.7411764705882353,0.9921568627450981,0.6549019607843137,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.2,0.9333333333333333,0.9921568627450981,0.9921568627450981,0.7450980392156863,0.4470588235294118,0.9921568627450981,0.8941176470588236,0.1843137254901961,0.30980392156862746,1.0,0.6588235294117647,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.18823529411764706,0.9333333333333333,0.9882352941176471,0.9882352941176471,0.7019607843137254,0.047058823529411764,0.29411764705882354,0.4745098039215686,0.08235294117647059,0.0,0.0,0.9921568627450981,0.9529411764705882,0.19607843137254902,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.14901960784313725,0.6470588235294118,0.9921568627450981,0.9137254901960784,0.8156862745098039,0.32941176470588235,0.0,0.0,0.0,0.0,0.0,0.0,0.9921568627450981,0.9882352941176471,0.6470588235294118,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.027450980392156862,0.6980392156862745,0.9882352941176471,0.9411764705882353,0.2784313725490196,0.07450980392156863,0.10980392156862745,0.0,0.0,0.0,0.0,0.0,0.0,0.9921568627450981,0.9882352941176471,0.7647058823529411,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.2235294117647059,0.9882352941176471,0.9882352941176471,0.24705882352941178,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.9921568627450981,0.9882352941176471,0.7647058823529411,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.7764705882352941,0.9921568627450981,0.7450980392156863,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.9921568627450981,0.7686274509803922,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.2980392156862745,0.9647058823529412,0.9882352941176471,0.4392156862745098,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.9921568627450981,0.9882352941176471,0.5803921568627451,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.3333333333333333,0.9882352941176471,0.9019607843137255,0.09803921568627451,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.027450980392156862,0.5294117647058824,0.9921568627450981,0.7294117647058823,0.047058823529411764,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.3333333333333333,0.9882352941176471,0.8745098039215686,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.027450980392156862,0.5137254901960784,0.9882352941176471,0.8823529411764706,0.2784313725490196,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.3333333333333333,0.9882352941176471,0.5686274509803921,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.18823529411764706,0.6470588235294118,0.9882352941176471,0.6784313725490196,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.33725490196078434,0.9921568627450981,0.8823529411764706,0.0,0.0,0.0,0.0,0.0,0.0,0.4470588235294118,0.9333333333333333,0.9921568627450981,0.6352941176470588,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  { 0.0,0.0,0.0,0.0,0.0,0.0,0.3333333333333333,0.9882352941176471,0.9764705882352941,0.5725490196078431,0.18823529411764706,0.11372549019607843,0.3333333333333333,0.6980392156862745,0.8823529411764706,0.9921568627450981,0.8745098039215686,0.6549019607843137,0.2196078431372549,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  { 0.0,0.0,0.0,0.0,0.0,0.0,0.3333333333333333,0.9882352941176471,0.9882352941176471,0.9882352941176471,0.8980392156862745,0.8431372549019608,0.9882352941176471,0.9882352941176471,0.9882352941176471,0.7686274509803922,0.5098039215686274,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.10980392156862745,0.7803921568627451,0.9882352941176471,0.9882352941176471,0.9921568627450981,0.9882352941176471,0.9882352941176471,0.9137254901960784,0.5686274509803921,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.09803921568627451,0.5019607843137255,0.9882352941176471,0.9921568627450981,0.9882352941176471,0.5529411764705883,0.1450980392156863,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
//  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}};

  ai_float out_data[10];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  BSP_TS_GetState(&screen_state);
	  BSP_LCD_DrawRect(0,0,240,240);
	  	  if(screen_state.TouchDetected){
	  		  int x =screen_state.Y*((float)28/240);
	  		  int y = screen_state.X*((float)28/240);
	  		  printf("in data at X= %d Y =%d state x=%d y=%d\r\n", x , y , screen_state.X, screen_state.Y);
	  		  //image[screen_state.X][screen_state.Y]= 0.9 ;
	  		  in_data[x+1][y+1] = 0.99;
	  		  in_data[x-1][y-1] = 0.99;
	  		  in_data[x+1][y] = 0.99;
	  		  in_data[x-1][y] = 0.99;
	  		  in_data[x][y+1] = 0.99;
	  		  in_data[x][y-1] = 0.99;
	  		  in_data[x+1][y-1] = 0.99;
	  		  in_data[x-1][y+1] = 0.99;
	  		  in_data[x][y] = 0.99;
	  		  BSP_LCD_DrawPixel(x+1,LCD_INPUT_IMAGE_HEIGHT+y+1,255);
	  		  BSP_LCD_DrawPixel(x-1,LCD_INPUT_IMAGE_HEIGHT+y-1,255);
	  		  BSP_LCD_DrawPixel(x+1,LCD_INPUT_IMAGE_HEIGHT+y,255);
	  		  BSP_LCD_DrawPixel(x-1,LCD_INPUT_IMAGE_HEIGHT+y,255);
	  		  BSP_LCD_DrawPixel(x,LCD_INPUT_IMAGE_HEIGHT+y,255);
	  		  BSP_LCD_DrawPixel(x,LCD_INPUT_IMAGE_HEIGHT+y+1,255);
	  		  BSP_LCD_DrawPixel(x,LCD_INPUT_IMAGE_HEIGHT+y-1,255);
	  		  BSP_LCD_DrawPixel(x+1,LCD_INPUT_IMAGE_HEIGHT+y-1,255);
	  		  BSP_LCD_DrawPixel(x-1,LCD_INPUT_IMAGE_HEIGHT+y+1,255);

	  		  BSP_LCD_FillCircle(screen_state.X, screen_state.Y, 6);
//	  		  in_data_cov[x+1][y+1][0] = 0.1;
//	  		  in_data_cov[x-1][y-1][0] = 0.1;
//	  		  in_data_cov[x+1][y][0] = 0.1;
//	  		  in_data_cov[x-1][y][0] = 0.1;
//	  		  in_data_cov[x][y+1][0] = 0.1;
//	  		  in_data_cov[x][y-1][0] = 0.1;
//	  		  in_data_cov[x+1][y-1][0] = 0.1;
//	  		  in_data_cov[x-1][y+1][0] = 0.1;
//	  		  in_data_cov[x][y][0] = 0.1;
	  	  }

	  	  HAL_Delay(1);
    /* USER CODE END WHILE */

  //MX_X_CUBE_AI_Process();
    /* USER CODE BEGIN 3 */
	if( BSP_PB_GetState(BUTTON_KEY)){
		printf("\e[1;1H\e[2J");
//		for(int i = 0; i < INPUT_IMAGE_WIDTH; ++i) {
//			for (int j = 0; j < INPUT_IMAGE_HEIGHT;  ++j) {
//				printf("%0.2f,", in_data[i][j]);
//			}
//			printf("\r\n");
//		}
		MX_X_CUBE_AI_Process(in_data,out_data,1);
	 }
	//Print_Image(in_data_cov);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 7;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 14;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 654;
  hltdc.Init.AccumulatedActiveH = 485;
  hltdc.Init.TotalWidth = 660;
  hltdc.Init.TotalHeigh = 487;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 0;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 0;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 0;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 0;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 0;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{
  FMC_SDRAM_TimingTypeDef SdramTiming;

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ACP_RST_GPIO_Port, ACP_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RDX_Pin|WRX_DCX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LD3_Pin|LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NCS_MEMS_SPI_Pin CSX_Pin OTG_FS_PSO_Pin */
  GPIO_InitStruct.Pin = NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : B1_Pin MEMS_INT1_Pin MEMS_INT2_Pin TP_INT1_Pin */
  GPIO_InitStruct.Pin = B1_Pin|MEMS_INT1_Pin|MEMS_INT2_Pin|TP_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ACP_RST_Pin */
  GPIO_InitStruct.Pin = ACP_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ACP_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OC_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_HS_ID_Pin OTG_HS_DM_Pin OTG_HS_DP_Pin */
  GPIO_InitStruct.Pin = OTG_HS_ID_Pin|OTG_HS_DM_Pin|OTG_HS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : VBUS_HS_Pin */
  GPIO_InitStruct.Pin = VBUS_HS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_HS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TE_Pin */
  GPIO_InitStruct.Pin = TE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RDX_Pin WRX_DCX_Pin */
  GPIO_InitStruct.Pin = RDX_Pin|WRX_DCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD4_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
