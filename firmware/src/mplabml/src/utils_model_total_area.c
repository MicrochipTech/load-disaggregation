
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

int32_t utils_model_total_area(kb_model_t *kb_model, int16_data_t *cols_to_use, float_data_t *params, FLOAT *pFV, int32_t abs_val)
{
#define FG_AREA_TOTAL_AREA_NUM_PARAMS 1
#define FG_AREA_TOTAL_AREA_SAMPLE_RATE_PARAM_IDX 0

	if (!kb_model || kb_model->sg_length <= 0 || !cols_to_use || cols_to_use->size <= 0 || !params || params->size < FG_AREA_TOTAL_AREA_NUM_PARAMS || !pFV)
		return 0;

	int32_t fsum;
	int32_t naxes, i;
	ringb *rb;
	int32_t start_index;

	for (naxes = 0; naxes < cols_to_use->size; naxes++)
	{
		fsum = 0;
		rb = kb_model->pdata_buffer->data + cols_to_use->data[naxes];
		start_index = kb_model->sg_index & rb->mask;
		for (i = 0; i < kb_model->sg_length; i++)
		{
			if (abs_val)
				fsum += abs(rb->buff[start_index]);
			else
				fsum += rb->buff[start_index];
			start_index = (start_index + 1) & rb->mask;
		}
		*pFV++ = ((float)fsum) / params->data[FG_AREA_TOTAL_AREA_SAMPLE_RATE_PARAM_IDX];
	}
	return cols_to_use->size;
}
