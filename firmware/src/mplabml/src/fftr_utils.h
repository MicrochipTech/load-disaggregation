
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

/**
 * \file fftr_utils.h
 * \brief utility functions for using in-place 512 bin FFTR
 */

#ifndef _FFTR_UTILS_H_
#define _FFTR_UTILS_H_

#include "kb_typedefs.h" // struct compx

// for unit testing with C++ framework:
#ifdef __cplusplus
extern "C"
{
#endif

// number of floats expected:
#define NUM_FFTR (512)
#define NUM_FFTR_CMPX (256)

    // FLOAT-based and int16-based wrappers for FFTR_512()
    //
    // params:
    // -- <input_data> ... points to a buffer containing up to 512 FLOAT values
    //    Because this buffer is re-used to return results, it must be large
    //    enough to hold 512 FLOAT values.
    // -- <len> ... FFT length, must be <= 512 and must be a multiple of 2;
    //    input data will be padded out to a len of 512 by adding zeroes.
    //
    // return:
    // -- on success ... pointer to 256 compx structs, each containing a FLOAT real and a FLOAT imaginary;
    //                   actually, just a cast of the pointer to the buffer containing the input data.
    // -- otherwise .... NULL, indicating an input parameter error
    //

    // The basic float-based wrapper, applies Hanning window to data, converts FLOAT inputs to int16, calls FFTR_512(),
    // and converts results back to FLOAT.
    struct compx *fftr_fl(FLOAT *input_data, int32_t len);

    // This applies Hanning and then auto-scales:
    struct compx *fftr_fl_as(FLOAT *input_data, int32_t len);

    // This removes the mean, applies Hanning, and then auto-scales:
    struct compx *fftr_fl_rm_as(FLOAT *input_data, int32_t len);

    // The basic int16-based wrapper, applies Hanning window and calls FFTR_512():
    struct compx_int16_t *fftr(int16_t *input_data, int32_t len);

    // This applies Hanning and then auto-scales:
    struct compx_int16_t *fftr_as(int16_t *input_data, int32_t len);

    // This removes the mean, applies Hanning, and then auto-scales:
    struct compx_int16_t *fftr_rm_as(int16_t *input_data, int32_t len);

#ifdef __cplusplus
}
#endif

#endif // defind _FFTR_UTILS_H_
