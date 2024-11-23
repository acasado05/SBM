#include "ThLCD.h"
#include "Arial12x12.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
osThreadId_t tid_ThLCD;                        // thread id

//OBJETOS SPI
 extern ARM_DRIVER_SPI Driver_SPI1;
 ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
 ARM_SPI_STATUS stat;

osThreadId_t tid_ThLCD;                       // Identificador del hilo
TIM_HandleTypeDef htim7;

//Cabeceras funciones SPI
void ThLcd (void *argument);                  // Funcion del hilo
void SPI_Callback (uint32_t event);
void initPines (void);
void delay (uint32_t n_microsegundos);
void LCD_Reset (void);
void LCD_wr_data (unsigned char data);
void LCD_wr_cmd (unsigned char cmd);
void LCD_Init (void);
void LCD_symbolToLocalBuffer_L1 (uint8_t symbol);
void LCD_symbolToLocalBuffer_L2 (uint8_t symbol);
void symbolToLocalBuffer (uint8_t line,uint8_t symbol);
void writeLine_LCD (char *miArray, uint8_t line);
void writeLCD (uint8_t line, char frase[256]);
void LCD_Clean(void);
void LCD_Update (void);
void cleanBuffer (uint8_t line);
void cleanLine (void);

//Variables del módulo
static unsigned char buffer[512];
static unsigned char positionL1 = 0;
static unsigned char positionL2 = 0;

//Prototipo del Thread 
void ThLCD (void *argument);                   // thread function
 
int Init_Thread (void) {
 
  tid_ThLCD = osThreadNew(ThLCD, NULL, NULL);
  if (tid_ThLCD == NULL) {
    return(-1);
  }
  
  LCD_Reset ();
  LCD_Init ();
  LCD_Clean ();
  LCD_Update ();
 
  return(0);
}
 
void ThLCD (void *argument) {
 
  while (1) {
    // Insert thread code here...
    LCD_Update ();
    //osThreadYield();                            // suspend thread
  }
}

/*----------------------------------------------------------------------------
 *                            FUNCIONES DEL SPI
 *---------------------------------------------------------------------------*/

void initPines(void){
  
  // Inicialiacion de los pines
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOA_CLK_ENABLE(); //PA6
  __HAL_RCC_GPIOF_CLK_ENABLE(); //PF13
  __HAL_RCC_GPIOD_CLK_ENABLE(); //PD14

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  //Inicializacion pin RESET
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
  
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
  
}

void delay (uint32_t n_microsegundos){
  
  __HAL_RCC_TIM7_CLK_ENABLE();    
  
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.Period = n_microsegundos - 1;
  
  HAL_TIM_Base_Init(&htim7);
  HAL_TIM_Base_Start(&htim7); 
  
  while((TIM7->CNT)<(n_microsegundos - 1)){}
  
  HAL_TIM_Base_Stop(&htim7); 
  
  HAL_TIM_Base_DeInit(&htim7);
  
}

void LCD_reset (void){
  
  /* Initialize the SPI driver */
  SPIdrv->Initialize(NULL);    // Inicializamos el SPI al callback
  /* Power up the SPI peripheral */
  SPIdrv->PowerControl(ARM_POWER_FULL);
  /* Configure the SPI to Master, 8-bit mode @10000 kBits/sec */
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
  /* SS line = INACTIVE = HIGH */
  //SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
  
  initPines();
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  delay(1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  delay(1001);
  
}

//Función que escribe un dato en el LCD
void LCD_wr_data (unsigned char data){
  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); //CS = 0
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);   //A0 = 1
  
  //Escribimos un dato enviandolo al SPI con ->Send
  SPIdrv->Send(&data, sizeof(data));
  
  //Esperamos a que se libere el bus SPI: Diapositiva 10 de CMSIS Driver
  do{
      stat = SPIdrv->GetStatus();
  }while(stat.busy); //Esperamos a que se libere, cuando no esté busy
  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);   //CS = 1
}

//Función que recibe un comando en el LCD
void LCD_wr_cmd (unsigned char cmd){
  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); //CS = 0
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);   //A0 = 0
  
    //Escribimos un dato enviandolo al SPI con ->Send
  SPIdrv->Send(&cmd, sizeof(cmd));
  
  //Esperamos a que se libere el bus SPI: Diapositiva 10 de CMSIS Driver
  do{
      stat = SPIdrv->GetStatus();
  }while(stat.busy); //Esperamos a que se libere, cuando no esté busy
  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);   //CS = 1
}

void LCD_init (void){  // Configuracion del LCD (no escribo nada)
  LCD_wr_cmd(0xAE);   // Power off
  LCD_wr_cmd(0xA2);   // Fija el valor de la relacion de la tension de polarizacion del LCD a 1/9
  LCD_wr_cmd(0xA0);   // El direccionamiento de la RAM de datos del display es la normal
  LCD_wr_cmd(0xC8);   // El scan en las salidas COM es el normal
  LCD_wr_cmd(0x22);   // Fija relacion de resistencias interna a 2
  LCD_wr_cmd(0x2F);   // Power on
  LCD_wr_cmd(0x40);   // Display empieza en la linea 0
  LCD_wr_cmd(0xAF);   // Display ON
  LCD_wr_cmd(0x81);   // Contraste
  LCD_wr_cmd(0X17);   // Valor de contraste a 33
  LCD_wr_cmd(0xA4);   // Display all points normal
  LCD_wr_cmd(0xA6);   // LCD Display normal
  
}

