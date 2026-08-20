/***********************************************************************************************************************
 * @file   : moli3206.c
 * @author : qn265366@gmail.com
 * @date   : 09/07/2026
 * @version: 1.0.0
 * @brief  : This file implements the public MOLI3206 driver API.
 **********************************************************************************************************************/

/*======================================================================================================================
 * INCLUDES
 *====================================================================================================================*/
#include "moli3206.h"
#include <stddef.h>

/*======================================================================================================================
 * PRIVATE CONSTANTS
 *====================================================================================================================*/
static const uint8_t cell_regs[] = {
  MO_CELL1, MO_CELL2, MO_CELL3, MO_CELL4, MO_CELL5, MO_CELL6
};

/*======================================================================================================================
 * PRIVATE FUNCTION PROTOTYPES
 *====================================================================================================================*/
static uint8_t moli3206_CalculateChecksum(const uint8_t *data, uint16_t size);
static int32_t moli3206_DecodeSigned15(uint16_t raw);
static moli3206_status_t moli3206_CheckReady(const moli3206_t *dev);
static moli3206_status_t moli3206_MapPortStatus(moli3206_port_status_t port_status);
static moli3206_status_t moli3206_ValidateConfig(const moli3206_config_t *config);

/*======================================================================================================================
 * PUBLIC API IMPLEMENTATION
 *====================================================================================================================*/

moli3206_status_t moli3206_Init(moli3206_t *dev)
{
  uint8_t probe_val     = 0U;
  uint8_t fault_mask2   = 0U;
  uint8_t required_bits = 0;
  moli3206_status_t ret;

  if (
    (dev == NULL)            ||
    (dev->i2c_write == NULL) ||
    (dev->i2c_read == NULL)  ||
    (dev->delay_ms == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }

  if (dev->dev_addr > 0x7FU)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }

  dev->initialized = false;
  dev->multi_write_enabled = false;

  /*
   * Wait until the startup WarmUp phase has completed.
   */
  dev->delay_ms(MOLI3206_STARTUP_DELAY_MS);

  /*
   * Verify communication using a readable register.
   */
  ret = moli3206_ReadReg(dev, MO_ID, &probe_val);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /*
   * Read the current FAULT_MASK2 value.
   */
  ret = moli3206_ReadReg(dev, MO_FAULT_MASK2, &fault_mask2);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /*
   * Multi-byte write is required by the driver for full OTP
   * configuration programming.
   *
   * The driver no longer depends on application MODE_RUN.
   */
  required_bits = (uint8_t)(FAULT_MASK2_I2C_MULTI_W_EN | FAULT_MASK2_TESTMODE_DISABLE);

  fault_mask2   = (uint8_t)((fault_mask2 & MOLI3206_FAULT_MASK2_WRITABLE_MASK) | required_bits);

  ret = moli3206_WriteReg(dev, MO_FAULT_MASK2, fault_mask2);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /*
   * Verify the required bits.
   */
  ret = moli3206_ReadReg(dev, MO_FAULT_MASK2, &fault_mask2);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  if ((fault_mask2 & required_bits) != required_bits)
  {
    return MOLI3206_ERR_VERIFY_FAILED;
  }

  dev->multi_write_enabled = ((fault_mask2 & FAULT_MASK2_I2C_MULTI_W_EN) != 0U);
  dev->initialized = true;

  return MOLI3206_OK;
}

moli3206_status_t moli3206_ApplyConfig(moli3206_t *dev, const moli3206_config_t *config)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  ret = moli3206_ValidateConfig(config);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /* Note: This operation is not atomic. If an I2C error occurs, 
   * some earlier registers may already have been updated. 
   */
  ret = moli3206_WriteReg(dev, MO_SCONF1, config->sconf1);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_SCONF2, config->sconf2);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OVR, config->ovr);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OV_OWT, config->ov_owt);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OVT_OV, config->ovt_ov);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_UVR, config->uvr);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_UV, config->uv);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_LV_UVT, config->lv_uvt);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_BALV, config->balv);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_BAL_OCD, config->bal_ocd);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_BAL_CONF, config->bal_conf);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_SC, config->sc);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OCC, config->occ);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OTC, config->otc);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OTCR, config->otcr);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OTD, config->otd);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OTDR, config->otdr);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_UTC, config->utc);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_UTCR, config->utcr);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_UTD, config->utd);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_UTDR, config->utdr);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OCD1V, config->ocd1v);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OCD2V, config->ocd2v);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_OCCV, config->occv);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_WriteReg(dev, MO_SLEEP_CONF, config->sleep_conf);
  if (ret != MOLI3206_OK) return ret;

  return MOLI3206_OK;
}

