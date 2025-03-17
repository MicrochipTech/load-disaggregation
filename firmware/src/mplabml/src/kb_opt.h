#ifndef __KB_OPT_H__
#define __KB_OPT_H__

#include <stdint.h>
#include <limits.h>
// clang-format off
#ifdef OPT_ARM_M4_DSP
#if defined(__arm__)        
    #include "cmsis_armcc.h"
#elif defined(__GNUC__)
    #include "cmsis_gcc.h"
#else
#include "arm_math.h"
#endif
#endif
// clang-format on

#include "kbutils.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void lsup_distance(uint8_t *pSrcA, uint8_t *pSrcB, uint8_t *pDist, uint8_t *pMaxDist, uint32_t blockSize);
    void l1_distance(uint8_t *pSrcA, uint8_t *pSrcB, uint32_t *pDist, uint32_t blockSize);
    void compute_distance_matrix(unsigned char *x, unsigned char *y, int32_t x_size, int32_t y_size, uint8_t num_channels);
    int32_t compute_warping_distance(int32_t x_size, int32_t y_size);
    int32_t get_distance_matrix_value(int32_t i, int32_t j);
    int32_t get_globdist(int32_t i, int32_t j);

#ifdef __cplusplus
}
#endif
// clang-format on

#endif //__KB_OPT_H__
