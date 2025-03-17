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

int32_t util_model_check_positive_crossing(int32_t first, int32_t second, int32_t positive_threshold, int32_t negative_threshold)
{

    int32_t ncrossings = 0;

    // Positive Threshold Value Crossing

    // Postive Crossing
    if ((first < positive_threshold) && (second > positive_threshold))
    {
        ncrossings++;
    }

    // Negative Threshold Value Crossing

    // Postive Crossing
    if ((first < negative_threshold) && (second > negative_threshold))
    {
        ncrossings++;
    }

    return ncrossings;
}

int32_t util_model_check_negative_crossing(int32_t first, int32_t second, int32_t positive_threshold, int32_t negative_threshold)
{

    int32_t ncrossings = 0;

    // Positive Threshold Value Crossing
    // Negative Crossing
    if ((first > positive_threshold) && (second < positive_threshold))
    {
        ncrossings++;
    }

    // Negative Threshold Value Crossing
    // Negative Crossing
    if ((first > negative_threshold) && (second < negative_threshold))
    {
        ncrossings++;
    }

    return ncrossings;
}

int32_t utils_model_crossing_rate(ringb *rb, int32_t base_index, int32_t num_rows, int32_t positive_threshold, int32_t negative_threshold, crossingType cross_type)
{
    int32_t ncrossings = 0;
    int32_t first;  // must be 32-bit int32_t for bit mask operations
    int32_t second; // must be 32-bit int32_t for bit mask operations
    int32_t i;
    int32_t start_index = base_index & rb->mask;

    first = rb->buff[start_index++];

    for (i = 1; i < num_rows; i++)
    {
        second = MOD_READ_RINGBUF(rb, start_index++);

        switch (cross_type)
        {
        case NUMBER_OF_CROSSINGS_OVER_THRESHOLD_REGIONS:
            ncrossings += util_model_check_positive_crossing(first, second, positive_threshold, negative_threshold);
            ncrossings += util_model_check_negative_crossing(first, second, positive_threshold, negative_threshold);
            break;
        case NUMBER_OF_POSITIVE_CROSSINGS_OVER_THRESHOLD_REGIONS:
            ncrossings += util_model_check_positive_crossing(first, second, positive_threshold, negative_threshold);
            break;
        case NUMBER_OF_NEGATIVE_CROSSINGS_OVER_THRESHOLD_REGIONS:
            ncrossings += util_model_check_negative_crossing(first, second, positive_threshold, negative_threshold);
            break;
        default:
            break;
        }

        first = second;
    }

    printf("Number Crossing %d\n", ncrossings);

    return ncrossings;
}
