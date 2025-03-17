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

#ifndef KB_PIPELINE_H
#define KB_PIPELINE_H

#ifdef WIN32
#include <stdio.h>
#include <math.h>
#include <time.h>
#else
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <stdint.h>
#include <stddef.h>

#include "kb_defines.h"
#include "kb_common.h"
#include "kb_typedefs.h"
#include "kbalgorithms.h"
#include "kbutils.h"

#ifdef __cplusplus
extern "C"
{
#endif

    #define PHASE_S_PIPELINE_LOAD_2_RANK_0 0
#define CURRENT_S_PIPELINE_LOAD_2_RANK_0 1
#define ACTIVEPOWER_S_PIPELINE_LOAD_2_RANK_0 2
#define REACTIVEPOWER_S_PIPELINE_LOAD_2_RANK_0 3
#define HARMONIC1_S_PIPELINE_LOAD_2_RANK_0 4
#define HARMONIC3_S_PIPELINE_LOAD_2_RANK_0 5

    #define PHASE_D_PIPELINE_LOAD_2_RANK_0 0
#define CURRENT_D_PIPELINE_LOAD_2_RANK_0 1
#define ACTIVEPOWER_D_PIPELINE_LOAD_2_RANK_0 2
#define REACTIVEPOWER_D_PIPELINE_LOAD_2_RANK_0 3
#define HARMONIC1_D_PIPELINE_LOAD_2_RANK_0 4
#define HARMONIC3_D_PIPELINE_LOAD_2_RANK_0 5

    
int32_t data_streaming_0(void* model, int16_t *pSample);

int32_t data_segmentation_0(void* model, int32_t model_index);

int32_t feature_gen_0(void * model, int32_t *nfeats);

int32_t feature_transform_0(void* model);

int32_t recognize_vector_0(void* model);


#ifdef __cplusplus
}
#endif

#endif // KB_PIPELINE_H
