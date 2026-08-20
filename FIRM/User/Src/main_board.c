/***********************************************************************************************************************
 * @file   : main_board.c
 * @author : qn265366@gmail.com
 * @date   : 10/07/2026
 * @version: 1.0.0
 * @brief  : Board-level CH32 hardware implementation and MOLI3206 application binding.
 **********************************************************************************************************************/

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "main_board.h"
#include <stddef.h>

/*======================================================================================================================
 * PRIVATE TYPES
 *====================================================================================================================*/
typedef uint32_t main_board_irq_state_t;

/*======================================================================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *====================================================================================================================*/
static moli3206_port_status_t main_board_MOLI3206_I2CWrite(
  void *context, 
  uint8_t dev_addr,
  const uint8_t *data, 
  uint16_t size);
static moli3206_port_status_t main_board_MOLI3206_I2CRead(
  void *context,
  uint8_t dev_addr,
  uint8_t *data,
  uint16_t size);
static void main_board_MOLI3206_DelayMs(void *context, uint32_t ms);
static moli3206_port_status_t main_board_MOLI3206_Wakeup(void *context, uint32_t low_time_ms);
static void main_board_MOLI3206_ClearErrorFlags(void);
static void main_board_MOLI3206_RestoreI2CState(void);
static moli3206_port_status_t main_board_MOLI3206_Abort(void);
static main_board_irq_state_t main_board_EnterCritical(void);
static void main_board_ExitCritical(main_board_irq_state_t state);
static moli3206_port_status_t main_board_MOLI3206_CheckI2CErrors(void);

/*======================================================================================================================
 * PUBLIC API IMPLEMENTATION
 *====================================================================================================================*/

void main_board_MOLI3206_InitHardware(void)
{
  GPIO_InitTypeDef gpio = {0};
  I2C_InitTypeDef i2c = {0};

  RCC_APB2PeriphClockCmd(MAIN_BOARD_SET_GPIO_CLK  |
                         MAIN_BOARD_MOLI_GPIO_CLK |
                         RCC_APB2Periph_AFIO
                         , ENABLE);

  RCC_APB1PeriphClockCmd(MAIN_BOARD_MOLI_I2C_CLK, ENABLE);

  gpio.GPIO_Pin = (uint16_t)(MAIN_BOARD_SET_3S_PIN | MAIN_BOARD_SET_4S_PIN);
  gpio.GPIO_Mode = GPIO_Mode_IPU;
  gpio.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(MAIN_BOARD_SET_3S_4S_GPIO, &gpio);

  I2C_Cmd(MAIN_BOARD_MOLI_I2C, DISABLE);
  I2C_DeInit(MAIN_BOARD_MOLI_I2C);

  gpio.GPIO_Pin   = (uint16_t)(MAIN_BOARD_MOLI_SCL_PIN | MAIN_BOARD_MOLI_SDA_PIN);
  gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
  gpio.GPIO_Speed = GPIO_Speed_30MHz;
  GPIO_Init(MAIN_BOARD_MOLI_GPIO, &gpio);

  i2c.I2C_ClockSpeed          = MAIN_BOARD_MOLI_I2C_CLOCK_HZ;
  i2c.I2C_Mode                = I2C_Mode_I2C;
  i2c.I2C_DutyCycle           = I2C_DutyCycle_2;
  i2c.I2C_OwnAddress1         = 0U;
  i2c.I2C_Ack                 = I2C_Ack_Enable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  I2C_Init(MAIN_BOARD_MOLI_I2C, &i2c);
  I2C_Cmd(MAIN_BOARD_MOLI_I2C, ENABLE);

  main_board_MOLI3206_ClearErrorFlags();
  main_board_MOLI3206_RestoreI2CState();
}

moli3206_status_t main_board_MOLI3206_Bind(void)
{
  const moli3206_port_config_t config = {
    .dev_addr    = MOLI3206_I2C_ADDR_DEFAULT_7BIT,
    .crc_enabled = true,
    .context     = NULL,
    .i2c_write   = main_board_MOLI3206_I2CWrite,
    .i2c_read    = main_board_MOLI3206_I2CRead,
    .delay_ms    = main_board_MOLI3206_DelayMs,
    .wakeup      = main_board_MOLI3206_Wakeup
  };

  return moli3206_port_Configure(&config);
}

