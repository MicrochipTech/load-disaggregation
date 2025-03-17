
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

int32_t calculate_positive_crossing_xor(int16_t *signal, int32_t length)
{
	int32_t positive_crossings = 0;
	int32_t prev_sign = 1; // Initialize with positive sign

	for (int32_t i = 1; i < length; i++)
	{
		int32_t curr_sign = (signal[i] > 0) ? 1 : 0;
		// Check sign change and positive edge with XOR and AND
		int32_t sign_change_and_positive = (prev_sign ^ curr_sign) & curr_sign;
		positive_crossings += sign_change_and_positive;

		prev_sign = curr_sign;
	}

	return positive_crossings; // Normalize by signal length
}

int32_t calculate_negative_crossing_xor(int16_t *signal, int32_t length)
{
	int32_t negative_crossings = 0;
	int32_t prev_sign = 1; // Initialize with positive sign

	for (int32_t i = 1; i < length; i++)
	{
		int32_t curr_sign = (signal[i] > 0) ? 1 : 0;
		// Sign change check with XOR
		int32_t sign_change = prev_sign ^ curr_sign;
		// Negative edge check with AND and mask
		int32_t negative_edge = sign_change & ~curr_sign; // Ensures change to negative
		negative_crossings += negative_edge;

		prev_sign = curr_sign;
	}

	return negative_crossings; // Normalize by signal length
}

int32_t calculate_zc_with_xor(int16_t *signal, int32_t length)
{
	int32_t crossings = 0;
	int32_t prev_sign_bit = (signal[0] > 0) ? 0 : 1; // 0 for positive, 1 for negative

	for (int32_t i = 1; i < length; i++)
	{
		int32_t curr_sign_bit = (signal[i] > 0) ? 0 : 1;
		crossings += prev_sign_bit ^ curr_sign_bit; // Check sign change with XOR
		prev_sign_bit = curr_sign_bit;
	}

	return crossings;
}

int32_t calculate_zc_with_threshold_xor(int16_t *signal, int32_t length, int32_t threshold)
{
	int32_t crossings = 0;
	int32_t prev_sign_bit = (signal[0] > threshold) ? 0 : 1;
	int32_t curr_sample_bit, sign_change_bit;

	// printf("Threshold %d\n", threshold);

	for (int32_t i = 1; i < length; i++)
	{
		// printf("%d %d  ", signal[i - 1], signal[i]);
		curr_sample_bit = (signal[i] > threshold) ? 0 : 1;
		sign_change_bit = prev_sign_bit ^ curr_sample_bit; // XOR for sign change
		crossings += sign_change_bit;					   // Count only sign changes above threshold
		// printf("curr_sample_bit %d, sign_change_bit %d, crossings %d\n", curr_sample_bit, sign_change_bit, crossings);
		prev_sign_bit = curr_sample_bit;
	}

	return crossings;
}

int32_t calculate_negative_crossing_rate_xor_threshold(int16_t *signal, int32_t length, int32_t threshold)
{
	int32_t negative_crossings = 0;
	int32_t prev_sign = (signal[0] > threshold) ? 1 : 0; // Initialize with sign based on threshold

	for (int32_t i = 1; i < length; i++)
	{
		int32_t curr_sign = (signal[i] > threshold) ? 1 : 0;
		// Sign change check with XOR
		int32_t sign_change = prev_sign ^ curr_sign;
		// Negative edge check with AND, mask, and threshold check
		int32_t negative_edge = sign_change & ~curr_sign & (signal[i] <= threshold); // Ensure change to negative and below threshold
		negative_crossings += negative_edge;

		prev_sign = curr_sign;
	}

	return negative_crossings;
}

int32_t calculate_positive_crossing_rate_xor_threshold(int16_t *signal, int32_t length, int32_t threshold)
{
	int32_t positive_crossings = 0;
	int32_t prev_sign = (signal[0] > threshold) ? 0 : 1; // Initialize with negative sign (opposite of positive)

	for (int32_t i = 1; i < length; i++)
	{

		int32_t curr_sign = (signal[i] > threshold) ? 0 : 1;
		// Sign change check with XOR
		int32_t sign_change = prev_sign ^ curr_sign;
		// Positive edge check with AND, mask, and threshold check
		int32_t positive_edge = sign_change & ~curr_sign & (signal[i] > threshold); // Ensure change to positive and above threshold
		positive_crossings += positive_edge;
		// printf("curr_sample_bit %d, sign_change_bit %d, positive_edge %d , pcrossings %d \n", curr_sign, sign_change, positive_edge, positive_crossings);
		prev_sign = curr_sign;
	}

	// printf("Possitve Corssings %d\n\n", positive_crossings);

	return positive_crossings;
}