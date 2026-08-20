/***********************************************************************************************************************
 * @file   : moli3206.h
 * @author : qn265366@gmail.com
 * @date   : 09/07/2026
 * @version: 1.0.0
 * @brief  : This file implements the public MOLI3206 driver API.
 **********************************************************************************************************************/
#ifndef MOLI3206_H
#define MOLI3206_H

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "moli3206_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================
 * PUBLIC MACROS
 *====================================================================================================================*/
/********************************* I2C ADDRESS ********************************/
#define MOLI3206_I2C_ADDR_DEFAULT_7BIT ((uint8_t)0x10U)

/****************************** PROTOCOL CONSTANTS ******************************/
#define MOLI3206_CMD_WRITE                  ((uint8_t)0x04U)
#define MOLI3206_CMD_READ                   ((uint8_t)0x05U)

/* Register address is 16-bit; current address space uses MSB = 0. */
#define MOLI3206_REG_ADDR_MSB               ((uint8_t)0x00U)

#define MOLI3206_PACKET_CAPACITY            (32U)

#define MOLI3206_MAX_WRITE_LEN_WITH_CRC     ((uint16_t)27U)
#define MOLI3206_MAX_WRITE_LEN_NO_CRC       ((uint16_t)28U)
#define MOLI3206_MAX_READ_LEN               ((uint16_t)255U)

#define MOLI3206_STARTUP_DELAY_MS           ((uint32_t)25U)
#define MOLI3206_POWERDOWN_WAKE_LOW_MS      ((uint32_t)21U)

/**************************** CONFIG WRITABLE MASKS *****************************/
#define MOLI3206_SCONF1_WRITABLE_MASK       ((uint8_t)0xF7U)
#define MOLI3206_SCONF2_WRITABLE_MASK       ((uint8_t)0xFDU)
#define MOLI3206_OV_OWT_WRITABLE_MASK       ((uint8_t)0x7FU)
#define MOLI3206_LV_UVT_WRITABLE_MASK       ((uint8_t)0x7FU)
#define MOLI3206_BAL_CONF_WRITABLE_MASK     ((uint8_t)0x03U)
#define MOLI3206_DIS1_WRITABLE_MASK         ((uint8_t)0xBFU)
#define MOLI3206_DIS2_WRITABLE_MASK         ((uint8_t)0x1FU)
#define MOLI3206_DIS3_WRITABLE_MASK         ((uint8_t)0xEFU)
#define MOLI3206_FAULT_MASK2_WRITABLE_MASK  ((uint8_t)0x3FU)
#define MOLI3206_OTP_ACCESS_WRITABLE_MASK   ((uint8_t)0x0FU)

/******************************** CONFIG OTP ***********************************/
#define MOLI3206_OTP_CONFIG_START_ADDR           ((uint8_t)0x00U)
#define MOLI3206_OTP_CONFIG_LENGTH               ((uint16_t)27U)
#define MOLI3206_OTP_CONF_IDENTIFY_ADDR          ((uint8_t)0x3FU)
#define MOLI3206_OTP_CONF_IDENTIFY_VALID_VALUE   ((uint8_t)0x4DU)

/********************************CONFIG REG ***********************************/
#define MO_ID          ((uint8_t)(0x40U))
#define MO_SCONF1      ((uint8_t)(0x41U))
#define MO_SCONF2      ((uint8_t)(0x42U))
#define MO_OVR         ((uint8_t)(0x43U))
#define MO_OV_OWT      ((uint8_t)(0x44U))
#define MO_OVT_OV      ((uint8_t)(0x45U))
#define MO_UVR         ((uint8_t)(0x46U))
#define MO_UV          ((uint8_t)(0x47U))
#define MO_LV_UVT      ((uint8_t)(0x48U))
#define MO_BALV        ((uint8_t)(0x49U))
#define MO_BAL_OCD     ((uint8_t)(0x4AU))
#define MO_SC          ((uint8_t)(0x4BU))
#define MO_OCC         ((uint8_t)(0x4CU))
#define MO_OTC         ((uint8_t)(0x4DU))
#define MO_OTCR        ((uint8_t)(0x4EU))
#define MO_OTD         ((uint8_t)(0x4FU))
#define MO_OTDR        ((uint8_t)(0x50U))
#define MO_UTC         ((uint8_t)(0x51U))
#define MO_UTCR        ((uint8_t)(0x52U))
#define MO_UTD         ((uint8_t)(0x53U))
#define MO_UTDR        ((uint8_t)(0x54U))
#define MO_OCD1V       ((uint8_t)(0x55U))
#define MO_OCD2V       ((uint8_t)(0x56U))
#define MO_OCCV        ((uint8_t)(0x57U))
#define MO_I2C_CONF    ((uint8_t)(0x58U))
#define MO_SLEEP_CONF  ((uint8_t)(0x59U))
#define MO_BAL_CONF    ((uint8_t)(0x5AU))

