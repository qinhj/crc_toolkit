// ------------------------------------------------------------------------
// @brief:      crc toolkit
// @file:       crc_utils.h
// @author:     qinhj@lsec.cc.ac.cn
// @date:       2020/04/22
// ------------------------------------------------------------------------

#ifndef _CRC_UTILS_H_
#define _CRC_UTILS_H_

#include <stdint.h>     // need for: uint_xxx
#include <inttypes.h>   // need for: PRIX32/64

#if defined(CRC64) || defined(_WIN64)
#define CRC_F   "%"PRIX64
typedef uint64_t crc_t;
#else //! CRC64
#define CRC_F   "%"PRIX32
typedef uint32_t crc_t;
#endif /* CRC64 */

#ifdef __cplusplus
extern "C" {
#endif //!__cplusplus

    typedef struct _crc_model_s *crc_model_t;
    /* crc model parameter */
    typedef struct _crc_model_param_s *crc_model_param_t;
    typedef struct _crc_model_param_s {
        const char  *name;  // model name (e.g. CRC32)
        uint16_t    width;  // polynominal order
        uint8_t     refin;  // flag: reflect input byte
        uint8_t     refout; // flag: reflect crc register
        crc_t       poly;   // generator polynominal
        crc_t       init;   // init value for register
        crc_t       xorout; // post processing
        crc_t       check;  // currently ignore
    } crc_model_param_s;

    /* -------------------- public  interface -------------------- */

    int crc_util_model_show(const crc_model_t model);
    crc_t crc_util_model_run(const crc_model_t model, const uint8_t *p, size_t len);

    crc_model_t crc_util_model_init(crc_model_param_s param, void *data);
    int crc_util_model_fini(crc_model_t model);

#ifdef _DEBUG
    void crc_util_model_debug(const crc_model_t model, const uint8_t *p, size_t len);
#endif  /* _DEBUG */

#ifdef __cplusplus
}
#endif //!__cplusplus

#endif  /*_CRC_UTILS_H_*/