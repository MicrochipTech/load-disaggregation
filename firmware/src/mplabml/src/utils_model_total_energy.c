
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

int32_t utils_model_total_energy(kb_model_t *kb_model, int16_data_t *cols_to_use, float_data_t *params, FLOAT *pFV, int32_t abs_val)
{
#if SML_DEBUG
	if (!kb_model || kb_model->sg_length <= 0 || !cols_to_use || cols_to_use->size <= 0 || params->size != 0 || !pFV)
	{
		return 0;
	}
#endif

	float sum = 0;
	int32_t row, icol;
	short data;
	int32_t base_index = kb_model->sg_index;
	ringb *rb;

	for (icol = 0; icol < cols_to_use->size; icol++)
	{
		rb = kb_model->pdata_buffer->data + cols_to_use->data[icol];
		base_index = kb_model->sg_index;

		for (row = 0; row < kb_model->sg_length; row++)
		{
			data = MOD_READ_RINGBUF(rb, base_index++);
			if (abs_val)
				sum += abs(data);
			else
				sum += data * data;
		}
	}
	if (abs_val)
		*pFV = (float)sum;
	else
		*pFV = ((float)sum) / kb_model->sg_length;

	return 1;
}
