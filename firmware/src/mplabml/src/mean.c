
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

//      mean.c
//
//	Return the arithmetic mean of the elements in an array.
//
//      This is the integer optimized version of mean.c
//      It calls the inline routine get_axis_data(), which returns an int16.
//      The input data values are therefore in -32k..32k (16 bits).
//      The routine get_axis_data() is in ../include/rb.h.
//
//	Note that there is a small danger of integer overflow if more than
//	64k data elements are input.
//
//	Note that we are now supporting both FLOAT and int32 return values.
//	Note that we lose a small amount of  precision when the int32 divide
//	takes place, at the end of the function.

FLOAT mean(ringb *pringb, int32_t base_index, int32_t len)
{
    if ((!pringb) || (!len))
    {
        return 0;
    }

    int32_t irow;
    int64_t sum = 0;

    for (irow = base_index; irow < (base_index + len); irow++)
    {
        sum += MOD_READ_RINGBUF(pringb, irow);
    }

    return ((FLOAT)sum / len);
}

int32_t i_mean(ringb *pringb, int32_t base_index, int32_t len)
{
    int32_t irow;
    int32_t sum = 0;

    for (irow = base_index; irow < (base_index + len); irow++)
    {
        sum += MOD_READ_RINGBUF(pringb, irow); // add int16 to int32
    }

    return (int32_t)(sum / len);
}

int32_t i_mean_buff(rb_data_t *buff, int32_t len)
{
    int32_t irow;
    int32_t sum = 0;

    for (irow = 0; irow < len; irow++)
    {
        sum += buff[irow]; // add int16 to int32
    }

    return (int32_t)(sum / len);
}
