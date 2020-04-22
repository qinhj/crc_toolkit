// ------------------------------------------------------------------------
// @brief:      crc toolkit
// @file:       crc_utils.c
// @author:     qinhj@lsec.cc.ac.cn
// @date:       2020/04/22
// ------------------------------------------------------------------------

#include <stdio.h>  // for: fprintf
#include <stdlib.h> // for: calloc
#ifdef _DEBUG
#include <string.h> // for: strlen
#endif /* _DEBUG */
/* user headers */
#include "crc_utils.h"

#define log_error(...)      fprintf(stdout, "[E] " __VA_ARGS__)
#define log_warn(...)       fprintf(stdout, "[W] " __VA_ARGS__)
#define log_info(...)       fprintf(stdout, "[I] " __VA_ARGS__)
#define log_debug(...)      fprintf(stdout, "[D] " __VA_ARGS__)
#define log_verbose(...)    fprintf(stdout, "[V] " __VA_ARGS__)

typedef struct _crc_model_s {
    crc_model_param_s param;

    // Hard coded lookup table size as 2^8. 
    crc_t *table;
    crc_t init_direct;
    crc_t init_nodirect;
    crc_t crc_mask;
    crc_t high_bit_mask;

    void *data; // user data
} crc_model_s;

/* -------------------- private interface -------------------- */

static __inline int crc_util_param_check(crc_model_param_s param) {
    crc_t crcmask = ((((crc_t)1 << (param.width - 1)) - 1) << 1) | 1;
    // check param: name (ignore)
    // check param: width
    if (param.width < 1 || 64 < param.width) {
        log_error("[%s] invalid model param width: %u\n", __FUNCTION__, param.width);
    }
    // check param: refin/out (ignore)
    // check param: poly
    if (param.poly != (param.poly & crcmask)) {
        log_error("[%s] invalid model param poly: " CRC_F "\n", __FUNCTION__, param.poly);
    }
    // check param: init
    if (param.init != (param.init & crcmask)) {
        log_error("[%s] invalid model param init: " CRC_F "\n", __FUNCTION__, param.init);
    }
    // check param: xorout
    if (param.xorout != (param.xorout & crcmask)) {
        log_error("[%s] invalid model param xorout: " CRC_F "\n", __FUNCTION__, param.xorout);
    }
    // check param: check(ignore)
    return 0;
}

// Reflects the lower 'bitnum' bits of 'crc'
static __inline crc_t crc_util_reflect(crc_t crc, int bitnum) {
    crc_t i, j = 1, crcout = 0;
    for (i = (crc_t)1 << (bitnum - 1); i; i >>= 1) {
        if (crc & i) crcout |= j;
        j <<= 1;
    }
    return crcout;
}

#if defined(CRC_UTIL_NORMAL) || defined(_DEBUG)

// Normal lookup table algorithm with augmented zero bytes.
// Only usable with polynom orders of 8, 16, 24 or 32.
static crc_t crc_util_table(const crc_model_t m, const uint8_t *p, size_t len) {
    if (!m || !p) {
        log_error("[%s] invalid parameter: NULL\n", __FUNCTION__);
        return ~((crc_t)0);
    }
    uint32_t order = m->param.width;
    crc_t crc = m->init_nodirect;
    if (m->param.refin) crc = crc_util_reflect(crc, order);

    if (!m->param.refin) {
        while (len--) crc = ((crc << 8) | *p++) ^ m->table[(crc >> (order - 8)) & 0xff];
    }
    else {
        // reflect input
        while (len--) crc = ((crc >> 8) | (*p++ << (order - 8))) ^ m->table[crc & 0xff];
    }
    // handle final augmented zero bytes
    if (!m->param.refin) {
        while (++len < order / 8) crc = (crc << 8) ^ m->table[(crc >> (order - 8)) & 0xff];
    }
    else {
        while (++len < order / 8) crc = (crc >> 8) ^ m->table[crc & 0xff];
    }

    if (m->param.refout ^ m->param.refin) crc = crc_util_reflect(crc, order);
    crc ^= m->param.xorout;
    crc &= m->crc_mask;
    return crc;
}

