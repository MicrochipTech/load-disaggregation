
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

#include "kbutils.h"

FLOAT stats_percentile_presorted(const int16_t *input_data, int32_t nframes, FLOAT pct)
{
	int32_t idx;
	int32_t nteger;
	FLOAT result;

	if (pct <= 0.0)
		return (FLOAT)input_data[0];
	if (pct >= 1.0)
		return (FLOAT)input_data[nframes - 1];

	// Compute percentile
	FLOAT index = pct * (nframes - 1);

	FLOAT finteger;
	FLOAT frac = (FLOAT)MODF(index, &finteger);

	if (finteger <= (FLOAT)(nframes - 1))
		nteger = (int32_t)finteger;
	else
		return input_data[nframes - 1];

	if (frac == 0.0)
	{
		idx = (int32_t)index;
		result = input_data[idx];
	}
	else
	{
		result = ((FLOAT)1.0 - frac) * input_data[nteger] + frac * input_data[nteger + 1];
	}

	return result;
}