/********************************STA REG************************************/
#define MO_BSTATUS1    ((uint8_t)(0x60U))
#define MO_BSTATUS2    ((uint8_t)(0x61U))
#define MO_BSTATUS3    ((uint8_t)(0x62U))
#define MO_BFLAG1      ((uint8_t)(0x63U))
#define MO_BFLAG2      ((uint8_t)(0x64U))
#define MO_RD1         ((uint8_t)(0x65U)) // 16 BIT, TS1 (LSB) 0x65, TS1 (MSB) 0x66
#define MO_RD2         ((uint8_t)(0x67U)) // 16 BIT, TS2 (LSB) 0x67, TS2 (MSB) 0x68
#define MO_TEMPI       ((uint8_t)(0x69U)) // 16 BIT, TI (LSB) 0x69, TI (MSB) 0x6A
#define MO_CUR_CHG     ((uint8_t)(0x6BU)) // 16 BIT, cur_chg (LSB) 0x6B, cur_chg (MSB) 0x6C
#define MO_CUR_DSG     ((uint8_t)(0x6DU)) // 16 BIT, cur_dsg (LSB) 0x6D, cur_dsg (MSB) 0x6E
#define MO_CELL1       ((uint8_t)(0x6FU)) // 16 BIT, CELL1 (LSB) 0x6F, CELL1 (MSB) 0x70
#define MO_CELL2       ((uint8_t)(0x71U)) // 16 BIT, CELL2 (LSB) 0x71, CELL2 (MSB) 0x72
#define MO_CELL3       ((uint8_t)(0x73U)) // 16 BIT, CELL3 (LSB) 0x73, CELL3 (MSB) 0x74
#define MO_CELL4       ((uint8_t)(0x75U)) // 16 BIT, CELL4 (LSB) 0x75, CELL4 (MSB) 0x76
#define MO_CELL5       ((uint8_t)(0x77U)) // 16 BIT, CELL5 (LSB) 0x77, CELL5 (MSB) 0x78
#define MO_CELL6       ((uint8_t)(0x79U)) // 16 BIT, CELL6 (LSB) 0x79, CELL6 (MSB) 0x7A
#define MO_CHG_LD_TEST ((uint8_t)(0x7BU))

/********************************CTRL REG************************************/
#define MO_DIS1        ((uint8_t)(0x80U))
#define MO_DIS2        ((uint8_t)(0x81U))
#define MO_DIS3        ((uint8_t)(0x82U))
#define MO_FAULT_MASK1 ((uint8_t)(0x83U))
#define MO_FAULT_MASK2 ((uint8_t)(0x84U))
#define MO_ALARM_EN1   ((uint8_t)(0x85U))
#define MO_ALARM_EN2   ((uint8_t)(0x86U))
#define MO_OTP_ACCESS  ((uint8_t)(0x87U))
#define MO_RST         ((uint8_t)(0x88U))

/***************************************************************************************************
 ************************************************ REGISTER MASK ************************************
 **************************************************************************************************/

/**
 * @defgroup CONFIG REGISTER MASKs
 */

/********************************SCONF1 MASK********************************/
#define SCONF1_ENMOSR       ((uint8_t)(0x80U))
#define SCONF1_CHG_HYS      ((uint8_t)(0x40U))

#define SCONF1_TC_MASK      ((uint8_t)(0x30U))
#define SCONF1_TC_2S        ((uint8_t)(0x00U)) // Delay over temperature, active if CHG_HYS = 1
#define SCONF1_TC_4S        ((uint8_t)(0x10U))
#define SCONF1_TC_6S        ((uint8_t)(0x20U))
#define SCONF1_TC_8S        ((uint8_t)(0x30U))

#define SCONF1_BALANCE_MODE ((uint8_t)(0x04U))

#define SCONF1_CN_MASK  ((uint8_t)(0x03U))
#define SCONF1_CN_3S    ((uint8_t)(0x00U)) // Num of cell battery connect to pack
#define SCONF1_CN_4S    ((uint8_t)(0x01U))
#define SCONF1_CN_5S    ((uint8_t)(0x02U))
#define SCONF1_CN_6S    ((uint8_t)(0x03U))

#define SCONF1_DEFAULT  ((uint8_t)(0x07U)) // Balance_mode = 1, CN = 6S

/********************************SCONF2 MASK********************************/
#define SCONF2_MASK         ((uint8_t)(0xFFU))
#define SCONF2_TS1_OFF      ((uint8_t)(0x80U)) // NTC 1 -> 0: TS1 on, 1: TS1 off
#define SCONF2_TS2_OFF      ((uint8_t)(0x40U)) // NTC 2 -> 0: TS2 on, 1: TS2 off
#define SCONF2_OCRA         ((uint8_t)(0x20U)) // Auto recover after Over current discharge. 0: off, 1: on fixed in 32s
#define SCONF2_CHG_OVR      ((uint8_t)(0x10U)) /* Auto-recovery configuration:
                                                0: Charger removal not required
                                                1: Charger removal required 
                                              */
#define SCONF2_LUVR         ((uint8_t)(0x08U))  /* Auto-recovery configuration:
                                                 0: Load removal not required
                                                 1: Load removal required
                                               */
#define SCONF2_EOW          ((uint8_t)(0x04U))  /* Open-wire protection bit: 0: Disabled, 1: Enabled */
#define SCONF2_ENMOS        ((uint8_t)(0x01U))  /* ENMOS: MOSFET Cross-Conduction Enable
                                                 0: Normal Operation. (MOSFETs remain open during fault)
                                                 1: Forced Cross-MOSFET ON to bypass body diode if 
                                                 opposite state detected during charge/discharge protections.
                                               */