// Bit by bit algorithm with augmented zero bytes.
// Don't use lookup table, suited for polynom orders between 1...32.
static crc_t crc_util_bitbybit(const crc_model_t m, const uint8_t *p, size_t len) {
    if (!m || !p) {
        log_error("[%s] invalid parameter: NULL\n", __FUNCTION__);
        return ~((crc_t)0);
    }
    size_t i, j;
    crc_t crc = m->init_nodirect, c, bit;
    for (i = 0; i < len; i++) {
        // qinhj: read one byte each time(since the generated table size is 2^8)
        c = (crc_t)*p++;
        if (m->param.refin) c = crc_util_reflect(c, 8 * sizeof(uint8_t));
        for (j = 0x80; j; j >>= 1) {
            bit = crc & m->high_bit_mask;
            crc <<= 1;
            if (c & j) crc |= 1;
            if (bit) crc ^= m->param.poly;
        }
    }
    for (i = 0; i < m->param.width; i++) {
        bit = crc & m->high_bit_mask;
        crc <<= 1;
        if (bit) crc ^= m->param.poly;
    }
    if (m->param.refout) crc = crc_util_reflect(crc, m->param.width);
    crc ^= m->param.xorout;
    crc &= m->crc_mask;
    return crc;
}

#endif  /* CRC_UTIL_NORMAL || _DEBUG */

#ifndef CRC_UTIL_NORMAL

// Fast lookup table algorithm without augmented zero bytes, e.g. used in pkzip.
// Only usable with polynom orders of 8, 16, 24 or 32.
static crc_t crc_util_table_fast(const crc_model_t m, const uint8_t *p, size_t len) {
    if (!m || !p) {
        log_error("[%s] invalid parameter: NULL\n", __FUNCTION__);
        return ~((crc_t)0);
    }
    uint32_t order = m->param.width;
    crc_t crc = m->init_direct;
    if (m->param.refin) crc = crc_util_reflect(crc, order);

    if (!m->param.refin) {
        while (len--) crc = (crc << 8) ^ m->table[((crc >> (order - 8)) & 0xff) ^ *p++];
    }
    else {
        while (len--) crc = (crc >> 8) ^ m->table[(crc & 0xff) ^ *p++];
    }

    if (m->param.refout ^ m->param.refin) crc = crc_util_reflect(crc, order);
    crc ^= m->param.xorout;
    crc &= m->crc_mask;
    return crc;
}

// Fast bit by bit algorithm without augmented zero bytes.
// Don't use lookup table, suited for polynom orders between 1...32.
static crc_t crc_util_bitbybit_fast(const crc_model_t m, const uint8_t *p, size_t len) {
    if (!m || !p) {
        log_error("[%s] invalid parameter: NULL\n", __FUNCTION__);
        return ~((crc_t)0);
    }
    size_t i, j;
    crc_t crc = m->init_direct, c, bit;
    for (i = 0; i < len; i++) {
        // qinhj: read one byte each time(since the generated table size is 2^8)
        c = (crc_t)*p++;
        if (m->param.refin) c = crc_util_reflect(c, 8 * sizeof(uint8_t));
        for (j = 0x80; j; j >>= 1) {
            bit = crc & m->high_bit_mask;
            crc <<= 1;
            if (c & j) bit ^= m->high_bit_mask;
            if (bit) crc ^= m->param.poly;
        }
    }
    if (m->param.refout) crc = crc_util_reflect(crc, m->param.width);
    crc ^= m->param.xorout;
    crc &= m->crc_mask;
    return crc;
}

#endif  /* CRC_UTIL_NORMAL */

// Make CRC lookup table used by table algorithms.
static int crc_util_table_generate(crc_model_t model) {
    if (NULL == model) {
        log_error("[%s] invalid input ptr: NULL\n", __FUNCTION__);
        return -1;
    }
    if (model->table) {
        log_warn("[%s] lookup table already inited\n", __FUNCTION__);
        return 0;
    }
    int i, j, count = 1 << 8;
    if (NULL == (model->table = calloc(count, sizeof(crc_t)))) {
        log_error("[%s] calloc for lookup table failed\n", __FUNCTION__);
        return -2;
    }
    crc_t crc, bit;
    for (i = 0; i < count; i++) {
        crc = (crc_t)i;
        if (model->param.refin) crc = crc_util_reflect(crc, 8);
        crc <<= model->param.width - 8;
        for (j = 0x80; j; j >>= 1) {
            bit = crc & model->high_bit_mask;
            crc <<= 1;
            if (bit) crc ^= model->param.poly;
        }
        if (model->param.refin) crc = crc_util_reflect(crc, model->param.width);
        crc &= model->crc_mask;
        model->table[i] = crc;
    }
    return 0;
}

