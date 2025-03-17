
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

int32_t ratio_diff_impl(ringb *rb, int32_t base_index, int32_t nlen, int32_t half_winsize, int32_t flag_h_l, float *out)
{
	int32_t i, window_size;
	int32_t start_index = base_index + nlen * flag_h_l;
	float sum_mem = 0, sumtemp = 0;

	float max = -KB_FLT_MAX, min = KB_FLT_MAX;

	window_size = 2 * half_winsize + 1;

	sum_mem = 0;
	for (i = 0; i < window_size; i++)
	{
		sum_mem += MOD_READ_RINGBUF(rb, start_index++);
	}
	start_index = base_index + nlen * flag_h_l + half_winsize;
	for (i = half_winsize; i < nlen - half_winsize; i++)
	{
		sumtemp = sum_mem;
		sumtemp = sumtemp / (window_size);
		sumtemp = rb->buff[start_index] - sumtemp;
		sum_mem = sum_mem - MOD_READ_RINGBUF(rb, (start_index - half_winsize)) +
				  MOD_READ_RINGBUF(rb, (start_index + half_winsize + 1));
		start_index++;
		if (sumtemp > max)
			max = sumtemp;
		if (sumtemp < min)
			min = sumtemp;
	}

	*out = max - min;
	return 1;
}