void LCD_symbolToLocalBuffer_L1 (uint8_t symbol){	//Paginas 1 y 2
  
  uint8_t i, value1, value2;
  uint16_t offset = 0;
  
  offset = 25*(symbol - ' '); // numero de la fila a la que quiero acceder
  
  for (i = 0; i < 12; i++){
    
    value1 = Arial12x12[offset+i*2+1];
    value2 = Arial12x12[offset+i*2+2];
    
    buffer[i + 0 + positionL1] = value1;
    buffer[i + 128 + positionL1] = value2;
  }
  positionL1 = positionL1 + Arial12x12[offset];
}

void LCD_symbolToLocalBuffer_L2 (uint8_t symbol){ //Paginas 3 y 4
    
  uint8_t i, value1, value2;
  uint16_t offset = 0;
  
  offset = 25*(symbol - ' '); // numero de la fila a la que quiero acceder
  
  for (i = 0; i < 12; i++){
    
    value1 = Arial12x12[offset+i*2+1];
    value2 = Arial12x12[offset+i*2+2];
    
    buffer[i + 256 + positionL2] = value1;
    buffer[i + 384 + positionL2] = value2;
  }
  positionL2 = positionL2 + Arial12x12[offset];
  
}

void moverBarraBaja(uint8_t symbol, uint8_t position)
{ 
  uint8_t i, value1, value2;
  uint16_t offset = 0;
  uint8_t positionL3 = position;
  
  offset = 25*(symbol - ' ');	// numero de la fila a la que quiero acceder
  
  for (i = 0; i < 12; i++){
    
    value1 = Arial12x12[offset+i*2+1];
    value2 = Arial12x12[offset+i*2+2];
    
    buffer[i + 384 + positionL3] = value1;
    buffer[i + 128 + positionL3] = value2;
    
  }
  
  if (positionL1 < 128){
    positionL3 = positionL3 + Arial12x12[offset];
  }else{
    positionL3 = 0;
  }
}

void symbolToLocalBuffer (uint8_t line,uint8_t symbol)
{
  if (line == 1)
    LCD_symbolToLocalBuffer_L1(symbol);

  if (line == 2)
    LCD_symbolToLocalBuffer_L2(symbol);
}

/*
  int sprintf(char *str, const char *format)
  
  str: puntero a una matriz de elementos char conde se almacena la cadena C resultante
  format: cadena que contien el texto que se escribirá en el buffer
Esta ultima, puede contener etiquetas de formato

ETIQUETAS:

- %c: Utilizada para formatear un carácter.
- %s: Utilizada para formatear una cadena de caracteres (string).
- %d o %i: Utilizada para formatear un número entero con signo.
- %u: Utilizada para formatear un número entero sin signo.
- %x o %X: Utilizada para formatear un número entero en hexadecimal.
- %o: Utilizada para formatear un número entero en octal.
- %f: Utilizada para formatear un número en punto flotante (decimal).
- %e o %E: Utilizada para formatear un número en notación científica.
- %p: Utilizada para formatear un puntero.
- %%: Utilizada para imprimir el carácter '%' literalmente.

*/
void writeLine_LCD (char *miArray, uint8_t line){
  
  int i;
  
  // Usamos la funcion symbolToLocalBuffer
  for(i = 0; i < strlen(miArray); i++){
    symbolToLocalBuffer(line, miArray[i]);
  }
  i = 0;
  
}

/**
  * @brief  Función que escribe en el LCD llamando a la función symbolToLocalBuffer
    @param  Linea sobre la que se va a escribir 
  */
void writeLCD (uint8_t line, char frase[256])
{
  //Escribir en el buffer lo que va a ser mostrado por pantalla
  if(line == 1)
  {
    for(int i = 0; frase[i] != '\0'; i++){
      symbolToLocalBuffer(line,frase[i]);
    }
  }else if(line == 2)
  {
    for(int i = 0; frase[i] != '\0'; i++){
      symbolToLocalBuffer(line,frase[i]);
    }
  }
}

void LCD_update(void){ // Sirve para mandar los datos a escribir al LCD
  int i;
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB0); // Página 0

  for(i=0;i<128;i++){
    LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB1); // Página 1

  for(i=128;i<256;i++){
   LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB2); //Página 2
  for(i=256;i<384;i++){
   LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB3); // Pagina 3


  for(i=384;i<512;i++){
   LCD_wr_data(buffer[i]);
  }
}

void cleanLine (void)
{
  positionL1 = 0;
  positionL2 = 0;
}

void cleanBuffer (uint8_t line)
{
  if(line == 1)
  {
    for(int i = 0; i<256; i++)
    {
      buffer[i] = 0x00;
    }
  }else if(line == 2)
  {
    for(int i = 256; i<512; i++)
    {
      buffer[i] = 0x00;
    }
  }
}

void LCD_Clean(void)
{
  memset(buffer, 0 , 512u); //implica añadir la libreria: #include "string.h"
  LCD_update();
}

