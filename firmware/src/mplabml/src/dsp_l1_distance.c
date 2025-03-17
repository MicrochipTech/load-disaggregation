
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/

#include "kb_opt.h"

/**
 * @brief L1 Distance Calculation for uint8_t vectors
 *
 * @param[in]       *pSrcA points to the first input vector
 * @param[in]       *pSrcB points to the second input vector
 * @param[out]      *pDst points to output distance
 * @param[in]       blockSize number of samples in each vector
 * @return none.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.
 */

void l1_distance(
    uint8_t *pSrcA,
    uint8_t *pSrcB,
    uint32_t *pDist,
    uint32_t blockSize)
{
    uint32_t blkCnt; /* loop counter */
    uint32_t sum = 0;

#ifdef OPT_ARM_M4_DSP

    /* Run the below code for Cortex-M4 and Cortex-M3 */

    /*loop Unrolling */
    blkCnt = blockSize >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
     ** a second loop below computes the remaining 1 to 3 samples. */
    while (blkCnt > 0u)
    {
        /* C = A - B */
        /* Subtract and then store the results in the destination buffer 4 samples at a time. */
        sum = __USADA8(*__SIMD32(pSrcA)++, *__SIMD32(pSrcB)++, sum);

        /* Decrement the loop counter */
        blkCnt--;
    }

    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
     ** No loop unrolling is used. */
    blkCnt = blockSize % 0x4u;

    while (blkCnt > 0u)
    {
        /* C = A - B */
        /* Subtract and then store the result in the destination buffer. */
        sum += bitwise_absolute_value(*pSrcA++ - *pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }

#else

    /* Run the below code for Cortex-M0 */

    /* Initialize blkCnt with number of samples */
    blkCnt = blockSize;

    while (blkCnt > 0u)
    {
        /* C = A - B */
        /* Subtract and then store the result in the destination buffer. */
        sum += bitwise_absolute_value(*pSrcA++ - *pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }

#endif /* #ifndef ARM_MATH_CM0_FAMILY */

    *pDist = sum;
}
