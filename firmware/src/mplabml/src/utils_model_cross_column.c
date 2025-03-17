
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

int32_t utils_model_cross_column(kb_model_t *kb_model, int16_data_t *cols_to_use, float_data_t *params, FLOAT *pFV, int32_t abs_val)
{
#if SML_DEBUG
	if (!kb_model || !cols_to_use || kb_model->sg_length <= 0 || cols_to_use->size <= 0 || params->size != NUM_PARAMS || !pFV)
	{
		return 0;
	}
#endif

	// uint64_t sum = 0;
	int32_t row, icol;
	int16_t max_col = 1;
	ringb *rb;
	int32_t start_index;
	int16_t data, max_value;
	rb = kb_model->pdata_buffer->data + cols_to_use->data[0];
	start_index = kb_model->sg_index & rb->mask;
	max_value = rb->buff[start_index];
	for (icol = 0; icol < cols_to_use->size; icol++)
	{
		rb = kb_model->pdata_buffer->data + cols_to_use->data[icol];
		start_index = kb_model->sg_index & rb->mask;
		for (row = 0; row < kb_model->sg_length; row++)
		{
			if (abs_val)
				data = abs(rb->buff[start_index]);
			else
				data = rb->buff[start_index];
			start_index = (start_index + 1) & rb->mask;
			if (data > max_value)
			{
				max_col = icol + 1;
				max_value = data;
			}
		}
	}
	*pFV = (FLOAT)max_col;

	return 1;
}
