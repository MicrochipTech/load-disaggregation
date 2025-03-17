
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
 * @brief LSUP Distance Calculation for uint8_t vectors
 *
 * @param[in]       *pSrcA points to the first input vector
 * @param[in]       *pSrcB points to the second input vector
 * @param[out]      *pDst points to an array that can hold the distances computed
 * @param[out]      *pDist points to the
 * @param[in]       blockSize number of samples in each vector
 * @return none.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.
 */

void lsup_distance(
    uint8_t *pSrcA,
    uint8_t *pSrcB,
    uint8_t *pDist,
    uint8_t *pMaxDist,
    uint32_t blockSize)
{
    uint32_t blkCnt; /* loop counter */

    /* Run the below code for Cortex-M4 and Cortex-M3 */

    /*loop Unrolling */
    blkCnt = blockSize;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
     ** a second loop below computes the remaining 1 to 3 samples. */
    while (blkCnt > 0u)
    {
        /* C = |A - B| */
        /* Absolute Difference. */
        *pDist++ = (uint8_t)bitwise_absolute_value(*pSrcA++ - *pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }

    array_max_uint8(pDist - blockSize, blockSize, pMaxDist);
}
