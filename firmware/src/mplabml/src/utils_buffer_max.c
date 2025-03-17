
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
    min value in buffer

    Return the min of the elements in an array.
    @param pringb - pointer to ring buffer
    @param offset -offset to start at in ring buffer
    @param col - the axis of the data to get form the ring buffer
    @param datalen - the len of the data to use in the ring buffer

*/
int16_t buffer_max(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen)
{
    return buffer_max_0(pringb, base_index, offset, datalen, BUFFER_NO_ABS);
}

int16_t buffer_max_0(ringb *pringb, int32_t base_index, int32_t offset, int32_t datalen, int32_t abs_val)
{
    int32_t irow;
    int16_t fmax;
    int16_t val;
    int32_t start_index = (base_index + offset) & pringb->mask;

    fmax = pringb->buff[start_index];

    for (irow = 0; irow < datalen; irow++)
    {
        if (abs_val)
            val = abs(pringb->buff[start_index]);
        else
            val = pringb->buff[start_index];
        if (fmax < val)
            fmax = val;
        start_index = (start_index + 1) & pringb->mask;
    }

    return fmax;
}
