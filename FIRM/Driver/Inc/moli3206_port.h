/***********************************************************************************************************************
 * @file   : moli3206_port.h
 * @author : qn265366@gmail.com
 * @date   : 09/07/2026
 * @version: 1.0.0
 * @brief  : Porting layer interface for connecting the MOLI3206 driver to CH32 hardware.
 **********************************************************************************************************************/
#ifndef MOLI3206_PORT_H
#define MOLI3206_PORT_H

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "moli3206.h"

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================
 * PUBLIC TYPES
 *====================================================================================================================*/

/**
 * @brief Application-provided I2C transmit callback.
 *
 * The application owns all MCU-specific operations, including peripheral setup,
 * timeout handling, error-flag handling, and bus recovery.
 *
 * @param context User-defined application context.
 * @param dev_addr MOLI3206 7-bit I2C address.
 * @param data Pointer to transmit data.
 * @param size Number of bytes to transmit.
 * @return MOLI3206_PORT_OK on success, otherwise a port error code.
 */
typedef moli3206_port_status_t (*moli3206_port_i2c_write_fn)(
    void *context, uint8_t dev_addr, const uint8_t *data, uint16_t size);

/**
 * @brief Application-provided I2C receive callback.
 *
 * The application owns all MCU-specific operations, including peripheral setup,
 * timeout handling, error-flag handling, and bus recovery.
 *
 * @param context User-defined application context.
 * @param dev_addr MOLI3206 7-bit I2C address.
 * @param data Pointer to receive buffer.
 * @param size Number of bytes to receive.
 * @return MOLI3206_PORT_OK on success, otherwise a port error code.
 */
typedef moli3206_port_status_t (*moli3206_port_i2c_read_fn)
(void *context, uint8_t dev_addr, uint8_t *data, uint16_t size);

/**
 * @brief Application-provided millisecond delay callback.
 *
 * @param context User-defined application context.
 * @param ms Delay duration in milliseconds.
 */
typedef void (*moli3206_port_delay_ms_fn)(void *context, uint32_t ms);

/**
 * @brief Application-provided MOLI3206 wakeup callback.
 *
 * The application must perform the complete board-specific wakeup sequence,
 * including taking control of SCL, holding it low for at least low_time_ms,
 * restoring the I2C pin configuration, and leaving the bus ready for use.
 *
 * @param context User-defined application context.
 * @param low_time_ms Minimum SCL-low duration required by the device.
 * @return MOLI3206_PORT_OK on success, otherwise a port error code.
 */
typedef moli3206_port_status_t (*moli3206_port_wakeup_fn)(void *context, uint32_t low_time_ms);

/**
 * @brief Application-provided interface used by the MOLI3206 port binding layer.
 *
 * No MCU peripheral, GPIO, clock, or board-specific type is stored here.
 * The application retains ownership of all hardware initialization.
 */
typedef struct
{
  uint8_t dev_addr;
  bool crc_enabled;

  void *context;

  moli3206_port_i2c_write_fn i2c_write;
  moli3206_port_i2c_read_fn i2c_read;
  moli3206_port_delay_ms_fn delay_ms;
  moli3206_port_wakeup_fn wakeup;
} moli3206_port_config_t;

/*======================================================================================================================
 * PUBLIC API PROTOTYPES
 *====================================================================================================================*/

/**
 * @brief Bind application callbacks to the static MOLI3206 device handle.
 *
 * This function does not initialize GPIO, clocks, interrupts, or the I2C peripheral.
 * The application must initialize its hardware before calling moli3206_Init().
 *
 * The configuration is copied internally, so the caller's object does not need
 * to remain in scope after this function returns.
 *
 * @param config Pointer to the application-provided interface configuration.
 * @return MOLI3206_OK on success, otherwise an error code.
 */
moli3206_status_t moli3206_port_Configure(const moli3206_port_config_t *config);

/**
 * @brief Retrieve the static MOLI3206 device handle.
 *
 * @return Pointer to the device handle, or NULL if the port is not configured.
 */
moli3206_t *moli3206_port_GetHandle(void);

/**
 * @brief Request the application to perform the MOLI3206 powerdown wakeup sequence.
 *
 * @return MOLI3206_OK on success, MOLI3206_ERR_UNSUPPORTED when no wakeup callback
 *         was supplied, or another error code on failure.
 */
moli3206_status_t moli3206_port_Wakeup(void);

#ifdef __cplusplus
}
#endif

#endif /* MOLI3206_PORT_H */

/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
