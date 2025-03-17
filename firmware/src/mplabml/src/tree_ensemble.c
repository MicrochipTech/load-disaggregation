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

#include "tree_ensemble.h"

tree_ensemble_classifier_rows_t *ClassifierTable;

// FILL_MAX_DECISION_TREE_ENSEMBLE_TMP_PARAMETERS
#ifndef MAX_DTE_CLASSIFICATIONS
#define MAX_DTE_CLASSIFICATIONS 10
#endif

uint8_t classification_counts[MAX_DTE_CLASSIFICATIONS];

uint8_t tree_classification(tree_t *model, uint8_t *feature_vector)
{
    int32_t classification = 0;
    int32_t current_node = 0;
    while (1)
    {
        if (feature_vector[model->features[current_node]] <= model->threshold[current_node])
        {
            current_node = model->left_children[current_node];
        }
        else
        {
            current_node = model->right_children[current_node];
        }
        if (model->right_children[current_node] == 0)
        {
            break;
        }
    }

    return model->features[current_node];
}

uint8_t ensemble_classification(tree_t *forest_ensemble, uint8_t *classification_counts, uint16_t number_of_trees, uint8_t number_of_classses, uint8_t *feature_vector)
{

    uint8_t y = 0;
    uint16_t max_count = 0;

    memset(classification_counts, 0, sizeof(uint8_t) * MAX_DTE_CLASSIFICATIONS);

    for (int32_t i = 0; i < number_of_trees; i++)
    {
        classification_counts[tree_classification(&forest_ensemble[i], feature_vector)]++;
    }

    max_count = classification_counts[0];

    for (int32_t i = 1; i < number_of_classses; i++)
    {
        if (classification_counts[i] > max_count)
        {
            max_count = classification_counts[i];
            y = i;
        }
    }

    return y + 1;
}

uint8_t tree_ensemble_simple_submit(uint8_t classifier_id, feature_vector_t *feature_vector, model_results_t *model_results)
{

    uint8_t y = 0;
    uint16_t max_count = 0;

    memset(classification_counts, 0, sizeof(uint8_t) * MAX_DTE_CLASSIFICATIONS);

    for (int32_t i = 0; i < ClassifierTable[classifier_id].number_of_trees; i++)
    {
        classification_counts[tree_classification(&ClassifierTable[classifier_id].tree_ensemble[i], (uint8_t *)feature_vector->data)]++;
    }

    max_count = classification_counts[0];
    model_results->output_tensor->data[0] = classification_counts[0];
    for (int32_t i = 1; i < ClassifierTable[classifier_id].number_of_classes; i++)
    {
        model_results->output_tensor->data[i] = (int16_t)classification_counts[i];
        if (classification_counts[i] > max_count)
        {
            max_count = classification_counts[i];

            y = i;
        }
    }

    model_results->result = (float)(y + 1);

    return y + 1;
}

void tree_ensemble_model_results_object(int32_t classifier_id, model_results_t *model_results)
{

    for (int32_t i = 0; i < ClassifierTable[classifier_id].number_of_classes; i++)
    {

        model_results->output_tensor->data[i] = classification_counts[i];
    }

    model_results->output_tensor->size = ClassifierTable[classifier_id].number_of_classes;
}

/*
 * Inititalize the PME
 */
void tree_ensemble_init(tree_ensemble_classifier_rows_t *classifier_table, const uint8_t num_classifiers)
{
    ClassifierTable = classifier_table;
}