/* -------------------- public  interface -------------------- */

int crc_util_model_show(const crc_model_t m) {
    if (m) {
        printf("\n");
        printf("CRC toolkit based on CRC tester v1.1 (Sven Reifegerste, 13/01/2003 (zorc/reflex))\n");
        printf("---------------------------------------------------------------------------------\n");
        printf("\n");
        printf("Parameters:\n");
        printf("\n");
        printf(" name       :  %s\n", m->param.name);
        printf(" polynom    :  0x" CRC_F "\n", m->param.poly);
        printf(" order      :  %d\n", m->param.width);
        printf(" crcinit    :  0x" CRC_F " direct, 0x" CRC_F " nondirect\n", m->init_direct, m->init_nodirect);
        printf(" crcxor     :  0x" CRC_F "\n", m->param.xorout);
        printf(" refin      :  %d\n", m->param.refin);
        printf(" refout     :  %d\n", m->param.refout);
    }
    return 0;
}

crc_t crc_util_model_run(const crc_model_t m, const uint8_t *p, size_t len) {
    if (!m || !p) {
        log_error("[%s] invalid parameter: NULL\n", __FUNCTION__);
        return ~((crc_t)0);
    }
#ifdef CRC_UTIL_NORMAL
    return (m->param.width & 7) ? crc_util_bitbybit(m, p, len) : crc_util_table(m, p, len);
#else //! CRC_UTIL_NORMAL
    return (m->param.width & 7) ? crc_util_bitbybit_fast(m, p, len) : crc_util_table_fast(m, p, len);
#endif /* CRC_UTIL_NORMAL */
}

crc_model_t crc_util_model_init(crc_model_param_s param, void *data) {
    if (crc_util_param_check(param)) {
        return NULL;
    }
    crc_model_t m = (crc_model_t)calloc(1, sizeof(crc_model_s));
    if (NULL == m) {
        log_error("[%s] calloc for crc model failed\n", __FUNCTION__);
        return m;
    }
    m->param = param;
    m->data = data;
    m->crc_mask = ((((crc_t)1 << (m->param.width - 1)) - 1) << 1) | 1;
    m->high_bit_mask = (crc_t)1 << (m->param.width - 1);
    // compute missing initial CRC value
    uint16_t i;
    crc_t crc = param.init, bit;
#ifdef CRC_UTIL_NORMAL
    for (i = 0; i < param.width; i++) {
        bit = crc & m->high_bit_mask;
        crc <<= 1;
        if (bit) crc ^= param.poly;
    }
    m->init_direct = crc;
    m->init_nodirect = param.init;
#else //! CRC_UTIL_NORMAL
    for (i = 0; i < param.width; i++) {
        bit = crc & 1;
        if (bit) crc ^= param.poly;
        crc >>= 1;
        if (bit) crc |= m->high_bit_mask;
    }
    m->init_direct = param.init;
    m->init_nodirect = crc;
#endif /* CRC_UTIL_NORMAL */
    // generate lookup table
    return (crc_util_table_generate(m), m);
}

int crc_util_model_fini(crc_model_t model) {
    if (model) free(model->table);
    return (free(model), 0);
}

#ifdef _DEBUG
void crc_util_model_debug(const crc_model_t m, const uint8_t *p, size_t len) {
    if (!m || !p || !len) {
        log_error("[%s] invalid parameter: NULL\n", __FUNCTION__);
        return;
    }
    log_verbose("[%s] input string          :   '%s' (%d bytes)\n", __FUNCTION__, p, (int)strlen((const char *)p));
    log_verbose("[%s] crc bit by bit        :   0x" CRC_F "\n", __FUNCTION__, crc_util_bitbybit(m, p, len));
    log_verbose("[%s] crc bit by bit fast   :   0x" CRC_F "\n", __FUNCTION__, crc_util_bitbybit_fast(m, p, len));
    if (!(m->param.width & 7)) {
        log_verbose("[%s] crc lookup table      :   0x" CRC_F "\n", __FUNCTION__, crc_util_table(m, p, len));
        log_verbose("[%s] crc lookup table fast :   0x" CRC_F "\n", __FUNCTION__, crc_util_table_fast(m, p, len));
    }
}
#endif  /* _DEBUG */