#define SCONF2_DEFAULT      ((uint8_t)(0x04U))

/********************************OV_OWT MASK********************************/
#define OV_OWT_DELAY_0S     ((uint8_t)(0x00U))   // Tow = 0 s
#define OV_OWT_DELAY_2S     ((uint8_t)(0x20U))   // Tow = 2 second
#define OV_OWT_DELAY_4S     ((uint8_t)(0x40U))   // Tow = 4 second
#define OV_OWT_DELAY_8S     ((uint8_t)(0x60U))   // Tow = 8 second

#define OV_OWT_DEFAULT      ((uint8_t)(0x71U))   // Tow = 8s

/********************************OVT_OV MASK********************************/
#define OVT_OV_MASK         ((uint8_t)(0xC0U))
#define OVT_OV_0_2S         ((uint8_t)(0x00U))   // T_ovt = 0.2 second
#define OVT_OV_1S           ((uint8_t)(0x40U))   // T_ovt = 1 second
#define OVT_OV_2S           ((uint8_t)(0x80U))   // T_ovt = 2 second
#define OVT_OV_4S           ((uint8_t)(0xC0U))   // T_ovt = 4 second

#define OVT_OV_DEFAULT      ((uint8_t)(0x74U))   // T_ov = 1s

/********************************LV_UVT MASK********************************/
#define LV_UVT_ENA_CHARGE   ((uint8_t)(0x00U))   // Enable charging during under-voltage

#define LV_UVT_DIS_MASK     ((uint8_t)(0x70U))
#define LV_UVT_DIS_750MV    ((uint8_t)(0x10U))   // Disable charging during under-voltage 750mV threshold
#define LV_UVT_DIS_1000MV   ((uint8_t)(0x20U))   // Disable charging during under-voltage 1000mV threshold
#define LV_UVT_DIS_1250MV   ((uint8_t)(0x30U))   // Disable charging during under-voltage 1250mV threshold
#define LV_UVT_DIS_1500MV   ((uint8_t)(0x40U))   // Disable charging during under-voltage 1500mV threshold
#define LV_UVT_DIS_1750MV   ((uint8_t)(0x50U))   // Disable charging during under-voltage 1750mV threshold
#define LV_UVT_DIS_2000MV   ((uint8_t)(0x60U))   // Disable charging during under-voltage 2000mV threshold
#define LV_UVT_DIS_2250MV   ((uint8_t)(0x70U))   // Disable charging during under-voltage 2250mV threshold

#define LV_UVT_BALT         ((uint8_t)(0x08U))   // 0: 0.5s ; 1: 4s

#define LV_UVT_MASK         ((uint8_t)(0x03U))
#define LV_UVT_0_5S         ((uint8_t)(0x00U))   // Tuv = 0.5s
#define LV_UVT_1S           ((uint8_t)(0x01U))   // Tuv = 1 s
#define LV_UVT_2S           ((uint8_t)(0x02U))   // Tuv = 2 s
#define LV_UVT_4S           ((uint8_t)(0x03U))   // Tuv = 4 s

#define LV_UVT_DEFAULT      ((uint8_t)(0x05U))   // balt = 0 Tuv = 1s

/********************************BAL_OCD MASK********************************/
#define BAL_OCD_BALS_MASK                ((uint8_t)(0xC0U))
#define BAL_OCD_BALS_CHARGING            ((uint8_t)(0x00U)) // Check and activate balancing during charging
#define BAL_OCD_BALS_CHARGE_OR_IDLE      ((uint8_t)(0x40U)) // Activate balancing during charging or idle
#define BAL_OCD_BALS_CHARGE_OR_DISCHARGE ((uint8_t)(0x80U)) // Activate balancing during charging or discharging
#define BAL_OCD_BALS_ALL_STATE           ((uint8_t)(0xC0U)) // Always balancing

#define BAL_OCD_BALD_MASK                ((uint8_t)(0x30U))
#define BAL_OCD_BALD_DEVIATION           ((uint8_t)(0x00U)) // voltage deviation for activate balance
#define BAL_OCD_BALD_DEVIATION_20MV      ((uint8_t)(0x10U)) // V_diff = 20mV
#define BAL_OCD_BALD_DEVIATION_30MV      ((uint8_t)(0x20U)) // V_diff = 30mV
#define BAL_OCD_BALD_DEVIATION_50MV      ((uint8_t)(0x30U)) // V_diff = 50mV

#define BAL_OCD_OCD2T_MASK               ((uint8_t)(0x0CU))
#define BAL_OCD_OCD2T_200MS              ((uint8_t)(0x00U)) // T_ocd2 = 200ms
#define BAL_OCD_OCD2T_700MS              ((uint8_t)(0x04U)) // T_ocd2 = 700ms
#define BAL_OCD_OCD2T_2S                 ((uint8_t)(0x08U)) // T_ocd2 = 2S
#define BAL_OCD_OCD2T_4S                 ((uint8_t)(0x0CU)) // T_ocd2 = 4S

