
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

#define NUM_PARAMS 0

int32_t utils_model_cross_difference(kb_model_t *kb_model, int16_data_t *cols_to_use, float_data_t *params, FLOAT *pFV, int32_t min_max)
{

#if SML_DEBUG
    if (!kb_model || !cols_to_use || kb_model->sg_length <= 0 || cols_to_use->size <= 0 || params->size != NUM_PARAMS || !pFV)
    {
        return 0;
    }
#endif

    int32_t i, index_col1, index_col2;
    int16_t val_col1, val_col2, max_col1, max_col2;
    ringb *rb1;
    ringb *rb2;
    int32_t base_index = kb_model->sg_index;

    rb1 = kb_model->pdata_buffer->data + cols_to_use->data[0];
    rb2 = kb_model->pdata_buffer->data + cols_to_use->data[1];

    index_col1 = base_index;
    index_col2 = base_index;

    max_col1 = KB_SHORT_INT_MIN;
    max_col2 = KB_SHORT_INT_MIN;
    int32_t final_index = base_index + kb_model->sg_length;

    for (i = base_index; i < final_index; i++)
    {
        val_col1 = MOD_READ_RINGBUF(rb1, i);
        val_col2 = MOD_READ_RINGBUF(rb2, i);

        if (val_col1 < max_col1)
        {
            max_col1 = val_col1;
            index_col1 = i;
        }

        if (val_col2 < max_col2)
        {
            max_col2 = val_col2;
            index_col2 = i;
        }
    }

    if (min_max)
    {
        if (max_col1 > max_col2)
        {
            *pFV = (FLOAT)(max_col1 - rb2->buff[index_col1 & rb2->mask]);
        }
        else
        {
            *pFV = (FLOAT)(max_col2 - rb1->buff[index_col2 & rb1->mask]);
        }
    }
    else
    {
        *pFV = (FLOAT)(max_col1 - max_col2);
    }

    return 1;
}
