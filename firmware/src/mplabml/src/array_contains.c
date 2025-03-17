
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

//              array_contains.c
//
//
//

bool selection_contains(int32_t feature, int32_t num_feature_selection,
                        int32_t *feature_selection)
//
// Given a feature number, return [is this feature in the list?]
// If the list is NULL, then return true.
//
{
    int32_t i;

    if (num_feature_selection == 0)
        return true;

    for (i = num_feature_selection; i > 0; i--)
    {
        if (*feature_selection++ == feature)
        {
            return true;
        }
    }

    return false;
}

int32_t selection_index(int32_t feature, int32_t num_feature_selection,
                        int32_t *feature_selection)
//
// Given a feature number, return the correct bin index.
// In other words, translate a selected feature back into the correct bin index.
// If the feature is not found, return -1.
//
//
{
    int32_t i;

    for (i = 0; i < num_feature_selection; i++)
    {
        if (*feature_selection++ == feature)
        {
            return i;
        }
    }

    return -1;
}

//
