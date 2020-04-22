/* memory leak checking */
#ifdef _MSC_VER
#include "vld.h"
#endif // _MSC_VER

/* std headers */
#include <stdio.h>  // for: NULL
#include <string.h> // for: strlen
/* user headers */
#include "crc_utils.h"

const uint8_t str[] = "123456789";

const crc_model_param_s crc16 = {
    "CRC16", 16, 1, 1, 0x8005, 0x0000, 0x0000, 0x00
};
const crc_model_param_s crc16_modbus = {
    "CRC16(Modbus)", 16, 1, 1, 0x8005, 0xFFFF, 0x0000, 0x00
};
// todo: fixme
const crc_model_param_s crc16_sick = {
    "CRC16(Sick)", 16, 0, 0, 0x8005, 0x0000, 0x0000, 0x00
};
const crc_model_param_s crc16_ccitt_xmodem = {
    "CRC16-CCITT(XModem)", 16, 0, 0, 0x1021, 0x0000, 0x0000, 0x00
};
const crc_model_param_s crc16_ccitt_ffff = {
    "CRC16-CCITT(0xFFFF)", 16, 0, 0, 0x1021, 0xFFFF, 0x0000, 0x00
};
const crc_model_param_s crc16_ccitt_1d0f = {
    "CRC16-CCITT(0x1D0F)", 16, 0, 0, 0x1021, 0x1D0F, 0x0000, 0x00
};
// todo: fixme
const crc_model_param_s crc16_kermit = {
    "CRC16-Kermit", 16, 1, 1, 0x8408, 0x0000, 0x0000, 0x00
};
// todo: fixme
const crc_model_param_s crc16_dnp = {
    "CRC16-DNP", 16, 1, 1, 0xA6BC, 0x0000, 0x0000, 0x00
    //"CRC16-DNP", 16, 0, 0, 0x3D65, 0x0000, 0x0000, 0x00
};
const crc_model_param_s crc32 = {
    "CRC32", 32, 1, 1, 0x4c11db7, 0xffffffff, 0xffffffff, 0x00
};

void smoke_test(const crc_model_param_s param) {
    crc_model_t m = crc_util_model_init(param, NULL);
    crc_util_model_show(m);
    crc_util_model_debug(m, str, strlen((const char *)str));
    crc_util_model_fini(m);
}

int main(int argc, char *argv[]) {
    smoke_test(crc16);
    smoke_test(crc16_modbus);
    //smoke_test(crc16_sick);
    smoke_test(crc16_ccitt_xmodem);
    smoke_test(crc16_ccitt_ffff);
    smoke_test(crc16_ccitt_1d0f);
    //smoke_test(crc16_kermit);
    //smoke_test(crc16_dnp);
    smoke_test(crc32);
    return 0;
}