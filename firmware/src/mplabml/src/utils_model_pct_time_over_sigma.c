
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

int32_t utils_model_pct_time_over_sigma(kb_model_t *kb_model, int16_data_t *cols_to_use, float_data_t *params, FLOAT *pFV, int32_t sigma_level)
{

#if SML_DEBUG
	if (!kb_model || kb_model->sg_length <= 0 || !cols_to_use || cols_to_use->size <= 0 || !pFV)
	{
		return 0;
	}
#endif
	int32_t icol;
	int32_t base_index = kb_model->sg_index;
	ringb *rb;
	int32_t start_index;

	for (icol = 0; icol < cols_to_use->size; icol++)
	{
		int32_t irow;
		int32_t sum = 0;
		rb = kb_model->pdata_buffer->data + cols_to_use->data[icol];

		FLOAT tmp = mean(rb, base_index, kb_model->sg_length) + sigma_level * kb_std(rb, base_index, kb_model->sg_length);
		start_index = base_index & rb->mask;
		for (irow = 0; irow < kb_model->sg_length; irow++)
		{
			FLOAT val = (FLOAT)MOD_READ_RINGBUF(rb, start_index++);
			if (val > tmp)
			{
				sum++;
			}
		}

		pFV[icol] = (FLOAT)sum / (FLOAT)kb_model->sg_length;
	}

	return cols_to_use->size;
}