moli3206_status_t moli3206_VerifyConfig(moli3206_t *dev, const moli3206_config_t *config)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  ret = moli3206_ValidateConfig(config);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  uint8_t val;
  
  #define VERIFY_REG(reg_, expected_, mask_)                            \
    do                                                                  \
    {                                                                   \
      ret = moli3206_ReadReg(dev, (reg_), &val);                        \
      if (ret != MOLI3206_OK)                                           \
      {                                                                 \
        return ret;                                                     \
      }                                                                 \
      if ((uint8_t)(val & (mask_)) != (uint8_t)((expected_) & (mask_))) \
      {                                                                 \
        return MOLI3206_ERR_VERIFY_FAILED;                              \
      }                                                                 \
    } while (0)

  VERIFY_REG(MO_SCONF1, config->sconf1, MOLI3206_SCONF1_WRITABLE_MASK);
  VERIFY_REG(MO_SCONF2, config->sconf2, MOLI3206_SCONF2_WRITABLE_MASK);
  VERIFY_REG(MO_OVR, config->ovr, 0xFFU);
  VERIFY_REG(MO_OV_OWT, config->ov_owt, MOLI3206_OV_OWT_WRITABLE_MASK);
  VERIFY_REG(MO_OVT_OV, config->ovt_ov, 0xFFU);
  VERIFY_REG(MO_UVR, config->uvr, 0xFFU);
  VERIFY_REG(MO_UV, config->uv, 0xFFU);
  VERIFY_REG(MO_LV_UVT, config->lv_uvt, MOLI3206_LV_UVT_WRITABLE_MASK);
  VERIFY_REG(MO_BALV, config->balv, 0xFFU);
  VERIFY_REG(MO_BAL_OCD, config->bal_ocd, 0xFFU);
  VERIFY_REG(MO_BAL_CONF, config->bal_conf, MOLI3206_BAL_CONF_WRITABLE_MASK);
  VERIFY_REG(MO_SC, config->sc, 0xFFU);
  VERIFY_REG(MO_OCC, config->occ, 0xFFU);
  VERIFY_REG(MO_OTC, config->otc, 0xFFU);
  VERIFY_REG(MO_OTCR, config->otcr, 0xFFU);
  VERIFY_REG(MO_OTD, config->otd, 0xFFU);
  VERIFY_REG(MO_OTDR, config->otdr, 0xFFU);
  VERIFY_REG(MO_UTC, config->utc, 0xFFU);
  VERIFY_REG(MO_UTCR, config->utcr, 0xFFU);
  VERIFY_REG(MO_UTD, config->utd, 0xFFU);
  VERIFY_REG(MO_UTDR, config->utdr, 0xFFU);
  VERIFY_REG(MO_OCD1V, config->ocd1v, 0xFFU);
  VERIFY_REG(MO_OCD2V, config->ocd2v, 0xFFU);
  VERIFY_REG(MO_OCCV, config->occv, 0xFFU);
  VERIFY_REG(MO_SLEEP_CONF, config->sleep_conf, 0xFFU);

  #undef VERIFY_REG
  return MOLI3206_OK;
}

