/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
    uint8_t timeOut;         //!< TiemOut para reiniciar la máquina si se interrumpe la comunicación
    uint8_t indexStart;      //!< Indice para saber en que parte del buffer circular arranca el ID
    uint8_t cheksumRx;       //!< Cheksumm RX
    uint8_t indexWriteRx;    //!< Indice de escritura del buffer circular de recepción
    uint8_t indexReadRx;     //!< Indice de lectura del buffer circular de recepción
    uint8_t indexWriteTx;    //!< Indice de escritura del buffer circular de transmisión
    uint8_t indexReadTx;     //!< Indice de lectura del buffer circular de transmisión
    uint8_t bufferRx[256];   //!< Buffer circular de recepción
    uint8_t bufferTx[256];   //!< Buffer circular de transmisión
}_sDato ;

typedef enum Comands{
    ALIVE=0xF0,
    FIRMWARE=0xF1,
    LEDS=0x10,
    PULSADORES=0x12,
    IRVALUE=0xA0,
    TESTMOTOR=0xA1,
    SERVO=0xA2,
    DISTANCIA=0xA3,
    SPEED=0xA4,
    SERVOCONFIG=0xA5,
    NEGROIR=0xA6,
    BLANCOIR=0xA7,
    PANEO=0xA8,
    STARTCONFIG=0xEE,
} _eEstadoMEFcmd;

typedef enum ProtocolState{
    START,
    HEADER_1,
    HEADER_2,
    HEADER_3,
    NBYTES=4,
    TOKEN,
    PAYLOAD
}_eProtocolo;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
_sDato datosComSerie, datosComWifi;
_eProtocolo estadoProtocolo;
uint8_t pepe[1];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
uint8_t CDC_Transmit_FS(uint8_t *buf, uint16_t leng);  // Envia una letra
void DecodeHeader(_sDato *);
void decodeData(_sDato *);
void SendInfo(uint8_t bufferAux[], uint8_t bytes);
void comunicationsTask(_sDato *datosCom);
int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
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
  pepe[0]=0xF0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  comunicationsTask(&datosComSerie);

	  HAL_Delay(1000);             // Espera 1 segundo
    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void DecodeHeader(_sDato *datosCom){

    static uint8_t nBytes=0;

    uint8_t indexWriteRxCopy=datosCom->indexWriteRx;

    while (datosCom->indexReadRx!=indexWriteRxCopy)
    {
        switch (estadoProtocolo) {
            case START:
                if (datosCom->bufferRx[datosCom->indexReadRx++]=='U'){
                    estadoProtocolo=HEADER_1;
                    datosCom->cheksumRx=0;
                }
                break;
            case HEADER_1:
                if (datosCom->bufferRx[datosCom->indexReadRx++]=='N')
                   estadoProtocolo=HEADER_2;
                else{
                    datosCom->indexReadRx--;
                    estadoProtocolo=START;
                }
                break;
            case HEADER_2:
                if (datosCom->bufferRx[datosCom->indexReadRx++]=='E')
                    estadoProtocolo=HEADER_3;
                else{
                    datosCom->indexReadRx--;
                   estadoProtocolo=START;
                }
                break;
        case HEADER_3:
            if (datosCom->bufferRx[datosCom->indexReadRx++]=='R')
                estadoProtocolo=NBYTES;
            else{
                datosCom->indexReadRx--;
               estadoProtocolo=START;
            }
            break;
            case NBYTES:
                datosCom->indexStart=datosCom->indexReadRx;
                nBytes=datosCom->bufferRx[datosCom->indexReadRx++];
               estadoProtocolo=TOKEN;
                break;
            case TOKEN:
                if (datosCom->bufferRx[datosCom->indexReadRx++]==':'){
                   estadoProtocolo=PAYLOAD;
                    datosCom->cheksumRx ='U'^'N'^'E'^'R'^ nBytes^':';
                }
                else{
                    datosCom->indexReadRx--;
                    estadoProtocolo=START;
                }
                break;
            case PAYLOAD:
                if (nBytes>1){
                    datosCom->cheksumRx ^= datosCom->bufferRx[datosCom->indexReadRx++];
                }
                nBytes--;
                if(nBytes<=0){
                    estadoProtocolo=START;
                    if(datosCom->cheksumRx == datosCom->bufferRx[datosCom->indexReadRx]){
                        decodeData(datosCom);
                    }
                }

                break;
            default:
                estadoProtocolo=START;
                break;
        }
    }
}

void SendInfo(uint8_t bufferAux[], uint8_t bytes){
    uint8_t bufAux[20], indiceAux=0,cks=0;
    #define NBYTES  4
    bufAux[indiceAux++]='U';
    bufAux[indiceAux++]='N';
    bufAux[indiceAux++]='E';
    bufAux[indiceAux++]='R';
    bufAux[indiceAux++]=0;
    bufAux[indiceAux++]=':';
    for(uint8_t i=0; i<bytes-1; i++){
        bufAux[indiceAux++] = bufferAux[i];
    }
    bufAux[NBYTES] = bytes;
        cks=0;
    for(uint8_t i=0 ;i<indiceAux;i++){
        cks^= bufAux[i];
        datosComSerie.bufferTx[datosComSerie.indexWriteTx++]=bufAux[i];
    }
     datosComSerie.bufferTx[datosComSerie.indexWriteTx++]=cks;

     CDC_Transmit_FS((uint8_t*)datosComSerie.bufferTx, sizeof(bufAux));
}

void decodeData(_sDato *datosCom){

    uint8_t bufAux[20], indiceAux=0,cks=0;

    bufAux[indiceAux++]='U';
    bufAux[indiceAux++]='N';
    bufAux[indiceAux++]='E';
    bufAux[indiceAux++]='R';
    bufAux[indiceAux++]=0;
    bufAux[indiceAux++]=':';

    switch (datosCom->bufferRx[datosCom->indexStart+2])//ID EN LA POSICION 2
    {
    case ALIVE:

        bufAux[indiceAux++]=ALIVE;
        bufAux[indiceAux++]=0x0D;
        bufAux[NBYTES]=0x03;

    break;
    case FIRMWARE:

        bufAux[indiceAux++]=FIRMWARE;
        bufAux[indiceAux++]=0xF1;
        bufAux[NBYTES]=0x03;

    break;

    default:

        bufAux[indiceAux++]=0xFF;
        bufAux[NBYTES]=0x02;

        break;
    }
    cks=0;
    for(uint8_t i=0 ;i<indiceAux;i++){

        cks^= bufAux[i];
        datosCom->bufferTx[datosCom->indexWriteTx++]=bufAux[i];

    }

     datosCom->bufferTx[datosCom->indexWriteTx++]=cks;
}

void comunicationsTask(_sDato *datosCom){
//    if(datosCom->indexReadRx!=datosCom->indexWriteRx ){ //para recibir data
            DecodeHeader(datosCom);
//            HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
//    }
//      if(datosCom->indexReadTx!=datosCom->indexWriteTx){ //para transmitir
    	  SendInfo(pepe, 2);
//      }
}
//void aliveAutoTask(){
//    static uint32_t alive=0;
//    if(miWifi.isWifiReady()){
//        if((myTimer.read_ms()-alive)>=5000){
//            alive=myTimer.read_ms();
//            datosComWifi.bufferRx[datosComWifi.indexWriteRx+2]=ALIVE; //le sumo 2 para irme a la posicion del ID
//            datosComWifi.indexStart=datosComWifi.indexWriteRx;
//            decodeData(&datosComWifi);
//        }
//    }
//}

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
