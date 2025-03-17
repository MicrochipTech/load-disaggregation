
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

int32_t fg_stats_variance(kb_model_t *kb_model, int16_data_t *cols_to_use, float_data_t *params, FLOAT *pFV)
{
	FLOAT mean = 0.0;
	FLOAT M2 = 0.0;
	int32_t irow, icol, base_index, num_rows;
	FLOAT delta, datapoint;
	ringb *rb;
	int32_t start_index;
	base_index = kb_model->sg_index;
	num_rows = kb_model->sg_length;

	for (icol = 0; icol < cols_to_use->size; icol++)
	{
		rb = kb_model->pdata_buffer->data + cols_to_use->data[icol];
		start_index = base_index;
		short n = 0;

		for (irow = 0; irow < num_rows; irow++)
		{
			n++;
			datapoint = MOD_READ_RINGBUF(rb, start_index++);
			delta = datapoint - mean;
			mean += delta / n;
			M2 += delta * (datapoint - mean);
		}
		if (n < 2)
		{
			pFV[icol] = 0;
		}
		else
		{
			pFV[icol] = M2 / (n - 1);
		}
	}
	return cols_to_use->size;
}