moli3206_status_t moli3206_WriteRegs(moli3206_t *dev, uint8_t reg, const uint8_t *buf, uint16_t len)
{
  if ((dev == NULL) || (dev->i2c_write == NULL) || (buf == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  
  uint16_t max_len = 
    dev->crc_enabled ? MOLI3206_MAX_WRITE_LEN_WITH_CRC : MOLI3206_MAX_WRITE_LEN_NO_CRC;
  if ((len == 0U) || (len > max_len))
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if (((uint16_t)reg + len - 1U) > 0xFFU)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if ((len > 1U) && (!dev->multi_write_enabled))
  {
    return MOLI3206_ERR_PROTOCOL_STATE;
  }

  uint8_t packet[MOLI3206_PACKET_CAPACITY];
  packet[0] = MOLI3206_CMD_WRITE;
  packet[1] = reg;                         /* 16-bit register address LSB */
  packet[2] = MOLI3206_REG_ADDR_MSB;       /* 16-bit register address MSB */
  packet[3] = (uint8_t)len;

  for (uint16_t i = 0U; i < len; ++i)
  {
    packet[4U + i] = buf[i];
  }

  uint16_t packet_size;
  if (dev->crc_enabled)
  {
    packet[4U + len] = moli3206_CalculateChecksum(packet, (uint16_t)(4U + len));
    packet_size = (uint16_t)(5U + len);
  }
  else
  {
    packet_size = (uint16_t)(4U + len);
  }

  moli3206_port_status_t port_ret = dev->i2c_write(dev->dev_addr, packet, packet_size);
  return moli3206_MapPortStatus(port_ret);
}

moli3206_status_t moli3206_ReadRegs(moli3206_t *dev, uint8_t reg, uint8_t *buf, uint16_t len)
{
  if ((dev == NULL) || (dev->i2c_write == NULL) || (dev->i2c_read == NULL) || (buf == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  if ((len == 0U) || (len > MOLI3206_MAX_READ_LEN))
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if (((uint16_t)reg + len - 1U) > 0xFFU)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }

  uint8_t cmd[5];
  cmd[0] = MOLI3206_CMD_READ;
  cmd[1] = reg;                         /* 16-bit register address LSB */
  cmd[2] = MOLI3206_REG_ADDR_MSB;       /* 16-bit register address MSB */
  cmd[3] = (uint8_t)len;

  uint16_t cmd_size;
  if (dev->crc_enabled)
  {
    cmd[4] = moli3206_CalculateChecksum(cmd, 4U);
    cmd_size = 5U;
  }
  else
  {
    cmd_size = 4U;
  }

  /* Transaction 1: Send read request command packet */
  moli3206_port_status_t port_ret = dev->i2c_write(dev->dev_addr, cmd, cmd_size);
  if (port_ret != MOLI3206_PORT_OK)
  {
    return moli3206_MapPortStatus(port_ret);
  }

  /* Transaction 2: Read exactly len response data bytes */
  port_ret = dev->i2c_read(dev->dev_addr, buf, len);
  return moli3206_MapPortStatus(port_ret);
}

moli3206_status_t moli3206_ReadReg(moli3206_t *dev, uint8_t reg, uint8_t *val)
{
  return moli3206_ReadRegs(dev, reg, val, 1U);
}

moli3206_status_t moli3206_WriteReg(moli3206_t *dev, uint8_t reg, uint8_t val)
{
  return moli3206_WriteRegs(dev, reg, &val, 1U);
}

moli3206_status_t moli3206_ReadReg16(moli3206_t *dev, uint8_t reg, uint16_t *val)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  if (val == NULL)
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  
  uint8_t buf[2] = {0U};
  ret = moli3206_ReadRegs(dev, reg, buf, 2U);
  if (ret == MOLI3206_OK)
  {
    *val = (uint16_t)(((uint16_t)buf[1] << 8U) | (uint16_t)buf[0]);
  }
  return ret;
}

moli3206_status_t moli3206_ReadCellVoltageRaw(moli3206_t *dev, uint8_t cell_idx, uint16_t *raw)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  if (cell_idx < 1U || cell_idx > 6U)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  return moli3206_ReadReg16(dev, cell_regs[cell_idx - 1U], raw);
}

moli3206_status_t moli3206_ReadTS1(moli3206_t *dev, uint16_t *ts1_raw)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_ReadReg16(dev, MO_RD1, ts1_raw);
}

moli3206_status_t moli3206_ReadTS2(moli3206_t *dev, uint16_t *ts2_raw)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_ReadReg16(dev, MO_RD2, ts2_raw);
}

moli3206_status_t moli3206_ReadInternalTemp(moli3206_t *dev, uint16_t *temp_raw)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_ReadReg16(dev, MO_TEMPI, temp_raw);
}

moli3206_status_t moli3206_ReadChargeCurrent(moli3206_t *dev, uint16_t *curr_chg_raw)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_ReadReg16(dev, MO_CUR_CHG, curr_chg_raw);
}

moli3206_status_t moli3206_ReadDischargeCurrent(moli3206_t *dev, uint16_t *curr_dsg_raw)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_ReadReg16(dev, MO_CUR_DSG, curr_dsg_raw);
}

moli3206_status_t moli3206_GetStatus(
  moli3206_t *dev,
  uint8_t *status1,
  uint8_t *status2,
  uint8_t *status3)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  if ((status1 == NULL) || (status2 == NULL) || (status3 == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }

  /* Optimization: read 3 status registers in a single block transaction */
  uint8_t buf[3];
  ret = moli3206_ReadRegs(dev, MO_BSTATUS1, buf, 3U);
  if (ret == MOLI3206_OK)
  {
    *status1 = buf[0];
    *status2 = buf[1];
    *status3 = buf[2];
  }
  return ret;
}

