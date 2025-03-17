
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

#include "kbutils.h"

void array_max_uint8(
    uint8_t *pSrc,
    uint32_t blockSize,
    uint8_t *pResult)
{
#ifndef ARM_MATH_CM0_FAMILY
    /* Run the below code for Cortex-M4 and Cortex-M3 */

    uint8_t maxVal1, maxVal2, out; /* Temporary variables to store the output value. */
    uint32_t blkCnt, count;        /* loop counter */

    /* Initialise the count value. */
    count = 0u;
    /* Load first input value that act as reference value for comparision */
    out = *pSrc++;

    /* Loop unrolling */
    blkCnt = (blockSize - 1u) >> 2u;

    /* Run the below code for Cortex-M4 and Cortex-M3 */
    while (blkCnt > 0u)
    {
        /* Initialize maxVal to the next consecutive values one by one */
        maxVal1 = *pSrc++;

        maxVal2 = *pSrc++;

        /* compare for the maximum value */
        if (out < maxVal1)
        {
            /* Update the maximum value and its index */
            out = maxVal1;
        }

        maxVal1 = *pSrc++;

        /* compare for the maximum value */
        if (out < maxVal2)
        {
            /* Update the maximum value and its index */
            out = maxVal2;
        }

        maxVal2 = *pSrc++;

        /* compare for the maximum value */
        if (out < maxVal1)
        {
            /* Update the maximum value and its index */
            out = maxVal1;
        }

        /* compare for the maximum value */
        if (out < maxVal2)
        {
            /* Update the maximum value and its index */
            out = maxVal2;
        }

        count += 4u;

        /* Decrement the loop counter */
        blkCnt--;
    }

    /* if (blockSize - 1u) is not multiple of 4 */
    blkCnt = (blockSize - 1u) % 4u;

#else

    /* Run the below code for Cortex-M0 */
    uint8_t maxVal1, out;      /* Temporary variables to store the output value. */
    uint32_t blkCnt, outIndex; /* loop counter */

    /* Initialise the index value to zero. */
    outIndex = 0u;
    /* Load first input value that act as reference value for comparision */
    out = *pSrc++;

    blkCnt = (blockSize - 1u);

#endif /* #ifndef ARM_MATH_CM0_FAMILY */

    while (blkCnt > 0u)
    {
        /* Initialize maxVal to the next consecutive values one by one */
        maxVal1 = *pSrc++;

        /* compare for the maximum value */
        if (out < maxVal1)
        {
            /* Update the maximum value and it's index */
            out = maxVal1;
        }
        /* Decrement the loop counter */
        blkCnt--;
    }

    /* Store the maximum value and its index into destination pointers */
    *pResult = out;
}