#define BAL_OCD_OCD1T_MASK               ((uint8_t)(0x03U))
#define BAL_OCD_OCD1T_500MS              ((uint8_t)(0x00U)) // T_ocd1 = 500ms
#define BAL_OCD_OCD1T_1_45S              ((uint8_t)(0x01U)) // T_ocd1 = 1.45s
#define BAL_OCD_OCD1T_3S                 ((uint8_t)(0x02U)) // T_ocd1 = 3s
#define BAL_OCD_OCD1T_6S                 ((uint8_t)(0x03U)) // T_ocd1 = 6s

#define BAL_OCD_DEFAULT                  ((uint8_t)(0xF5U)) // Always balancing, 
                                                            // V_diff = 50mV,
                                                            // T_ocd2 = 700mS,
                                                            // T_ocd1 = 1.45s

/*************************************SC MASK********************************/
#define SC_SCT_MASK                      ((uint8_t)(0xE0U))
#define SC_SCT_100US                     ((uint8_t)(0x00U)) // Tsc = 100us
#define SC_SCT_200US                     ((uint8_t)(0x20U)) // Tsc = 200us
#define SC_SCT_400US                     ((uint8_t)(0x40U)) // Tsc = 400us
#define SC_SCT_600US                     ((uint8_t)(0x60U)) // Tsc = 600us
#define SC_SCT_800US                     ((uint8_t)(0x80U)) // Tsc = 800us
#define SC_SCT_1MS                       ((uint8_t)(0xA0U)) // Tsc = 1ms
#define SC_SCT_1_2MS                     ((uint8_t)(0xC0U)) // Tsc = 1.2ms
#define SC_SCT_1_6MS                     ((uint8_t)(0xE0U)) // Tsc = 1.6ms

#define SC_SCT_DEFAULT                   ((uint8_t)(0x87U)) // Tsc = 800uS

/*************************************OCC MASK********************************/
#define OCC_DHS_MASK                     ((uint8_t)(0xE0U))
#define OCC_DHS_100UV                    ((uint8_t)(0x00U)) // Vdhs = 100 uV
#define OCC_DHS_200UV                    ((uint8_t)(0x20U)) // Vdhs = 200 uV
#define OCC_DHS_300UV                    ((uint8_t)(0x40U)) // Vdhs = 300 uV
#define OCC_DHS_400UV                    ((uint8_t)(0x60U)) // Vdhs = 400 uV
#define OCC_DHS_500UV                    ((uint8_t)(0x80U)) // Vdhs = 500 uV
#define OCC_DHS_1000UV                   ((uint8_t)(0xA0U)) // Vdhs = 1000uV
#define OCC_DHS_1500UV                   ((uint8_t)(0xC0U)) // Vdhs = 1500uV
#define OCC_DHS_2000UV                   ((uint8_t)(0xE0U)) // Vdhs = 2000uV

#define OCC_CHS_MASK                     ((uint8_t)(0x1CU))
#define OCC_CHS_100UV                    ((uint8_t)(0x00U)) // V_chs = 100 uV
#define OCC_CHS_200UV                    ((uint8_t)(0x04U)) // V_chs = 200 uV
#define OCC_CHS_300UV                    ((uint8_t)(0x08U)) // V_chs = 300 uV
#define OCC_CHS_400UV                    ((uint8_t)(0x0CU)) // V_chs = 400 uV
#define OCC_CHS_500UV                    ((uint8_t)(0x10U)) // V_chs = 500 uV
#define OCC_CHS_1000UV                   ((uint8_t)(0x14U)) // V_chs = 1000uV
#define OCC_CHS_1500UV                   ((uint8_t)(0x18U)) // V_chs = 1500uV
#define OCC_CHS_2000UV                   ((uint8_t)(0x1CU)) // V_chs = 2000uV

#define OCC_OCCT_MASK                    ((uint8_t)(0x03U))
#define OCC_OCCT_100MS                   ((uint8_t)(0x00U)) // V_occt = 100ms
#define OCC_OCCT_1S                      ((uint8_t)(0x01U)) // V_occt = 1s
#define OCC_OCCT_2S                      ((uint8_t)(0x02U)) // V_occt = 2s
#define OCC_OCCT_8S                      ((uint8_t)(0x03U)) // V_occt = 8s

#define OCC_DEFAULT                      ((uint8_t)(0xF5U)) // Vdhs = 2000uV, V_chs = 1000uV, T_occ = 1S

/************************************I2C_CONF MASK*******************************/
#define MOLI3206_I2C_CONF_CRC_EN_MASK    ((uint8_t)(0x80U))
#define MOLI3206_I2C_CONF_ADDR_MASK      ((uint8_t)(0x7FU))
#define MOLI3206_I2C_CONF_DEFAULT        ((uint8_t)(MOLI3206_I2C_CONF_CRC_EN_MASK | MOLI3206_I2C_ADDR_DEFAULT_7BIT))

/*********************************SLEEP_CONF MASK********************************/
#define SLEEP_CONF_CONF_SLEEP_DELAY      ((uint8_t)(0x80U)) // 0: 8 minutes, 1: 1 minute

