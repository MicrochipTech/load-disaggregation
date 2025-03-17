
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

FLOAT buffer_cumulative_sum_0(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen, int32_t abs_val)
{
    int32_t i;
    int32_t sum = 0;
    int32_t start_index = (base_index + offset) & pringb->mask;

    for (i = 0; i < datalen; i++)
    {
        if (abs_val)
            sum += abs(pringb->buff[start_index]);
        else
            sum += pringb->buff[start_index];
        start_index = (start_index + 1) & pringb->mask;
    }

    return (FLOAT)sum;
}

FLOAT buffer_cumulative_sum(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen)
{
    return buffer_cumulative_sum_0(pringb, base_index, offset, datalen, BUFFER_NO_ABS);
}

#if 0 // these 2 loops makes the code clumsy 
FLOAT buffer_cumulative_sum_0(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen, int32_t abs_val)
{
    int32_t i;
    int32_t sum = 0;
    int32_t start_index = (base_index + offset) & pringb->mask;;
    int32_t rb_length = pringb->mask + 1;
    int32_t final_index = start_index + datalen;

    if(final_index > pringb->mask)
    {
        for (i = start_index; i < rb_length; i++)
        {
            if(abs_val)
                sum += abs(pringb->buff[i]);
            else
                sum += pringb->buff[i];
        }
        start_index = 0;
        final_index = final_index - rb_length;
    }
    if(final_index > 0)
    {
        for (i = start_index; i < final_index; i++)
        {
            if(abs_val)
                sum += abs(pringb->buff[i]);
            else
                sum += pringb->buff[i];
        }
    }

    return (FLOAT)sum;
}

#endif