void main_board_get_series_battery(uint8_t *num_series){
  uint8_t temp[2] = {1};
  temp[0] = GPIO_ReadInputDataBit(MAIN_BOARD_SET_3S_4S_GPIO, MAIN_BOARD_SET_3S_PIN);
  temp[1] = GPIO_ReadInputDataBit(MAIN_BOARD_SET_3S_4S_GPIO, MAIN_BOARD_SET_4S_PIN);
  *num_series = temp[0] > temp[1]?4:3;
}

/*======================================================================================================================
 * PRIVATE IMPLEMENTATION
 *====================================================================================================================*/

static moli3206_port_status_t main_board_MOLI3206_Wakeup(void *context, uint32_t low_time_ms)
{
  (void)context;

  GPIO_InitTypeDef gpio = {0};

  I2C_Cmd(MAIN_BOARD_MOLI_I2C, DISABLE);

  RCC_APB2PeriphClockCmd(MAIN_BOARD_MOLI_GPIO_CLK, ENABLE);

  gpio.GPIO_Pin   = MAIN_BOARD_MOLI_SCL_PIN;
  gpio.GPIO_Mode  = GPIO_Mode_Out_OD;
  gpio.GPIO_Speed = GPIO_Speed_30MHz;
  GPIO_Init(MAIN_BOARD_MOLI_GPIO, &gpio);

  GPIO_WriteBit(MAIN_BOARD_MOLI_GPIO, MAIN_BOARD_MOLI_SCL_PIN, Bit_RESET);
  Delay_Ms(low_time_ms);
  GPIO_WriteBit(MAIN_BOARD_MOLI_GPIO, MAIN_BOARD_MOLI_SCL_PIN, Bit_SET);

  Delay_Ms(1U);

  main_board_MOLI3206_InitHardware();
  return MOLI3206_PORT_OK;
}

static void main_board_MOLI3206_DelayMs(void *context, uint32_t ms)
{
  (void)context;
  Delay_Ms(ms);
}

static void main_board_MOLI3206_ClearErrorFlags(void)
{
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_AF) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_AF);
  }
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BERR) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BERR);
  }
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_ARLO) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_ARLO);
  }
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_OVR) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_OVR);
  }
}

static void main_board_MOLI3206_RestoreI2CState(void)
{
  I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, ENABLE);
  I2C_NACKPositionConfig(MAIN_BOARD_MOLI_I2C, I2C_NACKPosition_Current);
}

static moli3206_port_status_t main_board_MOLI3206_Abort(void)
{
  uint32_t timeout = MAIN_BOARD_MOLI_ABORT_TIMEOUT;

  I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);
  I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, ENABLE);
  I2C_NACKPositionConfig(MAIN_BOARD_MOLI_I2C, I2C_NACKPosition_Current);

  main_board_MOLI3206_ClearErrorFlags();

  /* Wait for busy flag to clear with timeout */
  while ((I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BUSY) != RESET) && (timeout > 0U))
  {
    --timeout;
  }

  /* If the bus remains busy, disable, call recovery hook, and re-enable peripheral */
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BUSY) != RESET)
  {
    I2C_Cmd(MAIN_BOARD_MOLI_I2C, DISABLE);
    
    main_board_MOLI3206_InitHardware();

    I2C_Cmd(MAIN_BOARD_MOLI_I2C, ENABLE);
    I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, ENABLE);
    I2C_NACKPositionConfig(MAIN_BOARD_MOLI_I2C, I2C_NACKPosition_Current);
    
    main_board_MOLI3206_ClearErrorFlags();
  }

  return MOLI3206_PORT_ERR_TIMEOUT;
}

static main_board_irq_state_t main_board_EnterCritical(void)
{
  uint32_t state = __get_MSTATUS() & 0x08U; /* Save only the MIE bit (bit 3) */
  __disable_irq();
  return state;
}

static void main_board_ExitCritical(main_board_irq_state_t state)
{
  if (state != 0U)
  {
    __enable_irq();
  }
}

