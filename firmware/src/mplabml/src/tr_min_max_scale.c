
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

/**
 * Transcoded from min_max_scale from tr_scale.py
 */
int32_t min_max_scale(float *pFeatures, feature_vector_t *feature_vector, int32_t nfeats, int32_t start, int32_t total_features, FLOAT minbound, FLOAT maxbound, struct minmax *m)
{
	int32_t index;
	int32_t i;
	FLOAT value;
	float *pdata_float;
	uint8_t *pdata_uint8;

	pdata_uint8 = (uint8_t *)feature_vector->data;
	pdata_float = (float *)feature_vector->data;

	for (i = 0; i < nfeats; i++)
	{
		index = (start + m[i].index) % total_features;
		// printf("%d: %f\n", index, pFeatures[index]);
		value = maxbound * (pFeatures[index] - m[i].min) / (m[i].max - m[i].min + 1.0e-10);
		if (value > maxbound)
			value = maxbound;
		if (value < minbound)
			value = minbound;
		if (feature_vector->typeID == 1)
		{
			pdata_uint8[i] = (uint8_t)value;
		}
		else
		{
			pdata_float[i] = (float)value;
		}
	}
	return nfeats;
}