
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

#ifndef __TREE_ENSEMBLE_H__
#define __TREE_ENSEMBLE_H__

// clang-format off
#ifdef __cplusplus
extern "C"
{
#endif

#include "kbutils.h"

typedef struct tree
{
    uint16_t *left_children;
    uint16_t *right_children;
    uint8_t *threshold;
    uint16_t *features;
} tree_t;

typedef struct tree_ensemble_classifier_rows
{
    uint8_t number_of_classes;
    uint16_t number_of_trees;
    tree_t *tree_ensemble;
} tree_ensemble_classifier_rows_t;



uint8_t ensemble_classification(tree_t *forest_ensemble, uint8_t *classification_counts, uint16_t number_of_trees, uint8_t number_of_classses, uint8_t *feature_vector);
uint8_t tree_classification(tree_t *model, uint8_t *feature_vector);
uint8_t tree_ensemble_simple_submit(uint8_t classifier_id, feature_vector_t *feature_vector, model_results_t * model_results);
void tree_ensemble_init(tree_ensemble_classifier_rows_t *classifier_table, const uint8_t num_classifiers);
void tree_ensemble_model_results_object(int32_t classifier_id, model_results_t *model_results);

#ifdef __cplusplus
}
#endif

// clang-format on

#endif // __TREE_ENSEMBLE_H__