static moli3206_port_status_t main_board_MOLI3206_CheckI2CErrors(void)
{
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_ARLO) != RESET)
  {
    /* Clear only the ARLO flag and restore state without sending STOP */
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_ARLO);
    main_board_MOLI3206_RestoreI2CState();
    return MOLI3206_PORT_ERR_ARBITRATION;
  }
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_AF) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_AF);
    I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);
    main_board_MOLI3206_RestoreI2CState();
    return MOLI3206_PORT_ERR_NACK;
  }
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BERR) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BERR);
    I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);
    main_board_MOLI3206_RestoreI2CState();
    return MOLI3206_PORT_ERR_BUS;
  }
  if (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_OVR) != RESET)
  {
    I2C_ClearFlag(MAIN_BOARD_MOLI_I2C, I2C_FLAG_OVR);
    I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);
    main_board_MOLI3206_RestoreI2CState();
    return MOLI3206_PORT_ERR_OVERRUN;
  }
  return MOLI3206_PORT_OK;
}

static moli3206_port_status_t main_board_MOLI3206_I2CWrite(
  void *context, uint8_t dev_addr, const uint8_t *data, uint16_t size)
{
  (void)context;
  if ((dev_addr > 0x7FU) || (data == NULL) || (size == 0U))
  {
    return MOLI3206_PORT_ERR_BUS;
  }

  uint32_t timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
  moli3206_port_status_t err = MOLI3206_PORT_OK;

  /* Wait for bus to be free */
  while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BUSY))
  {
    err = main_board_MOLI3206_CheckI2CErrors();
    if (err != MOLI3206_PORT_OK) return err;
    if (--timeout == 0U) return main_board_MOLI3206_Abort();
  }

  /* Generate START condition */
  I2C_GenerateSTART(MAIN_BOARD_MOLI_I2C, ENABLE);
  timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
  while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    err = main_board_MOLI3206_CheckI2CErrors();
    if (err != MOLI3206_PORT_OK) return err;
    if (--timeout == 0U) return main_board_MOLI3206_Abort();
  }

  /* Send device address in write direction */
  I2C_Send7bitAddress(MAIN_BOARD_MOLI_I2C, (uint8_t)(dev_addr << 1U), I2C_Direction_Transmitter);
  timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
  while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    err = main_board_MOLI3206_CheckI2CErrors();
    if (err != MOLI3206_PORT_OK) return err;
    if (--timeout == 0U) return main_board_MOLI3206_Abort();
  }

  /* Send data bytes */
  for (uint16_t i = 0U; i < size; ++i)
  {
    I2C_SendData(MAIN_BOARD_MOLI_I2C, data[i]);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }
  }

  /* Generate STOP condition */
  I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);

  return MOLI3206_PORT_OK;
}

