
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

#ifndef _AVSASR_ROCKHOPPER_MINI_FRONTEND_SRC_FFTR_H_
#define _AVSASR_ROCKHOPPER_MINI_FRONTEND_SRC_FFTR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file fftr.h
 * \brief In-place 512 bin FFTR
 */

//#include "mfcc_mini.h"

/**
 * \brief Calculates 512-bin in-place FFT
 *
 * \return applied number of right-shifts to fit 16-bit
 */
int32_t FFTR_512(int16_t *samples);

/**
 * Get the size of the ROM in Bytes.
 */
int32_t FFTR_GetRomSize(void);

#ifdef __cplusplus
}
#endif

#endif  /* define _AVSASR_ROCKHOPPER_MINI_FRONTEND_SRC_FFTR_H_ */
