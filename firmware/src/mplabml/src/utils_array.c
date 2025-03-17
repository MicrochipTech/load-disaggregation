
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

/* Support function for data in a contigous array, these will typically be used to act on our tempData buffer */

#include <stdbool.h>
#include "kbutils.h"
#include "fftr_utils.h"

////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: this is here to make it easier to debug off-device (during development) using the
// unit test (gtest) framework and printf()
#ifdef UTEST
// comment out the next line before committing:
// #define DBG_UTEST
#endif
#ifdef DBG_UTEST
#include <stdio.h>
#endif
////////////////////////////////////////////////////////////////////////////////////////////

// buffer large enough to hold 1/2 the hanning coefficients for window sizes upto 512 (FFTR limit):
// note: only need the 1st half of them because the other half is mirrored
static FLOAT window_coeffs[NUM_FFTR / 2];
static int16_t window_size = 0;
static uint8_t window_uses_float = false;

void remove_mean_data_float(FLOAT *pdata, int32_t len)
{
    int32_t idx;
    FLOAT sum = 0.0;
    FLOAT average;

    // calc average:
    for (idx = 0; idx < len; idx++)
    {
        sum += pdata[idx];
    }
    average = sum / len;

    // remove mean:
    for (idx = 0; idx < len; idx++)
    {
        pdata[idx] -= average;
    }
}

void autoscale_data_float(FLOAT *pdata, int32_t len)
{
    int32_t idx;
    FLOAT min = KB_FLT_MAX;
    FLOAT max = KB_FLT_MIN;
    FLOAT sf;

    // find min,max:
    for (idx = 0; idx < len; idx++)
    {
        if (pdata[idx] < min)
        {
            min = pdata[idx];
        }
        if (pdata[idx] > max)
        {
            max = pdata[idx];
        }
    }

    // get abs of min,max:
    if (max < 0.0f)
        max *= -1.0f;
    if (min < 0.0f)
        min *= -1.0f;

    // calc scale factor based on larger abs value:
    if (max >= min)
    {
        sf = 32767.0f / max;
    }
    else
    {
        sf = 32767.0f / min;
    }

    // scale the data:
    for (idx = 0; idx < len; idx++)
    {
        pdata[idx] *= sf;
    }
}

static void gen_coeffs_float(int32_t len)
{
    FLOAT fTmp;
    FLOAT *pWC = window_coeffs;
    int32_t idx;

    // generate the 1st half; the 2nd half mirrors the first
    for (idx = 0; idx < len / 2; idx++)
    {
        fTmp = 0.5 - (0.5 * cos((2 * PI * idx) / len)); // range is 0.0 to 1.0
        pWC[idx] = fTmp;
    }
#ifdef DBG_UTEST
    printf("\nHanning Coeffs:\n");
    for (idx = 0; idx < len / 2; idx += 8)
    {
        printf("%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\n",
               pWC[idx], pWC[idx + 1], pWC[idx + 2], pWC[idx + 3],
               pWC[idx + 4], pWC[idx + 5], pWC[idx + 6], pWC[idx + 7]);
    }
#endif
}

void apply_hanning_float(FLOAT *pdata, int32_t len)
{
    FLOAT fTmp;
    int32_t idx, idx_m;

    // see if we need to regenerate the coefficients:
    if ((window_size != len) || (!window_uses_float))
    {
        gen_coeffs_float(len);
        window_size = len;
        window_uses_float = true;
    }

    // now, apply them to the data:
    for (idx = 0; idx < len / 2; idx++)
    {
        fTmp = window_coeffs[idx];

        pdata[idx] = (fTmp * pdata[idx]);
        idx_m = (len - idx - 1);
        pdata[idx_m] = (fTmp * pdata[idx_m]);
    }
}

void remove_mean_data_int(int16_t *pdata, int32_t len)
{
    int32_t sum = 0;
    int16_t average;
    int32_t idx;

    // calc average:
    for (idx = 0; idx < len; idx++)
    {
        sum += pdata[idx];
    }
    average = sum / len;

    // remove mean:
    for (idx = 0; idx < len; idx++)
    {
        pdata[idx] -= average;
    }
}

void autoscale_data_int(int16_t *pdata, int32_t len)
{
    int32_t idx;
    int32_t min = KB_SHORT_INT_MAX;
    int32_t max = KB_SHORT_INT_MIN;
    FLOAT sf = 0;
    FLOAT tmp = 0;

    // find min,max:
    for (idx = 0; idx < len; idx++)
    {
        if (pdata[idx] < min)
        {
            min = pdata[idx];
        }
        if (pdata[idx] > max)
        {
            max = pdata[idx];
        }
    }

    // get abs of min,max:
    if (max < 0)
        max *= -1;
    if (min < 0)
        min *= -1;

    // calc scale factor based on larger abs value:
    if (max >= min)
    {
        if (max > 0)
        {
            sf = 32767 / (float)max;
        }
    }
    else
    {
        if (min > 0)
        {
            sf = 32767 / (float)min;
        }
    }

    // leave data alone if sf < 2:
    if (sf > 1)
    {
        for (idx = 0; idx < len; idx++)
        {
            tmp = pdata[idx] * sf;
            pdata[idx] = (int16_t)tmp;
        }
    }
}

static void gen_coeffs_int(int32_t len)
{
    FLOAT fTmp;
    int32_t idx;
    int16_t *pWC = (int16_t *)window_coeffs;

    // generate the 1st half; the 2nd half mirrors the first
    for (idx = 0; idx < len / 2; idx++)
    {
        fTmp = 0.5f - (0.5f * cos((2.0f * PI * idx) / len)); // range is 0.0 to 1.0
        fTmp *= 16384.0f;
        pWC[idx] = (int16_t)(fTmp);
    }
#ifdef DBG_UTEST
    printf("\nHanning Coeffs:\n");
    for (idx = 0; idx < len / 2; idx += 8)
    {
        printf("%5d, %5d, %5d, %5d, %5d, %5d, %5d, %5d\n",
               pWC[idx], pWC[idx + 1], pWC[idx + 2], pWC[idx + 3],
               pWC[idx + 4], pWC[idx + 5], pWC[idx + 6], pWC[idx + 7]);
    }
#endif
}

void apply_hanning_int(int16_t *pdata, int32_t len)
{
    int32_t iTmp;
    int32_t idx, idx_m;
    int16_t *pWC = (int16_t *)window_coeffs;

    // see if we need to regenerate the coefficients:
    if ((window_size != len) || window_uses_float)
    {
        gen_coeffs_int(len);
        window_size = len;
        window_uses_float = false;
    }

    // now, apply them to the data and scale down to fit in 16 bits:
    for (idx = 0; idx < len / 2; idx++)
    {
        iTmp = (int32_t)pWC[idx];

        pdata[idx] = ((iTmp * (int32_t)pdata[idx]) + 1) >> 14;
        idx_m = (len - idx - 1);
        pdata[idx_m] = ((iTmp * (int32_t)pdata[idx_m]) + 1) >> 14;
    }
}
