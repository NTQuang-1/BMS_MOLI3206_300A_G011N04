/***********************************************************************************************************************
 * @file   : main.c
 * @author : qn265366@gmail.com
 * @date   : 20/08/2026
 * @version: 1.0.0
 * @brief  : The program 
 **********************************************************************************************************************/

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "debug.h"
#include "main_board.h"

/*======================================================================================================================
 * PRIVATE MACROS
 *====================================================================================================================*/

/*======================================================================================================================
 * PRIVATE TYPES & STRUCTURES
 *====================================================================================================================*/

/*======================================================================================================================
 * MODULE STATE (STATIC GLOBALS)
 *====================================================================================================================*/
uint8_t num_of_cell = 3;

/*======================================================================================================================
 * PRIVATE CONSTANTS
 *====================================================================================================================*/

/*======================================================================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *====================================================================================================================*/
static void ApplyApplicationOverrides(moli3206_config_t *config);

/**
 * @brief main
 *
 * @return none
 */
int main(void)
{
  moli3206_status_t status = MOLI3206_OK;
  moli3206_t *bms_dev      = NULL;
  moli3206_config_t config = {0};
  moli3206_data_t bms_data = {0};

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  SystemCoreClockUpdate();
  Delay_Init();

  main_board_MOLI3206_InitHardware();
  status = main_board_MOLI3206_Bind();
  bms_dev = moli3206_port_GetHandle();
  status = moli3206_port_Wakeup();
  status = moli3206_Init(bms_dev);

  main_board_get_series_battery(&num_of_cell);
  ApplyApplicationOverrides(&config);
  status = moli3206_ApplyConfig(bms_dev, &config);
  status = moli3206_VerifyConfig(bms_dev, &config);
  status = moli3206_WriteReg(bms_dev, MO_DIS2, (uint8_t)(DIS2_DIS_UTD | DIS2_DIS_UTC));

  status = moli3206_SetFETState(bms_dev, true, true);
  
  while(1)
  {
  }
}

/*======================================================================================================================
 * PRIVATE IMPLEMENTATION
 *====================================================================================================================*/
static void ApplyApplicationOverrides(moli3206_config_t *config)
{
  if (config == NULL)
  {
    return;
  }

  /* 3S4S pack: keep odd/even balancing enabled and select CN[1:0] = 01. */
  if (num_of_cell = 3)
  {
    config->sconf1 = (uint8_t)(SCONF1_BALANCE_MODE | SCONF1_CN_3S);
  }
  else
  {
    config->sconf1 = (uint8_t)(SCONF1_BALANCE_MODE | SCONF1_CN_4S);
  }

  config->sconf2 = (uint8_t)(SCONF2_TS1_OFF | SCONF2_TS2_OFF | SCONF2_LUVR);

  config->occv  = 0x00; // Turn of occv

  config->ovr = 0xBDU;
  config->ov_owt = OV_OWT_DELAY_8S | 0x05U;
  config->ovt_ov = OVT_OV_1S | 0x30U;

  config->uvr = 0x7DU;
  config->uv = 0xC8U;
  config->lv_uvt = LV_UVT_DIS_1000MV & (~0x04U) | LV_UVT_1S;

  config->balv = 0xB3U;
  config->bal_ocd = BAL_OCD_BALS_CHARGE_OR_IDLE | BAL_OCD_BALD_DEVIATION_20MV |
                    BAL_OCD_OCD2T_4S            | BAL_OCD_OCD1T_6S;
  config->bal_conf = BAL_CONF_BALT_NCHGING_400MS;

  config->sc = SC_SCT_1_6MS  | 0x1FU;
  config->occ = OCC_DHS_400UV | OCC_CHS_400UV | OCC_OCCT_1S;

  config->ocd1v = 0xFFU;
  config->ocd2v = 0xFFU;


  /*
   * Temperature registers are intentionally preserved from the current
   * device configuration. Replace them here with validated production
   * values before using BURN_OTP in manufacturing.
   */



  config->sleep_conf = SLEEP_CONF_DEFAULT;
}

/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
