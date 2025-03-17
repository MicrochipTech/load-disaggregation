
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

/*
    Check if buffer has value greater than some threshold

    Return true if buffer has value greater than threshold else false
    @param pringb - pointer to ring buffer
    @param offset -offset to start at in ring buffer
    @param col - the axis of the data to get form the ring buffer
    @param datalen - the len of the data to use in the ring buffer
    @param threshold - value to check against

*/

bool buffer_pass_threshold(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen, int32_t threshold)
{
    int32_t irow;
    int32_t start_index = base_index + offset;
    for (irow = 0; irow < datalen; irow++)
    {
        if (abs(MOD_READ_RINGBUF(pringb, start_index++)) > threshold)
        {
            return true;
        }
    }

    return false;
}

bool buffer_pass_threshold_peak_ratio(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen, int32_t threshold_upper, int32_t threshold_lower, float_t ratio_limit)
{
    int32_t irow;
    int32_t start_index = base_index + offset;

    int32_t sum = 0;
    int32_t peak = 0;
    int32_t sample;
    float_t mean = 0;

    for (irow = 0; irow < datalen; irow++)
    {
        sample = abs(MOD_READ_RINGBUF(pringb, start_index++));
        sum += sample;
        if (sample > peak)
        {
            peak = sample;
        }
    }

    mean = (float_t)sum / datalen;

    if ((peak / (mean + 0.00001) > ratio_limit && peak > threshold_lower) || (peak > threshold_upper))
    {
        return true;
    }

    return false;
}
