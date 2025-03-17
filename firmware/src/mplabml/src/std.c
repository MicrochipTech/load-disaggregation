
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

//      std.c
//
//      Return square root of the arithmetic mean of the square of the
//      elements in an array.  It is called the "root mean square",
//      or the Standard Deviation.
//
//      std = SQRT(mean(abs(x - x.mean())**2))
//
//      This is the integer optimized version of std.c
//      It calls the inline routine get_axis_data(), which returns an int16.
//      The input data values are therefore in -32k..32k (16 bits).
//      The routine get_axis_data() is in ../include/rb.h.
//
//      Note that there is a small danger of integer overflow if more than
//      64k data elements are input.
//
//      Note that we are now supporting both FLOAT and int32 return values.
//      Note that we lose a small amount of  precision when the int32 divide
//      takes place, at the end of the function.

int32_t i_mean(ringb *pringb, int32_t base_index, int32_t len);

FLOAT f_std(ringb *pringb, int32_t base_index, int32_t len)
{
    int32_t irow;
    FLOAT sum = 0.0;
    FLOAT tmp;
    FLOAT xmean;

    // Compute the mean of the input data
    xmean = mean(pringb, base_index, len);

    for (irow = 0; irow < len; irow++)
    {
        tmp = MOD_READ_RINGBUF(pringb, base_index++) - xmean;
        sum += tmp * tmp;
    }

    return SQRT(sum / len);
}

int32_t i_std(ringb *pringb, int32_t base_index, int32_t len)
//
//      Returns the standard deviation, as computed using integers.
//
{
    int32_t irow;
    int32_t tmp;
    int32_t xmean;
    int64_t sum = 0.0;

    // Compute the mean of the input data
    xmean = i_mean(pringb, base_index, len);

    for (irow = 0; irow < len; irow++)
    {
        tmp = MOD_READ_RINGBUF(pringb, base_index++) - xmean;
        sum += tmp * tmp; // don't need 64-bit multiply
    }

    return (int32_t)(SQRT((FLOAT)sum / (FLOAT)len));
}

int32_t i_std_buffer(rb_data_t *buffer, int32_t len)
//
//      Returns the standard deviation, as computed using integers.
//
{
    int32_t irow;
    int32_t tmp;
    int32_t xmean;
    int64_t sum = 0.0;

    // Compute the mean of the input data
    xmean = i_mean_buff(buffer, len);

    for (irow = 0; irow < len; irow++)
    {
        tmp = buffer[irow] - xmean;
        sum += tmp * tmp; // don't need 64-bit multiply
    }

    return (int32_t)(SQRT((FLOAT)sum / (FLOAT)len));
}

FLOAT kb_std(ringb *pringb, int32_t base_index, int32_t len)
//
//      Generic version, for compatibility
//
{
    return f_std(pringb, base_index, len);
}
