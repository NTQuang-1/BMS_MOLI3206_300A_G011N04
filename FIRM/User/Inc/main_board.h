/***********************************************************************************************************************
 * @file   : main_board.h
 * @author : qn265366@gmail.com
 * @date   : 20/08/2026
 * @version: 1.0.0
 * @brief  : Board-level hardware configuration and MOLI3206 application binding declarations.
 **********************************************************************************************************************/
#ifndef MAIN_BOARD_H
#define MAIN_BOARD_H

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "debug.h"
#include "moli3206_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================
 * PUBLIC MACROS
 *====================================================================================================================*/

/******************************* MOLI3206 BOARD CONFIGURATION *******************************/
#ifndef MAIN_BOARD_MOLI_I2C
#define MAIN_BOARD_MOLI_I2C I2C1
#endif

#ifndef MAIN_BOARD_MOLI_I2C_CLOCK_HZ
#define MAIN_BOARD_MOLI_I2C_CLOCK_HZ 20000UL
#endif

#ifndef MAIN_BOARD_MOLI_I2C_CLK
#define MAIN_BOARD_MOLI_I2C_CLK RCC_APB1Periph_I2C1
#endif

#if (MAIN_BOARD_MOLI_I2C_CLOCK_HZ < 200UL) || (MAIN_BOARD_MOLI_I2C_CLOCK_HZ > 20000UL)
#error "MOLI3206 I2C clock must be between 200 Hz and 20 kHz"
#endif

/* Default CH32V003 I2C1 pins: SCL = PC2, SDA = PC1. */
#ifndef MAIN_BOARD_MOLI_GPIO
#define MAIN_BOARD_MOLI_GPIO GPIOC
#endif

#ifndef MAIN_BOARD_MOLI_GPIO_CLK
#define MAIN_BOARD_MOLI_GPIO_CLK RCC_APB2Periph_GPIOC
#endif

#ifndef MAIN_BOARD_MOLI_SCL_PIN
#define MAIN_BOARD_MOLI_SCL_PIN GPIO_Pin_2
#endif

#ifndef MAIN_BOARD_MOLI_SDA_PIN
#define MAIN_BOARD_MOLI_SDA_PIN GPIO_Pin_1
#endif

#ifndef MAIN_BOARD_MOLI_I2C_TIMEOUT
#define MAIN_BOARD_MOLI_I2C_TIMEOUT ((uint32_t)100000U)
#endif

#ifndef MAIN_BOARD_MOLI_ABORT_TIMEOUT
#define MAIN_BOARD_MOLI_ABORT_TIMEOUT ((uint32_t)1000U)
#endif

#ifndef MAIN_BOARD_SET_GPIO_CLK
#define MAIN_BOARD_SET_GPIO_CLK RCC_APB2Periph_GPIOA
#endif

#ifndef MAIN_BOARD_SET_3S_4S_GPIO
#define MAIN_BOARD_SET_3S_4S_GPIO GPIOA
#endif

#ifndef MAIN_BOARD_SET_3S_PIN
#define MAIN_BOARD_SET_3S_PIN GPIO_Pin_1
#endif


#ifndef MAIN_BOARD_SET_4S_PIN
#define MAIN_BOARD_SET_4S_PIN GPIO_Pin_2
#endif

/*======================================================================================================================
 * PUBLIC API PROTOTYPES
 *====================================================================================================================*/

/**
 * @brief Initialize or reinitialize the board hardware used by the MOLI3206.
 */
void main_board_MOLI3206_InitHardware(void);

/**
 * @brief Bind the board I2C, delay, and wakeup callbacks to the MOLI3206 driver.
 *
 * @return MOLI3206_OK on success, otherwise an error code.
 */
moli3206_status_t main_board_MOLI3206_Bind(void);

/**
 * @brief fn get num series battery
 */
void main_board_get_series_battery(uint8_t *num_series);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_BOARD_H */

/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
