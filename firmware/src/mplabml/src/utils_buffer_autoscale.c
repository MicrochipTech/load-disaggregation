
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

void buffer_autoscale(ringb *pringb, int32_t base_index, int32_t length)
{

    int32_t idx;
    int16_t value = 0;
    int32_t min = KB_SHORT_INT_MAX;
    int32_t max = KB_SHORT_INT_MIN;
    float sf = 0;
    int32_t start_index = base_index;

    // find min,max:
    for (idx = 0; idx < length; idx++)
    {
        value = MOD_READ_RINGBUF(pringb, start_index++);
        if (value < min)
        {
            min = value;
        }
        if (value > max)
        {
            max = value;
        }
    }

    // get abs of min,max:
    if (max < 0)
    {
        max *= -1;
    }
    if (min < 0)
    {
        min *= -1;
    }

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

    if (sf <= 1.)
    {
        return;
    }

    for (idx = base_index; idx < base_index + length; idx++)
    {
        multiply_axis_data_float(pringb, idx, sf);
    }
}
