/***********************************************************************************************************************
 * @file   : moli3206_port.c
 * @author : qn265366@gmail.com
 * @date   : 09/07/2026
 * @version: 1.0.0
 * @brief  : Hardware-independent application binding implementation for the MOLI3206 driver.
 **********************************************************************************************************************/

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "moli3206_port.h"
#include <stddef.h>

/*======================================================================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *====================================================================================================================*/
static moli3206_port_status_t moli3206_port_I2CWriteAdapter(
  uint8_t dev_addr,
  const uint8_t *data, 
  uint16_t size);
static moli3206_port_status_t moli3206_port_I2CReadAdapter(
  uint8_t dev_addr,
  uint8_t *data,
  uint16_t size);
static void moli3206_port_DelayMsAdapter(uint32_t ms);
static moli3206_status_t moli3206_port_MapStatus(moli3206_port_status_t port_status);

/*======================================================================================================================
 * MODULE STATE
 *====================================================================================================================*/
static moli3206_port_config_t moli_port_config;
static bool moli_port_configured = false;

static moli3206_t moli_dev = {
    .dev_addr = MOLI3206_I2C_ADDR_DEFAULT_7BIT,
    .crc_enabled = true,
    .multi_write_enabled = false,
    .initialized = false,
    .i2c_write = moli3206_port_I2CWriteAdapter,
    .i2c_read  = moli3206_port_I2CReadAdapter,
    .delay_ms  = moli3206_port_DelayMsAdapter
};

/*======================================================================================================================
 * PUBLIC API IMPLEMENTATION
 *====================================================================================================================*/

moli3206_status_t moli3206_port_Configure(const moli3206_port_config_t *config)
{
  if (config == NULL)
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }

  if ((config->i2c_write == NULL) || (config->i2c_read == NULL) || (config->delay_ms == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }

  if (config->dev_addr > 0x7FU)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }

  /* Copy the application interface before publishing the configured state. */
  moli_port_config = *config;

  /* Synchronize the generic device handle with the new application binding. */
  moli_dev.dev_addr = config->dev_addr;
  moli_dev.crc_enabled = config->crc_enabled;
  moli_dev.multi_write_enabled = false;
  moli_dev.initialized = false;

  moli_port_configured = true;
  return MOLI3206_OK;
}

moli3206_t *moli3206_port_GetHandle(void)
{
  if (!moli_port_configured)
  {
    return NULL;
  }

  return &moli_dev;
}

moli3206_status_t moli3206_port_Wakeup(void)
{
  if (!moli_port_configured)
  {
    return MOLI3206_ERR_NOT_READY;
  }

  if (moli_port_config.wakeup == NULL)
  {
    return MOLI3206_ERR_UNSUPPORTED;
  }

  moli3206_port_status_t port_status = 
    moli_port_config.wakeup(moli_port_config.context, MOLI3206_POWERDOWN_WAKE_LOW_MS);
  return moli3206_port_MapStatus(port_status);
}

/*======================================================================================================================
 * PRIVATE IMPLEMENTATION
 *====================================================================================================================*/

static moli3206_port_status_t moli3206_port_I2CWriteAdapter(
  uint8_t dev_addr, const uint8_t *data, uint16_t size)
{
  if ((!moli_port_configured) || (moli_port_config.i2c_write == NULL))
  {
    return MOLI3206_PORT_ERR_BUS;
  }

  return moli_port_config.i2c_write(moli_port_config.context, dev_addr, data, size);
}

static moli3206_port_status_t moli3206_port_I2CReadAdapter(
  uint8_t dev_addr,
  uint8_t *data,
  uint16_t size)
{
  if ((!moli_port_configured) || (moli_port_config.i2c_read == NULL))
  {
    return MOLI3206_PORT_ERR_BUS;
  }

  return moli_port_config.i2c_read(moli_port_config.context, dev_addr, data, size);
}

static void moli3206_port_DelayMsAdapter(uint32_t ms)
{
  if (moli_port_configured && (moli_port_config.delay_ms != NULL))
  {
    moli_port_config.delay_ms(moli_port_config.context, ms);
  }
}

static moli3206_status_t moli3206_port_MapStatus(moli3206_port_status_t port_status)
{
  switch (port_status)
  {
    case MOLI3206_PORT_OK:              return MOLI3206_OK;
    case MOLI3206_PORT_ERR_TIMEOUT:     return MOLI3206_ERR_TIMEOUT;
    case MOLI3206_PORT_ERR_NACK:
    case MOLI3206_PORT_ERR_BUS:
    case MOLI3206_PORT_ERR_ARBITRATION:
    case MOLI3206_PORT_ERR_OVERRUN:
    default:                            return MOLI3206_ERR_I2C;
  }
}

/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