moli3206_status_t moli3206_GetFlags(moli3206_t *dev, uint8_t *flag1, uint8_t *flag2)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  if ((flag1 == NULL) || (flag2 == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }

  /* Optimization: read 2 flag registers in a single block transaction */
  uint8_t buf[2];
  ret = moli3206_ReadRegs(dev, MO_BFLAG1, buf, 2U);
  if (ret == MOLI3206_OK)
  {
    *flag1 = buf[0];
    *flag2 = buf[1];
  }
  return ret;
}

moli3206_status_t moli3206_ClearFlags(moli3206_t *dev, uint8_t flag1_mask, uint8_t flag2_mask)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  if (flag1_mask != 0U)
  {
    ret = moli3206_WriteReg(dev, MO_BFLAG1, flag1_mask);
    if (ret != MOLI3206_OK)
    {
      return ret;
    }
  }

  if (flag2_mask != 0U)
  {
    ret = moli3206_WriteReg(dev, MO_BFLAG2, flag2_mask);
    if (ret != MOLI3206_OK)
    {
      return ret;
    }
  }

  return MOLI3206_OK;
}

moli3206_status_t moli3206_SetFETState(moli3206_t *dev, bool chg_fet_en, bool dsg_fet_en)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  uint8_t val = 0U;
  ret = moli3206_ReadReg(dev, MO_DIS3, &val);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /* 0: Enable (Active Low), 1: Disable */
  if (chg_fet_en)
  {
    val = (uint8_t)(val & (uint8_t)~DIS3_DIS_CHG_FET);
  }
  else
  {
    val |= DIS3_DIS_CHG_FET;
  }

  if (dsg_fet_en)
  {
    val = (uint8_t)(val & (uint8_t)~DIS3_DIS_DSG_FET);
  }
  else
  {
    val |= DIS3_DIS_DSG_FET;
  }

  return moli3206_WriteReg(dev, MO_DIS3, (uint8_t)(val & MOLI3206_DIS3_WRITABLE_MASK));
}

moli3206_status_t moli3206_GetFETState(moli3206_t *dev, bool *chg_fet_en, bool *dsg_fet_en)
{
  if ((dev == NULL) || (chg_fet_en == NULL) || (dsg_fet_en == NULL))
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  uint8_t status3 = 0U;
  ret = moli3206_ReadReg(dev, MO_BSTATUS3, &status3);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  *chg_fet_en = ((status3 & BSTATUS3_CHG_FET) != 0U);
  *dsg_fet_en = ((status3 & BSTATUS3_DSG_FET) != 0U);

  return MOLI3206_OK;
}

moli3206_status_t moli3206_SetBalanceState(moli3206_t *dev, bool balance_en)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  uint8_t val = 0U;
  ret = moli3206_ReadReg(dev, MO_DIS3, &val);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /* 0: Enable (Active Low), 1: Disable */
  if (balance_en)
  {
    val = (uint8_t)(val & (uint8_t)~DIS3_DIS_BAL);
  }
  else
  {
    val |= DIS3_DIS_BAL;
  }

  return moli3206_WriteReg(dev, MO_DIS3, (uint8_t)(val & MOLI3206_DIS3_WRITABLE_MASK));
}

moli3206_status_t moli3206_Reset(moli3206_t *dev)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  if (dev->delay_ms == NULL)
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }

  ret = moli3206_WriteReg(dev, MO_RST, FUNC_RST_RESET_CMD);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /* Clear driver internal states */
  dev->initialized = false;
  dev->multi_write_enabled = false;

  /* Wait for warm-up state startup completion */
  dev->delay_ms(MOLI3206_STARTUP_DELAY_MS);

  return MOLI3206_OK;
}

