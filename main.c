// @brief:  crc toolkit demo

/* for checking memory leak */
#ifdef _MSC_VER
#include "vld.h"
#endif // _MSC_VER

/* std headers */
#include <stdio.h>  // for: NULL
#include <string.h> // for: strlen
/* user headers */
#include "crc_utils.h"

const uint8_t str[] = "123456789";

const crc_model_param_s crc16 = { "CRC16(IBM/ARC/LHA)",
16, 1, 1, 0, 0x8005, 0x0000, 0x0000, 0xBB3D
};
const crc_model_param_s crc16_maxim = { "CRC16(Maxim)",
16, 1, 1, 0, 0x8005, 0x0000, 0xFFFF, 0x44C2
};
const crc_model_param_s crc16_usb = { "CRC16(Usb)",
16, 1, 1, 0, 0x8005, 0xFFFF, 0xFFFF, 0xB4C8
};
const crc_model_param_s crc16_modbus = { "CRC16(Modbus)",
16, 1, 1, 0, 0x8005, 0xFFFF, 0x0000, 0x4B37
};
//#define SUPPORT_CRC_SICK
#ifdef SUPPORT_CRC_SICK
// Note: In CRC Sick algorithm, every input byte is passed twice through the
// algorithm. So it's not available by the generic CRC algo framework. Visit
// https://github.com/lammertb/libcrc for more details.
const crc_model_param_s crc16_sick = { "CRC16(Sick)",
16, 0, 0, 1, 0x8005, 0x0000, 0x0000, 0x56A6
};
#endif /* SUPPORT_CRC_SICK */
const crc_model_param_s crc16_ccitt_xmodem = { "CRC16-CCITT(XModem/ZModem/Acorn)",
16, 0, 0, 0, 0x1021, 0x0000, 0x0000, 0x31C3
};
const crc_model_param_s crc16_ccitt_ffff = { "CRC16-CCITT(0xFFFF)",
16, 0, 0, 0, 0x1021, 0xFFFF, 0x0000, 0x29B1
};
const crc_model_param_s crc16_ccitt_1d0f = { "CRC16-CCITT(0x1D0F)",
16, 0, 0, 0, 0x1021, 0x1D0F, 0x0000, 0xE5CC
};
// Note: Currently, I'm not sure which one is the right check value for standard
// CRC16-Kermit and CRC16-DNP protocol. In "CRC RevEng", they're "0x2189" and
// "0xEA82", while "0x8921" and "0x82EA" in "On-line CRC calculation". Finally,
// I add another parameter in CRC model "swapout" to support both of them.
const crc_model_param_s crc16_kermit = { "CRC16-Kermit",
16, 1, 1, 0, 0x1021/* 0x8408 */, 0x0000, 0x0000, 0x2189
};
const crc_model_param_s crc16_kermit_swap = { "CRC16-Kermit(Swap)",
16, 1, 1, 1, 0x1021/* 0x8408 */, 0x0000, 0x0000, 0x8921
};
const crc_model_param_s crc16_dnp = { "CRC16-DNP",
16, 1, 1, 0, 0x3D65/* 0xA6BC */, 0x0000, 0xFFFF, 0xEA82
};
const crc_model_param_s crc16_dnp_swap = { "CRC16-DNP(Swap)",
16, 1, 1, 1, 0x3D65/* 0xA6BC */, 0x0000, 0xFFFF, 0x82EA
};
const crc_model_param_s crc16_x25 = { "CRC16-X25",
16, 1, 1, 0, 0x1021, 0x0000, 0xFFFF, 0xDE76
};
const crc_model_param_s crc32 = { "CRC32",
32, 1, 1, 0, 0x4c11db7, 0xffffffff, 0xffffffff, 0xCBF43926
};

void smoke_test(const crc_model_param_s param) {
    crc_model_t m = crc_util_model_init(param, NULL);
    crc_util_model_show(m);
    crc_util_model_debug(m, str, strlen((const char *)str));
    crc_util_model_fini(m);
}

int main(int argc, char *argv[]) {
    smoke_test(crc16);
    smoke_test(crc16_maxim);
    smoke_test(crc16_usb);
    smoke_test(crc16_modbus);
#ifdef SUPPORT_CRC_SICK
    smoke_test(crc16_sick);
#endif /* SUPPORT_CRC_SICK */
    smoke_test(crc16_ccitt_xmodem);
    smoke_test(crc16_ccitt_ffff);
    smoke_test(crc16_ccitt_1d0f);
    smoke_test(crc16_kermit);
    smoke_test(crc16_kermit_swap);
    smoke_test(crc16_dnp);
    smoke_test(crc16_dnp_swap);
    smoke_test(crc16_x25);
    smoke_test(crc32);
    return 0;
}