#define SLEEP_CONF_SLEEP_A_MASK          ((uint8_t)(0x70U))
#define SLEEP_CONF_SLEEP_A_0             ((uint8_t)(0x00U)) // No sleep
#define SLEEP_CONF_SLEEP_A_4             ((uint8_t)(0x10U)) // Tsleep = 4*100 ms
#define SLEEP_CONF_SLEEP_A_8             ((uint8_t)(0x20U)) // Tsleep = 8*100 ms
#define SLEEP_CONF_SLEEP_A_16            ((uint8_t)(0x30U))
#define SLEEP_CONF_SLEEP_A_32            ((uint8_t)(0x40U))
#define SLEEP_CONF_SLEEP_A_64            ((uint8_t)(0x50U))
#define SLEEP_CONF_SLEEP_A_128           ((uint8_t)(0x60U))
#define SLEEP_CONF_SLEEP_A_256           ((uint8_t)(0x70U))

#define SLEEP_CONF_SLEEP_V2_MASK         ((uint8_t)(0x0CU))
#define SLEEP_CONF_SLEEP_V2_200UV        ((uint8_t)(0x00U)) // Threshold volt in R_shunt for exit sleep while discharge
#define SLEEP_CONF_SLEEP_V2_500UV        ((uint8_t)(0x04U))
#define SLEEP_CONF_SLEEP_V2_800UV        ((uint8_t)(0x08U))
#define SLEEP_CONF_SLEEP_V2_1000UV       ((uint8_t)(0x0CU))

#define SLEEP_CONF_SLEEP_V1_MASK         ((uint8_t)(0x03U))
#define SLEEP_CONF_SLEEP_V1_200UV        ((uint8_t)(0x00U)) // Threshold volt in R_shunt for exit sleep while discharge
#define SLEEP_CONF_SLEEP_V1_500UV        ((uint8_t)(0x01U))
#define SLEEP_CONF_SLEEP_V1_800UV        ((uint8_t)(0x02U))
#define SLEEP_CONF_SLEEP_V1_1000UV       ((uint8_t)(0x03U))

#define SLEEP_CONF_DEFAULT               ((uint8_t)(0x55U))

/*********************************BAL_CONF MASK********************************/
#define BAL_CONF_BALT_NCHGING_MASK       ((uint8_t)(0x03U))
#define BAL_CONF_BALT_NCHGING_400MS      ((uint8_t)(0x00U)) // Tbalance while state no charge
#define BAL_CONF_BALT_NCHGING_1S         ((uint8_t)(0x01U))
#define BAL_CONF_BALT_NCHGING_3S         ((uint8_t)(0x02U))
#define BAL_CONF_BALT_NCHGING_29S        ((uint8_t)(0x03U))

/**
 * @defgroup : STATUS REGISTER MASK
 */

/***********************************BSTATUS1 MASK***********************************/
#define BSTATUS1_LV   ((uint8_t)(0x80U))
#define BSTATUS1_OW   ((uint8_t)(0x40U))
#define BSTATUS1_SC   ((uint8_t)(0x20U))
#define BSTATUS1_OCC  ((uint8_t)(0x10U))
#define BSTATUS1_OCD1 ((uint8_t)(0x08U))
#define BSTATUS1_OCD2 ((uint8_t)(0x04U))
#define BSTATUS1_UV   ((uint8_t)(0x02U))
#define BSTATUS1_OV   ((uint8_t)(0x01U))

/***********************************BSTATUS2 MASK***********************************/
#define BSTATUS2_SLEEP ((uint8_t)(0x40U))
#define BSTATUS2_PRO   ((uint8_t)(0x20U))
#define BSTATUS2_OTI   ((uint8_t)(0x10U))
#define BSTATUS2_OTD   ((uint8_t)(0x08U))
#define BSTATUS2_UTD   ((uint8_t)(0x04U))
#define BSTATUS2_OTC   ((uint8_t)(0x02U))
#define BSTATUS2_UTC   ((uint8_t)(0x01U))

/***********************************BSTATUS3 MASK***********************************/
#define BSTATUS3_BAL     ((uint8_t)(0x80U))
#define BSTATUS3_PD      ((uint8_t)(0x40U))
#define BSTATUS3_CTLD    ((uint8_t)(0x20U))
#define BSTATUS3_CTLC    ((uint8_t)(0x10U))
#define BSTATUS3_CHGING  ((uint8_t)(0x08U))
#define BSTATUS3_DSGING  ((uint8_t)(0x04U))
#define BSTATUS3_CHG_FET ((uint8_t)(0x02U))
#define BSTATUS3_DSG_FET ((uint8_t)(0x01U))

/************************************BFLAG1 MASK************************************/
#define BFLAG1_LV_FLAG   ((uint8_t)(0x80U))
#define BFLAG1_OW_FLAG   ((uint8_t)(0x40U))
#define BFLAG1_SC_FLAG   ((uint8_t)(0x20U))
#define BFLAG1_OCC_FLAG  ((uint8_t)(0x10U))
#define BFLAG1_OCD1_FLAG ((uint8_t)(0x08U))
#define BFLAG1_OCD2_FLAG ((uint8_t)(0x04U))
#define BFLAG1_UV_FLAG   ((uint8_t)(0x02U))
#define BFLAG1_OV_FLAG   ((uint8_t)(0x01U))

