/**
 * @file McuVersion.c
 * @author xiao.chen
 * @brief 
 * @version 0.1
 * @date 2019-08-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "McuVersion.h"
#pragma GCC optimize "O0"
typedef struct {
  uint8 sw_ver_len;
  uint8 hw_ver_len;
  uint16 reserved;
} McuVersion_ConfigType_st;
const McuVersion_ConfigType_st McuVersion_sConfig = {32};

const uint8 McuVersion_sSw[MCUVERSION_MAX_LEN] = {
    'M', 'C', 'P', '3', '.', '0', '-', 'M', 'C', 'U', '-', '8', '.', '0', '.', '0', '-',
    '2', '0', '2', '2', '0', '2', '2', '8', '\0'};
// const uint8 McuVersion_sHw[MCUVERSION_MAX_LEN] = VAR_HW_VER;
const uint8 McuVersion_sDate[MCUVERSION_MAX_LEN] = {
    '2', '0', '2', '0', '0', '8', '1', '8', '2', '1', '5', '7', '0', '0', '\0'};
const uint8 McuVersion_sExSw[MCUVERSION_MAX_LEN] = {
    '3', '.', '0', '.', '0', '-', 'r', 'c', '.', '0', '\0'};


// #pragma section ".ProgFlag"
// const uint32 ProgComplete[2] = {0xAA55AA55, 0x55AA55AA};
// #pragma section

void McuVersion_Read(uint8 *dst, uint16 *len) {
  uint8 i;
  for (i = 0; i < McuVersion_sConfig.sw_ver_len; i++) {
    dst[i] = McuVersion_sSw[i];
    if (dst[i] == '\0') {
      break;
    }
  }
  *len = i;
}

void McuVerion_ReadExtVer(uint8 *dst, uint16 *len) {
  uint8 i;
  for (i = 0; i < MCUVERSION_MAX_LEN; i++) {
    dst[i] = McuVersion_sExSw[i];
    if (dst[i] == '\0') {
      break;
    }
  }
  *len = i;
}

void McuVersion_Init() {
  uint8 ver[MCUVERSION_MAX_LEN];
  uint32 prog[2];
  uint16 len;
  McuVersion_Read(&ver[0], &len);
  // McuVersion_fReadProgFlag(&prog[0], &len);
}