static moli3206_port_status_t main_board_MOLI3206_I2CRead(
  void *context, uint8_t dev_addr, uint8_t *data, uint16_t size)
{
  (void)context;
  if ((dev_addr > 0x7FU) || (data == NULL) || (size == 0U))
  {
    return MOLI3206_PORT_ERR_BUS;
  }

  uint32_t timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
  main_board_irq_state_t irq_state;
  moli3206_port_status_t err;

  /* Explicitly initialize default peripheral states at start of read transaction */
  main_board_MOLI3206_RestoreI2CState();

  /* Wait for bus to be free */
  while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BUSY))
  {
    err = main_board_MOLI3206_CheckI2CErrors();
    if (err != MOLI3206_PORT_OK) return err;
    if (--timeout == 0U) return main_board_MOLI3206_Abort();
  }

  /* Handle three different receive paths to guarantee safe flag and ACK management */
  if (size == 1U)
  {
    /* Generate START condition */
    I2C_GenerateSTART(MAIN_BOARD_MOLI_I2C, ENABLE);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    /* Send device address in read direction */
    I2C_Send7bitAddress(MAIN_BOARD_MOLI_I2C, (uint8_t)(dev_addr << 1U), I2C_Direction_Receiver);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_ADDR) == RESET)
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    /* Disable interrupts to protect critical timing sequence */
    irq_state = main_board_EnterCritical();

    /* Disable ACK before clearing ADDR flag */
    I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, DISABLE);

    /* Clear ADDR flag */
    (void)MAIN_BOARD_MOLI_I2C->STAR1;
    (void)MAIN_BOARD_MOLI_I2C->STAR2;

    /* Generate STOP condition */
    I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);

    main_board_ExitCritical(irq_state);

    /* Wait for data to be received (RXNE) */
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_RXNE) == RESET)
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    data[0] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);

    /* Restore default ACK config */
    I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, ENABLE);
  }
  else if (size == 2U)
  {
    /* Configure POS to NACK next byte */
    I2C_NACKPositionConfig(MAIN_BOARD_MOLI_I2C, I2C_NACKPosition_Next);

    /* Generate START condition */
    I2C_GenerateSTART(MAIN_BOARD_MOLI_I2C, ENABLE);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    /* Send device address in read direction */
    I2C_Send7bitAddress(MAIN_BOARD_MOLI_I2C, (uint8_t)(dev_addr << 1U), I2C_Direction_Receiver);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_ADDR) == RESET)
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    /* Disable interrupts to protect critical timing sequence */
    irq_state = main_board_EnterCritical();

    /* Disable ACK and clear ADDR */
    I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, DISABLE);
    (void)MAIN_BOARD_MOLI_I2C->STAR1;
    (void)MAIN_BOARD_MOLI_I2C->STAR2;

    main_board_ExitCritical(irq_state);

    /* Wait for BTF (Byte Transfer Finished) */
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BTF) == RESET)
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    /* Disable interrupts to protect final stop generation and register reads */
    irq_state = main_board_EnterCritical();

    /* Generate STOP condition */
    I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);

    /* Read the two bytes */
    data[0] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);
    data[1] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);

    main_board_ExitCritical(irq_state);

    /* Restore default configurations */
    main_board_MOLI3206_RestoreI2CState();
  }
  else
  {
    /* Generate START condition */
    I2C_GenerateSTART(MAIN_BOARD_MOLI_I2C, ENABLE);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    /* Send device address in read direction */
    I2C_Send7bitAddress(MAIN_BOARD_MOLI_I2C, (uint8_t)(dev_addr << 1U), I2C_Direction_Receiver);
    timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
    while (!I2C_CheckEvent(MAIN_BOARD_MOLI_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
      err = main_board_MOLI3206_CheckI2CErrors();
      if (err != MOLI3206_PORT_OK) return err;
      if (--timeout == 0U) return main_board_MOLI3206_Abort();
    }

    for (uint16_t i = 0U; i < size; ++i)
    {
      if (i == (size - 3U))
      {
        /* Wait for BTF (3 bytes remaining) */
        timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
        while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BTF) == RESET)
        {
          err = main_board_MOLI3206_CheckI2CErrors();
          if (err != MOLI3206_PORT_OK) return err;
          if (--timeout == 0U) return main_board_MOLI3206_Abort();
        }

        /* Protect ACK disabling in critical section */
        irq_state = main_board_EnterCritical();

        /* Disable ACK for the remaining bytes */
        I2C_AcknowledgeConfig(MAIN_BOARD_MOLI_I2C, DISABLE);

        /* Read the first of the final three bytes */
        data[i] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);
        
        main_board_ExitCritical(irq_state);
        i++;

        /* Wait for BTF again (2 bytes remaining) */
        timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
        while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_BTF) == RESET)
        {
          err = main_board_MOLI3206_CheckI2CErrors();
          if (err != MOLI3206_PORT_OK) return err;
          if (--timeout == 0U) return main_board_MOLI3206_Abort();
        }

        /* Protect STOP generation and final reads in critical section */
        irq_state = main_board_EnterCritical();

        /* Generate STOP condition */
        I2C_GenerateSTOP(MAIN_BOARD_MOLI_I2C, ENABLE);

        /* Read the final two bytes */
        data[i] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);
        i++;
        data[i] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);

        main_board_ExitCritical(irq_state);
        break;
      }

      /* Standard multi-byte read path */
      timeout = MAIN_BOARD_MOLI_I2C_TIMEOUT;
      while (I2C_GetFlagStatus(MAIN_BOARD_MOLI_I2C, I2C_FLAG_RXNE) == RESET)
      {
        err = main_board_MOLI3206_CheckI2CErrors();
        if (err != MOLI3206_PORT_OK) return err;
        if (--timeout == 0U) return main_board_MOLI3206_Abort();
      }
      data[i] = I2C_ReceiveData(MAIN_BOARD_MOLI_I2C);
    }

    /* Restore default configurations */
    main_board_MOLI3206_RestoreI2CState();
  }

  return MOLI3206_PORT_OK;
}

/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
