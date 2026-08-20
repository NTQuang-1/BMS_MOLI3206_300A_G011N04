/***********************************************************************************************************************
 * @file   : main.h
 * @author : qn265366@gmail.com
 * @date   : 05/07/2026
 * @version: 1.0.0
 * @brief  : This file consist of include file .h for main.c caller
 **********************************************************************************************************************/
#ifndef MAIN_H
#define MAIN_H

/*======================================================================================================================
 * APPLICATION RUN MODE
 *====================================================================================================================*/

/**
 * @brief MCU configures MOLI3206 RAM and continues normal operation.
 */
#define USE_MCU      (0U)

/**
 * @brief MCU applies and verifies RAM configuration, then burns it into OTP once.
 */
#define BURN_OTP     (1U)

/**
 * @brief Select the active application mode.
 *
 * Select exactly one of:
 * - USE_MCU
 * - BURN_OTP
 */
#ifndef MODE_RUN
#define MODE_RUN     USE_MCU
#endif

#if ((MODE_RUN != USE_MCU) && \
     (MODE_RUN != BURN_OTP))
#error "MODE_RUN must be USE_MCU or BURN_OTP"
#endif

#endif /* MAIN_H */
/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
