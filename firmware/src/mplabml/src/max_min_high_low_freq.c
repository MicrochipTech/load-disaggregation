
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

/*
 * max_min_high_low_freq - compute the maximum and minimum of the high or low frequency
 *							 component of a waveform.
 *
 * Inputs
 *		nframes - number of rows in the array
 *       col - which column to analyze
 * 		offset - offset into the column to start
 *		sf	- smoothing factor (how many data points on either side of a sample to use to
 *				compute the mean.)
 *		max = pointer to FLOAT to receive the maximum value.
 *		min = pointer to FLOAT to receive the minimum value.
 *		lowhigh  - 1 = use high-freq values, 0 = use low freq values.
 */
int32_t max_min_high_low_freq(ringb *pringb, int32_t base_index, int32_t nframes, int32_t col, int32_t offset, int32_t sf, int32_t lowhigh, FLOAT *max, FLOAT *min)
{
	int32_t i;
	ringb *rb = pringb + col;
	int32_t start_index = (base_index + offset) & rb->mask;
	FLOAT dsum;
	FLOAT davg;
	FLOAT dmin = KB_FLT_MAX;
	FLOAT dmax = KB_FLT_MIN;
	FLOAT dval;
	FLOAT dval0 = (FLOAT)rb->buff[start_index];

	// Compute the running mean for the first sample
	for (i = -sf / 2, dsum = 0.0; i < sf / 2; i++)
	{
		start_index = (base_index + offset + i) & rb->mask;
		if (i < 0)
		{
			dsum += dval0; // pad with first value on left side.
		}
		else
		{
			dsum += (FLOAT)rb->buff[start_index];
		}
	}
	davg = dsum / sf;

	// Subtract the mean from the first sample
	dval = dval0 - davg;

	// Compare to max/min
	if (dval < dmin)
		dmin = dval;
	if (dval > dmax)
		dmax = dval;

	start_index = (base_index + offset) & rb->mask;
	// Compute the running mean for the each sample
	for (i = 1; i < nframes; i++)
	{
		int32_t decndx = i - sf / 2;
		int32_t addndx = i + sf / 2;
		int32_t dec = decndx < 0 ? dval0 : rb->buff[(base_index + decndx + offset) & rb->mask];
		int32_t add = addndx >= nframes ? rb->buff[(base_index + nframes + offset - 1) & rb->mask] : rb->buff[(base_index + addndx + offset) & rb->mask];

		dsum = dsum - dec + add;
		davg = dsum / sf;

		start_index = (start_index + 1) & rb->mask;
		switch (lowhigh)
		{
		case MOD_HF: // Subtract the mean to get the high freq components.
			dval = rb->buff[start_index] - davg;
			break;

		case MOD_LF:
			dval = davg; // Return the mean for the low freq components.
			break;

		case MOD_RAW:
			dval = rb->buff[start_index];
			break;
		}
		if (dval < dmin)
			dmin = dval;
		if (dval > dmax)
			dmax = dval;
	}
	*max = dmax;
	*min = dmin;

	return 1;
}