/************************************BFLAG2 MASK************************************/
#define BFLAG2_SLEEP_FLAG         ((uint8_t)(0x80U))
#define BFLAG2_STA_CONVERT_FLAG   ((uint8_t)(0x40U))
#define BFLAG2_RST_FLAG           ((uint8_t)(0x20U))
#define BFLAG2_OTI_FLAG           ((uint8_t)(0x10U))
#define BFLAG2_OTD_FLAG           ((uint8_t)(0x08U))
#define BFLAG2_UTD_FLAG           ((uint8_t)(0x04U))
#define BFLAG2_OTC_FLAG           ((uint8_t)(0x02U))
#define BFLAG2_UTC_FLAG           ((uint8_t)(0x01U))

/********************************CHG_LD_TEST MASK***********************************/
#define CHG_LD_TEST_LOAD_RM       ((uint8_t)(0x20U)) // 0: Load attached, 1: Load removed

#define CHG_LD_TEST_CHARGE        ((uint8_t)(0x08U)) // 01: Charger plugged      
#define CHG_LD_TEST_NO_CHARGE     ((uint8_t)(0x10U)) // 10: Charger removed
#define CHG_LD_TEST_CHARGE_MASK   ((uint8_t)(0x18U))
#define CHG_LD_TEST_TSI_OFFSET    ((uint8_t)(0x07U)) // Internal temperature adjustment value

/**
 * @defgroup:  CONTROL REGISTER MASK
 */

/*************************************DIS1 MASK*************************************/
#define DIS1_DIS_LV               ((uint8_t)(0x80U)) // 0: Enable, 1 Disable
#define DIS1_DIS_SC               ((uint8_t)(0x20U))
#define DIS1_DIS_OCC              ((uint8_t)(0x10U))
#define DIS1_DIS_OCD1             ((uint8_t)(0x08U))
#define DIS1_DIS_OCD2             ((uint8_t)(0x04U))
#define DIS1_DIS_UV               ((uint8_t)(0x02U))
#define DIS1_DIS_OV               ((uint8_t)(0x01U))

/*************************************DIS2 MASK*************************************/
#define DIS2_DIS_OTI              ((uint8_t)(0x10U)) // Over Temperature Internal
#define DIS2_DIS_OTD              ((uint8_t)(0x08U)) // Over Temp Discharge
#define DIS2_DIS_UTD              ((uint8_t)(0x04U)) // Under Temp Discharge
#define DIS2_DIS_OTC              ((uint8_t)(0x02U)) // Over Temp Charge
#define DIS2_DIS_UTC              ((uint8_t)(0x01U)) // Under Temp Charge

/*************************************DIS3 MASK*************************************/
#define DIS3_DIS_BAL              ((uint8_t)(0x80U)) // Balance
#define DIS3_DIS_PD               ((uint8_t)(0x40U)) // Enable\Disable Normal -> PowerDown
#define DIS3_ENTER_PD             ((uint8_t)(0x20U)) // exchange Normal-> PowerDown
#define DIS3_EN_LR                ((uint8_t)(0x08U)) // Check Load
#define DIS3_EN_CR                ((uint8_t)(0x04U)) // Check charge
#define DIS3_DIS_CHG_FET          ((uint8_t)(0x02U))
#define DIS3_DIS_DSG_FET          ((uint8_t)(0x01U))

/***********************************FAULT MASK1 ************************************/
#define FAULT_MASK1_MASK_LV       ((uint8_t)(0x80U)) // FET while Low voltage charge
#define FAULT_MASK1_MASK_OW       ((uint8_t)(0x40U)) // FET while Open wire protection
#define FAULT_MASK1_MASK_SC       ((uint8_t)(0x20U)) // FET while Short circuit protection
#define FAULT_MASK1_MASK_OCC      ((uint8_t)(0x10U)) // FET while Over current charge protection
#define FAULT_MASK1_MASK_OCD1     ((uint8_t)(0x08U)) // FET while Over current discharge 1 protection
#define FAULT_MASK1_MASK_OCD2     ((uint8_t)(0x04U)) // FET while Over current discharge 2 protection
#define FAULT_MASK1_MASK_UV       ((uint8_t)(0x02U)) // FET while Under voltage protection
#define FAULT_MASK1_MASK_OV       ((uint8_t)(0x01U)) // FET while Over voltage protection

/***********************************FAULT MASK2 ************************************/
#define FAULT_MASK2_I2C_MULTI_W_EN     ((uint8_t)(0x20U)) // I2C multiple bytes write enable
#define FAULT_MASK2_TESTMODE_DISABLE   ((uint8_t)(0x10U)) // Test mode disable
#define FAULT_MASK2_MASK_OTD           ((uint8_t)(0x08U)) // Over temperature discharge protection
#define FAULT_MASK2_MASK_UTD           ((uint8_t)(0x04U)) // Under temperature discharge protection
#define FAULT_MASK2_MASK_OTC           ((uint8_t)(0x02U)) // Over temperature charge protection
#define FAULT_MASK2_MASK_UTC           ((uint8_t)(0x01U)) // Under temperature charge protection

