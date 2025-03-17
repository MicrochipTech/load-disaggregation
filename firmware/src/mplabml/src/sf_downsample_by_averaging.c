
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
 * \brief Downsample incomming sensor data by taking the average over filter length
 *
 * \param ringbuffer pointer to the ring buffer to add data too
 * \param input_data Pointer to the data which will be transformed
 */
int32_t streaming_downsample_by_averaging(ring_buffer_t *pringb, int16_t *pSample, int16_data_t *cols_to_use, int32_t filter_length)
{
    saveSensorData(pringb, pSample, cols_to_use->size);
    ringb *rb = pringb;

    if (rb_items(rb) < filter_length - 1)
    {
        return -1;
    }

    for (int32_t i = 0; i < cols_to_use->size; i++)
    {
        pSample[i] = (int16_t)i_mean(rb, 0, filter_length);
        rb_reset(rb);
        rb++;
    }
    return 1;
}
