
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

#include "kbalgorithms.h"

/*!
 * \brief Simple IIR filter
 *
 * \param pringb pointer to the ring buffer to add data too
 * \param pSample Pointer to the data which will be transformed
 * \param cols_to_use columns to transform
 * \param num_cols number of columns ub in col_to_use
 * \param alpha attenuation coefficient
 */

int32_t streaming_high_pass_filter(ring_buffer_t *pringb, int16_t *pSample, int16_data_t *cols_to_use, float alpha)
{
    ringb *rb;
    int32_t col;
    if (rb_status(pringb) == false)
    {
        for (int32_t i = 0; i < cols_to_use->size; i++)
        {
            rb = pringb + i;
            rb->stat = pSample[cols_to_use->data[i]];
            rb_lock(rb);
        }
        return -1;
    }

    for (int32_t i = 0; i < cols_to_use->size; i++)
    {
        rb = pringb + i;
        col = cols_to_use->data[i];
        {
            pSample[col] = (int16_t)(pringb->stat + (int16_t)(alpha * (float)(pSample[col] - rb->stat)));
            rb->stat = pSample[col];
        }
    }

    // printf("moving avg %d\n" , get_col_data(rbuff_index, index));

    return 1;
}