moli3206_status_t moli3206_ReadAllData(moli3206_t *dev, moli3206_data_t *data)
{
  if (data == NULL)
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /* Note: This operation is not atomic. On failure, data may
   * contain a mixture of new and previous values. 
   */

  /* 1. Read status and flags registers */
  ret = moli3206_GetStatus(dev, &data->status1, &data->status2, &data->status3);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_GetFlags(dev, &data->flag1, &data->flag2);
  if (ret != MOLI3206_OK) return ret;

  /* 2. Read cell voltages atomically in pairs */
  for (uint8_t i = 1U; i <= 6U; ++i)
  {
    ret = moli3206_ReadCellVoltageRaw(dev, i, &data->cell_voltage_raw[i - 1U]);
    if (ret != MOLI3206_OK) return ret;
  }

  /* 3. Read thermistors and internal temperature */
  ret = moli3206_ReadTS1(dev, &data->ts1_raw);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_ReadTS2(dev, &data->ts2_raw);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_ReadInternalTemp(dev, &data->internal_temp_raw);
  if (ret != MOLI3206_OK) return ret;

  /* 4. Read currents */
  ret = moli3206_ReadChargeCurrent(dev, &data->charge_current_raw);
  if (ret != MOLI3206_OK) return ret;

  ret = moli3206_ReadDischargeCurrent(dev, &data->discharge_current_raw);
  if (ret != MOLI3206_OK) return ret;

  /* 5. Extract FET states from status3 */
  data->chg_fet_enabled = ((data->status3 & BSTATUS3_CHG_FET) != 0U);
  data->dsg_fet_enabled = ((data->status3 & BSTATUS3_DSG_FET) != 0U);

  /* 6. Extract balancing state details */
  data->balancing_active = ((data->status3 & BSTATUS3_BAL) != 0U);

  uint8_t dis3_val = 0U;
  ret = moli3206_ReadReg(dev, MO_DIS3, &dis3_val);
  if (ret != MOLI3206_OK) return ret;

  data->balancing_allowed = ((dis3_val & DIS3_DIS_BAL) == 0U);

  return MOLI3206_OK;
}

moli3206_status_t moli3206_EnableOTPReadAccess(moli3206_t *dev)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_WriteReg(dev, MO_OTP_ACCESS, CONF_OTP_READ_ENABLE);
}

moli3206_status_t moli3206_EnableOTPReadWriteAccess(moli3206_t *dev)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_WriteReg(dev, MO_OTP_ACCESS, CONF_OTP_READ_WRITE_EN);
}

moli3206_status_t moli3206_DisableOTPAccess(moli3206_t *dev)
{
  moli3206_status_t ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }
  return moli3206_WriteReg(dev, MO_OTP_ACCESS, 0x00U);
}

