
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
 * \brief Moving average for incomming sensor data by taking the average over filter order
 *
 * \param ringbuffer pointer to the ring buffer to add data too
 * \param input_data Pointer to the data which will be transformed
 */
int16_t ma_filter_full(ringb *pringb, int32_t filter_order)
{
    int32_t val = 0;
    int32_t num_vals = 2 * filter_order + 1;

    for (int32_t i = 0; i < num_vals; i++)
    {
        val += rb_read_offset(pringb, i);
    }

    return val / num_vals;
}

int16_t time_lapse(ringb *pringb, int32_t filter_order)
{
    return rb_read_offset(pringb, filter_order);
}

int32_t streaming_moving_average(ring_buffer_t *pringb, int16_t *pSample, int16_data_t *cols_to_use, int32_t filter_order)
{
    saveSensorData(pringb, pSample, cols_to_use->size);
    ringb *rb;

    if (rb_items(pringb) < (2 * filter_order))
    {
        return -1;
    }

    for (int32_t i = 0; i < cols_to_use->size; i++)
    {
        rb = pringb + i;
        if (cols_to_use->data[i])
        {
            pSample[i] = (int16_t)ma_filter_full(rb, filter_order);
        }
        else
        {
            pSample[i] = (int16_t)time_lapse(rb, filter_order);
        }
        rb_step_head(rb, 1);
    }

    // printf("moving avg %d\n" , get_col_data(rbuff_index, index));

    return 1;
}