/****************************ALARM PIN EN1 MASK ************************************/
#define ALARM_EN_LV           ((uint8_t)(0x80U)) // Low voltage charge
#define ALARM_EN_OW           ((uint8_t)(0x40U)) // Open wire protection
#define ALARM_EN_SC           ((uint8_t)(0x20U)) // Short circuit protection
#define ALARM_EN_OCC          ((uint8_t)(0x10U)) // Over current charge protection
#define ALARM_EN_OCD1         ((uint8_t)(0x08U)) // Over current discharge 1 protection
#define ALARM_EN_OCD2         ((uint8_t)(0x04U)) // Over current discharge 2 protection
#define ALARM_EN_UV           ((uint8_t)(0x02U)) // Under voltage protection
#define ALARM_EN_OV           ((uint8_t)(0x01U)) // Over voltage protection
#define ALARM_PIN_EN1_MASK    ((uint8_t)(0xFFU))

/***********************************ALARM PIN EN2 MASK ********************************/
#define ALARM_EN_SLEEP        ((uint8_t)(0x80U)) // Sleep mode
#define ALARM_EN_STA_CONVERT  ((uint8_t)(0x40U)) // Normal <=> Powerdown transition
#define ALARM_EN_RST          ((uint8_t)(0x20U)) // System reset
#define ALARM_EN_OTI          ((uint8_t)(0x10U)) // Internal over temperature protection
#define ALARM_EN_OTD          ((uint8_t)(0x08U)) // Over temperature discharge protection
#define ALARM_EN_UTD          ((uint8_t)(0x04U)) // Under temperature discharge protection
#define ALARM_EN_OTC          ((uint8_t)(0x02U)) // Over temperature charge protection
#define ALARM_EN_UTC          ((uint8_t)(0x01U)) // Under temperature charge protection
#define ALARM_PIN_EN2_MASK    ((uint8_t)(0xFFU))

/************************* CONF OTP ACCESS EN MASK ********************************/
#define CONF_OTP_READ_ENABLE      ((uint8_t)(0x05U)) // Enable CONF OTP read
#define CONF_OTP_READ_WRITE_EN    ((uint8_t)(0x0AU)) // Enable CONF OTP read and write

/****************************** FUNC RST MASK *************************************/
#define FUNC_RST_RESET_CMD        ((uint8_t)(0x55U)) // Command to reset all configurations

/*======================================================================================================================
 * PUBLIC CONFIGURATION STRUCTURE
 *====================================================================================================================*/
typedef struct {
  uint8_t sconf1;
  uint8_t sconf2;

  uint8_t ovr;
  uint8_t ov_owt;
  uint8_t ovt_ov;

  uint8_t uvr;
  uint8_t uv;
  uint8_t lv_uvt;

  uint8_t balv;
  uint8_t bal_ocd;
  uint8_t bal_conf;

  uint8_t sc;
  uint8_t occ;

  uint8_t otc;
  uint8_t otcr;
  uint8_t otd;
  uint8_t otdr;
  uint8_t utc;
  uint8_t utcr;
  uint8_t utd;
  uint8_t utdr;

  uint8_t ocd1v;
  uint8_t ocd2v;
  uint8_t occv;

  uint8_t sleep_conf;
} moli3206_config_t;

/*======================================================================================================================
 * PUBLIC API PROTOTYPES
 *====================================================================================================================*/

/**
 * @brief  Initialize the MOLI3206 device and check communication.
 * @param  dev Pointer to the device handle
 * @return moli3206_status_t status code
 */
moli3206_status_t moli3206_Init(moli3206_t *dev);

/**
 * @brief  Apply standard threshold and configuration settings to the MOLI3206 registers.
 * @param  dev Pointer to the device handle
 * @param  config Pointer to configuration values structure
 * @return moli3206_status_t status code
 */
moli3206_status_t moli3206_ApplyConfig(moli3206_t *dev, const moli3206_config_t *config);

/**
 * @brief  Verify that the device's configuration registers match the expected values.
 * @param  dev Pointer to the device handle
 * @param  config Pointer to expected configuration values
 * @return moli3206_status_t status code
 */
moli3206_status_t moli3206_VerifyConfig(moli3206_t *dev, const moli3206_config_t *config);

/**
 * @brief Low-level raw register access.
 *
 * These APIs may be called before moli3206_Init().
 * The caller must ensure that dev_addr and crc_enabled
 * match the actual device configuration.
 */
moli3206_status_t moli3206_ReadReg(moli3206_t *dev, uint8_t reg, uint8_t *val);

/**
 * @brief Low-level raw register access.
 *
 * These APIs may be called before moli3206_Init().
 * The caller must ensure that dev_addr and crc_enabled
 * match the actual device configuration.
 */
moli3206_status_t moli3206_WriteReg(moli3206_t *dev, uint8_t reg, uint8_t val);

/**
 * @brief Low-level raw register access.
 *
 * These APIs may be called before moli3206_Init().
 * The caller must ensure that dev_addr and crc_enabled
 * match the actual device configuration.
 */
