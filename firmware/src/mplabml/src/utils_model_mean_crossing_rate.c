
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

#define NUM_PARAMS 1
#define OFFSET_PARAM_IDX 0
int32_t utils_model_mean_crossing_rate(kb_model_t *kb_model, int16_data_t *cols_to_use, FLOAT *pFV, int32_t offset)
{

#if SML_DEBUG
    if (!kb_model || !cols_to_use || cols_to_use->size != 2 || kb_model->sg_length <= 0 || !pFV)
    {
        return 0;
    }
#endif

    int32_t sum = 0;
    int32_t base_index = kb_model->sg_index;
    ringb *rb;

    rb = kb_model->pdata_buffer->data + cols_to_use->data[0];

    for (int32_t i = 0; i < kb_model->sg_length; i++)
    {
        sum += MOD_READ_RINGBUF(rb, base_index++); // mean = sum << num_bits(mask);
    }

    sum = offset + sum / kb_model->sg_length;

    rb = kb_model->pdata_buffer->data + cols_to_use->data[1];

    copy_segment_to_buff(rb, sortedData, kb_model->sg_index, kb_model->sg_length);

    float num_crossings = (float)calculate_zc_with_threshold_xor(sortedData, kb_model->sg_length, sum / kb_model->sg_length);

    *pFV++ = num_crossings / kb_model->sg_length;

    return 1;
}
