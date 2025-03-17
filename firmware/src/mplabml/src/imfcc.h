
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

#ifndef _AVSASR_ROCKHOPPER_MINI_FRONTEND_SRC_IMFCC_H_
#define _AVSASR_ROCKHOPPER_MINI_FRONTEND_SRC_IMFCC_H_

/**
 * \file imfcc.h
 * \brief Integer Rockhopper MFCC frontend
 */

#include <stdint.h>
#include "kb_typedefs.h"
#include "rb.h"

#define IMFCC_FFT_SIZE 512
#define IMFCC_MAX_NUM_FEATURES 23

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Extract features for one frame of audio
 *
 * Note that feature extraction is done in-place meaning that the samples
 * buffer is overwritten.
 *
 * The samples buffer must have the size of IMFCC_GetFFTSize() + 2, not
 * IMFCC_GetWindowSize() and the spare values must all be 0.
 *
 * \param context Context that holds internal data of feature extraction
 * \param samples Array of audio samples
 * \param features Array that calculated features are stored in
 */
void IMFCC_ProcessFrame(    int16_t *samples,
                            int16_t sample_nrow,
                            int16_t window_size,
                            int32_t num_cepstra);

void IMFE_ProcessFrame(    int16_t *samples,
                            int16_t sample_nrow,
                            int16_t window_size);

// Exposing the following functions
void IMFCC_PreEmphasis(int16_t * data, int16_t length);
int32_t IMFCC_HannWindow(	int16_t *buffer);
void IMFCC_PowerTriFilter(	const int16_t *samples,
                                int32_t shift, int32_t *output);
void IMFCC_DCT(int32_t *input, int32_t *output, int32_t num_cepstra);
void IMFCC_Liftering(int32_t *MFCCs, int32_t num_cepstra);

#ifdef __cplusplus
}
#endif

#endif  /* define _AVSASR_ROCKHOPPER_MINI_DECODER_SRC_WFST_ON_THE_FLY_DECODER_H_ */
