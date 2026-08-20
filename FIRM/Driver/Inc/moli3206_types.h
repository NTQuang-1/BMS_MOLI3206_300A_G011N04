/***********************************************************************************************************************
 * @file   : moli3206_types.h
 * @author : qn265366@gmail.com
 * @date   : 09/07/2026
 * @version: 1.0.0
 * @brief  : Type definitions, structures, and error status enums for MOLI3206 driver
 **********************************************************************************************************************/
#ifndef MOLI3206_TYPES_H
#define MOLI3206_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================
 * ERROR STATUS ENUM
 *====================================================================================================================*/
typedef enum {
     MOLI3206_OK = 0,

  /* MCU software errors. */
  MOLI3206_ERR_MCU_NULL_PTR = -1,
  MOLI3206_ERR_MCU_INVALID_PARAM = -2,

  /* Hardware and communication errors. */
  MOLI3206_ERR_I2C = -3,
  MOLI3206_ERR_TIMEOUT = -4,

  /* Driver-specific errors. */
  MOLI3206_ERR_NOT_READY = -5,
  MOLI3206_ERR_VERIFY_FAILED = -6,
  MOLI3206_ERR_PROTOCOL_STATE = -7,
  MOLI3206_ERR_UNSUPPORTED = -8,

  /* OTP errors. */
  MOLI3206_ERR_OTP_ALREADY_PROGRAMMED = -9
} moli3206_status_t;

/* Port-level error codes returned by I2C callbacks */
typedef enum {
    MOLI3206_PORT_OK = 0,
    MOLI3206_PORT_ERR_TIMEOUT = -1,
    MOLI3206_PORT_ERR_NACK = -2,
    MOLI3206_PORT_ERR_BUS = -3,
    MOLI3206_PORT_ERR_ARBITRATION = -4,
    MOLI3206_PORT_ERR_OVERRUN = -5
} moli3206_port_status_t;

/*======================================================================================================================
 * LOW-LEVEL DRIVER CALLBACK STRUCTURE
 *====================================================================================================================*/
typedef moli3206_port_status_t (*moli3206_i2c_write_fn)(uint8_t dev_addr, const uint8_t *data, uint16_t size);
typedef moli3206_port_status_t (*moli3206_i2c_read_fn)(uint8_t dev_addr, uint8_t *data, uint16_t size);

typedef struct {
    uint8_t dev_addr;             /* MOLI3206 7-bit I2C address. Must match device configuration before moli3206_Init() is called. */
    bool crc_enabled;             /* Flag indicating if CRC is enabled. Must match device configuration before moli3206_Init() is called. */
    bool multi_write_enabled;     /* Flag indicating if multi-byte write is enabled on the device */
    bool initialized;             /* Flag indicating if driver has been initialized successfully */
    
    /* Low-level interface callbacks. 
     * Write and read functions must return MOLI3206_PORT_OK on success, and a port error code on failure.
     * These perform generic raw I2C master transmit and master receive.
     */
    moli3206_i2c_write_fn i2c_write;
    moli3206_i2c_read_fn i2c_read;
    void (*delay_ms)(uint32_t ms);
} moli3206_t;

/*======================================================================================================================
 * BMS DATA STRUCTURE
 *====================================================================================================================*/
typedef struct {
    uint16_t cell_voltage_raw[6];   /* Raw cell voltages for cells 1 to 6 */
    uint16_t ts1_raw;               /* TS1 thermistor raw ADC value */
    uint16_t ts2_raw;               /* TS2 thermistor raw ADC value */
    uint16_t internal_temp_raw;     /* Internal temperature raw ADC value */
    uint16_t charge_current_raw;    /* Charge current raw ADC value */
    uint16_t discharge_current_raw; /* Discharge current raw ADC value */
    uint8_t status1;                /* BSTATUS1 register value */
    uint8_t status2;                /* BSTATUS2 register value */
    uint8_t status3;                /* BSTATUS3 register value */
    uint8_t flag1;                  /* BFLAG1 register value */
    uint8_t flag2;                  /* BFLAG2 register value */
    bool chg_fet_enabled;           /* Charging FET physical state */
    bool dsg_fet_enabled;           /* Discharging FET physical state */
    bool balancing_active;          /* Balancing is currently running */
    bool balancing_allowed;         /* Balancing control is not disabled */
} moli3206_data_t;

#ifdef __cplusplus
}
#endif

#endif // MOLI3206_TYPES_H
/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