moli3206_status_t moli3206_BurnOTP(
    moli3206_t *dev)
{
  uint8_t ram_config[MOLI3206_OTP_CONFIG_LENGTH];
  uint8_t otp_config[MOLI3206_OTP_CONFIG_LENGTH];

  uint8_t identify = 0U;
  uint8_t status2 = 0U;
  uint8_t status3 = 0U;

  moli3206_status_t ret;
  moli3206_status_t disable_ret;

  uint16_t i;

  ret = moli3206_CheckReady(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /*
   * A complete 27-byte OTP image requires multi-byte write.
   */
  if (!dev->multi_write_enabled)
  {
    return MOLI3206_ERR_PROTOCOL_STATE;
  }

  /*
   * OTP may only be programmed while the device is in Normal mode.
   */
  ret = moli3206_ReadReg(dev, MO_BSTATUS2, &status2);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  ret = moli3206_ReadReg(dev, MO_BSTATUS3, &status3);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  if (((status2 & BSTATUS2_SLEEP) != 0U) || ((status3 & BSTATUS3_PD) != 0U))
  {
    return MOLI3206_ERR_PROTOCOL_STATE;
  }

  /*
   * First check whether the OTP already contains a valid image.
   */
  ret = moli3206_EnableOTPReadAccess(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  ret = moli3206_ReadReg(dev, MOLI3206_OTP_CONF_IDENTIFY_ADDR, &identify);

  disable_ret = moli3206_DisableOTPAccess(dev);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  if (disable_ret != MOLI3206_OK)
  {
    return disable_ret;
  }

  if (identify == MOLI3206_OTP_CONF_IDENTIFY_VALID_VALUE)
  {
    return MOLI3206_ERR_OTP_ALREADY_PROGRAMMED;
  }

  /*
   * Read the complete RAM configuration image:
   *
   * 0x40 ID
   * ...
   * 0x58 I2C_CONF
   * 0x59 SLEEP_CONF
   * 0x5A BAL_CONF
   */
  ret = moli3206_ReadRegs(dev, MO_ID, ram_config, MOLI3206_OTP_CONFIG_LENGTH);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /*
   * Enable OTP read/write access.
   *
   * V_otp must already be supplied with 7.2 V by the application
   * hardware before this point.
   */
  ret = moli3206_EnableOTPReadWriteAccess(dev);
  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  /*
   * Program data first. Do not write the identify marker yet.
   */
  ret = moli3206_WriteRegs(
          dev,
          MOLI3206_OTP_CONFIG_START_ADDR,
          ram_config,
          MOLI3206_OTP_CONFIG_LENGTH);

  /*
   * Read back the OTP image.
   */
  if (ret == MOLI3206_OK)
  {
    ret = moli3206_ReadRegs(
            dev,
            MOLI3206_OTP_CONFIG_START_ADDR,
            otp_config,
            MOLI3206_OTP_CONFIG_LENGTH);
  }

  /*
   * Verify every byte before marking the OTP image valid.
   */
  if (ret == MOLI3206_OK)
  {
    for (i = 0U;i < MOLI3206_OTP_CONFIG_LENGTH;++i)
    {
      if (otp_config[i] != ram_config[i])
      {
        ret = MOLI3206_ERR_VERIFY_FAILED;
        break;
      }
    }
  }

  /*
   * Program CONF_IDENTIFY last.
   *
   * At the next power cycle, 0x4D tells the device to copy
   * OTP 0x00..0x1A into RAM 0x40..0x5A.
   */
  if (ret == MOLI3206_OK)
  {
    ret = moli3206_WriteReg(
            dev,
            MOLI3206_OTP_CONF_IDENTIFY_ADDR,
            MOLI3206_OTP_CONF_IDENTIFY_VALID_VALUE);
  }

  /*
   * Verify the identify marker.
   */
  if (ret == MOLI3206_OK)
  {
    ret = moli3206_ReadReg(dev, MOLI3206_OTP_CONF_IDENTIFY_ADDR, &identify);
  }

  if ((ret == MOLI3206_OK) && (identify != MOLI3206_OTP_CONF_IDENTIFY_VALID_VALUE))
  {
    ret = MOLI3206_ERR_VERIFY_FAILED;
  }

  /*
   * Always close OTP access.
   */
  disable_ret = moli3206_DisableOTPAccess(dev);

  if (ret != MOLI3206_OK)
  {
    return ret;
  }

  return disable_ret;
}

int32_t moli3206_CellRawToMillivolts(uint16_t raw)
{
  int32_t signed_code = moli3206_DecodeSigned15(raw);
  return (signed_code * 6) / 32;
}

/*======================================================================================================================
 * PRIVATE IMPLEMENTATION
 *====================================================================================================================*/

static uint8_t moli3206_CalculateChecksum(const uint8_t *data, uint16_t size)
{
  uint32_t sum = 0U;
  for (uint16_t i = 0U; i < size; ++i)
  {
    sum += data[i];
  }
  return (uint8_t)(sum & 0xFFU);
}

static int32_t moli3206_DecodeSigned15(uint16_t raw)
{
  int32_t magnitude = (int32_t)(raw & 0x7FFFU);
  if ((raw & 0x8000U) != 0U)
  {
    return -magnitude;
  }
  return magnitude;
}

static moli3206_status_t moli3206_CheckReady(const moli3206_t *dev)
{
  if (dev == NULL)
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  if (!dev->initialized)
  {
    return MOLI3206_ERR_NOT_READY;
  }
  return MOLI3206_OK;
}

static moli3206_status_t moli3206_MapPortStatus(moli3206_port_status_t port_status)
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

static moli3206_status_t moli3206_ValidateConfig(const moli3206_config_t *config)
{
  if (config == NULL)
  {
    return MOLI3206_ERR_MCU_NULL_PTR;
  }
  if (((config->sconf1) & (uint8_t)~(MOLI3206_SCONF1_WRITABLE_MASK)) != 0U)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if (((config->sconf2) & (uint8_t)~(MOLI3206_SCONF2_WRITABLE_MASK)) != 0U)
  { 
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if (((config->ov_owt) & (uint8_t)~(MOLI3206_OV_OWT_WRITABLE_MASK)) != 0U)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if (((config->lv_uvt) & (uint8_t)~(MOLI3206_LV_UVT_WRITABLE_MASK)) != 0U)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }
  if (((config->bal_conf) & (uint8_t)~(MOLI3206_BAL_CONF_WRITABLE_MASK)) != 0U)
  {
    return MOLI3206_ERR_MCU_INVALID_PARAM;
  }

  return MOLI3206_OK;
}

/*======================================================================================================================
 * END OF FILE
 *====================================================================================================================*/