moli3206_status_t moli3206_ReadRegs(moli3206_t *dev, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief Low-level raw register access.
 *
 * These APIs may be called before moli3206_Init().
 * The caller must ensure that dev_addr and crc_enabled
 * match the actual device configuration.
 */
moli3206_status_t moli3206_WriteRegs(moli3206_t *dev, uint8_t reg, const uint8_t *buf, uint16_t len);

/**
 * @brief  Read a 16-bit register value (LSB first, MSB second format).
 */
moli3206_status_t moli3206_ReadReg16(moli3206_t *dev, uint8_t reg, uint16_t *val);

/**
 * @brief  Read the raw cell voltage for a specific cell (1 to 6).
 * @param  cell_idx Cell index (1 to 6)
 * @param  raw Pointer to store the raw 16-bit ADC value
 */
moli3206_status_t moli3206_ReadCellVoltageRaw(moli3206_t *dev, uint8_t cell_idx, uint16_t *raw);

/**
 * @brief  Convert a raw signed 15-bit voltage reading to millivolts.
 * @param  raw Raw 16-bit register reading
 * @return Decoded voltage in millivolts (mV)
 */
int32_t moli3206_CellRawToMillivolts(uint16_t raw);

/**
 * @brief  Read external NTC TS1 raw ADC value.
 */
moli3206_status_t moli3206_ReadTS1(moli3206_t *dev, uint16_t *ts1_raw);

/**
 * @brief  Read external NTC TS2 raw ADC value.
 */
moli3206_status_t moli3206_ReadTS2(moli3206_t *dev, uint16_t *ts2_raw);

/**
 * @brief  Read internal temperature raw ADC value.
 */
moli3206_status_t moli3206_ReadInternalTemp(moli3206_t *dev, uint16_t *temp_raw);

/**
 * @brief  Read charge current raw ADC value.
 */
moli3206_status_t moli3206_ReadChargeCurrent(moli3206_t *dev, uint16_t *curr_chg_raw);

/**
 * @brief  Read discharge current raw ADC value.
 */
moli3206_status_t moli3206_ReadDischargeCurrent(moli3206_t *dev, uint16_t *curr_dsg_raw);

/**
 * @brief  Get the current status registers.
 */
moli3206_status_t moli3206_GetStatus(moli3206_t *dev, uint8_t *status1, uint8_t *status2, uint8_t *status3);

/**
 * @brief  Get the current interrupt/protection flags.
 */
moli3206_status_t moli3206_GetFlags(moli3206_t *dev, uint8_t *flag1, uint8_t *flag2);

/**
 * @brief  Clear specific interrupt/protection flags (Write-1-to-Clear).
 */
moli3206_status_t moli3206_ClearFlags(moli3206_t *dev, uint8_t flag1_mask, uint8_t flag2_mask);

/**
 * @brief  Configure charging and discharging MOSFET control states.
 */
moli3206_status_t moli3206_SetFETState(moli3206_t *dev, bool chg_fet_en, bool dsg_fet_en);

/**
 * @brief  Get the current MOSFET states from status.
 */
moli3206_status_t moli3206_GetFETState(moli3206_t *dev, bool *chg_fet_en, bool *dsg_fet_en);

/**
 * @brief  Enable or disable battery cell balancing function.
 */
moli3206_status_t moli3206_SetBalanceState(moli3206_t *dev, bool balance_en);

/**
 * @brief Reset all configuration registers.
 *
 * After success, the driver becomes uninitialized.
 * The application must synchronize dev_addr/crc_enabled
 * with the post-reset device state and call moli3206_Init().
 */
moli3206_status_t moli3206_Reset(moli3206_t *dev);

/**
 * @brief  Read all cell voltages, currents, temperatures, statuses, and FET states in one operation.
 * @param  dev Pointer to the device handle
 * @param  data Pointer to data structure to store telemetry
 * @return moli3206_status_t status code
 */
moli3206_status_t moli3206_ReadAllData(moli3206_t *dev, moli3206_data_t *data);

/**
 * @brief Burn the current RAM configuration block into CONF OTP.
 *
 * The function copies RAM addresses 0x40..0x5A to OTP addresses
 * 0x00..0x1A, verifies the copied bytes, and writes CONF_IDENTIFY
 * value 0x4D last so the OTP becomes active after the next power cycle.
 *
 * @warning OTP is one-time programmable and cannot be changed after burning.
 * @warning The application must place the device in Normal mode and provide
 *          the required 7.2 V programming supply on V_otp before calling.
 * @warning Call moli3206_ApplyConfig() and moli3206_VerifyConfig() first.
 *
 * @param dev Pointer to the device handle.
 * @return MOLI3206_OK on success, otherwise an error code.
 */
moli3206_status_t moli3206_BurnOTP(moli3206_t *dev);

/**
 * @brief  Enable OTP Read Access by setting the correct OTP access bits.
 */
moli3206_status_t moli3206_EnableOTPReadAccess(moli3206_t *dev);

/**
 * @brief  Enable OTP Read/Write Access (useful for development config access).
 */
moli3206_status_t moli3206_EnableOTPReadWriteAccess(moli3206_t *dev);

/**
 * @brief  Disable OTP Access to protect configuration.
 */
moli3206_status_t moli3206_DisableOTPAccess(moli3206_t *dev);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MOLI3206_H
/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
