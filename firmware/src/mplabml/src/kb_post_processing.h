
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

#ifndef _KB_POSTPROCESSING_H_
#define _KB_POSTPROCESSING_H_

#include "kb_typedefs.h"

/*
    Tools for post processing the outputs of a model
*/

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct feature_set
    {
        uint16_t size;
        uint16_t num_elements;
        uint16_t index;
        uint16_t feature_length;
        FLOAT *data;
        FLOAT *sum;
        FLOAT *average;
    } feature_set_t;

    /**
     * @brief Setup an instance of the feature set
     *
     * Use "get_buffer_size" function to determine the size of the buffer.
     * Allocate the required memory for the buffer separately and pass the pointer.
     *
     * @param[in] feature_set Pointer to the feature set object
     * @param[in] size maximum number of stored feature vectors
     * @param[in] feature_length number of elements in each feature vector
     * @param[in] pbuffer pointer to the buffer to store data of the set
     *
     * @returns The instance of feature set
     */
    void setup_feature_set(feature_set_t *feature_set, uint16_t size, uint16_t feature_length, FLOAT *pbuffer);

    /**
     * @brief Initializing the feature set
     *
     * @param[in] feature_set Pointer to the feature set object
     *
     * @returns 0 if successful, != 0 otherwise
     */
    int init_feature_set(feature_set_t *feature_set);

    /**
     * @brief Determine the size of the array buffer required to store data for the set
     *
     * @param[in] size maximum number of stored feature vectors
     * @param[in] feature_length number of elements in each feature vector
     *
     * @returns buffer size
     */
    uint_fast16_t get_buffer_size(uint16_t size, uint16_t feature_length);

    /**
     * @brief Push a feature vector into the set
     *
     * Use this function to determine the size of the memory required
     * by "feature_set" and pass the pointer to the allocated memory to "setup_feature_set"
     *
     * @param[in] feature_set pointer referring to the feature set object
     * @param[in] feature pointer to the input feature array
     *
     * @returns 0 if successful, != 0 otherwise
     */
    int push_feature(feature_set_t *feature_set, FLOAT *feature);

    /**
     * @brief Pop the last element of the set
     *
     * The last feature is removed and the set properties (sum, average, etc) are updated
     *
     * @param[in] feature_set pointer referring to the feature set object
     *
     * @returns 0 if successful, 1 for null feature sets, 2 for empty sets
     */
    int pop_last_feature(feature_set_t *feature_set);

    /**
     * @brief Pop the first element of the set
     *
     * The first feature is removed and the set properties (sum, average, etc) are updated
     *
     * @param[in] feature_set pointer referring to the feature set object
     *
     * @returns 0 if successful, 1 for null feature sets, 2 for empty sets
     */
    int pop_first_feature(feature_set_t *feature_set);

    /**
     * @brief Get index of the head feature of the set
     * @param[in] feature_set pointer referring to the feature set object
     *
     * @returns head index
     */
    uint16_t head_index_feature_set(feature_set_t *feature_set);

    /**
     * @brief Get index of the tail feature of the set

    * @param[in] feature_set pointer referring to the feature set object
    *
    * @returns tail index
    */
    uint16_t tail_index_feature_set(feature_set_t *feature_set);

    /**
     * @brief Get the head feature of the set
     *
     * @param[in] feature_set pointer referring to the feature set object
     * @param[out] head pointer to the output array to store the head feature
     *
     */
    void get_head_feature_set(feature_set_t *feature_set, FLOAT *head);

    /**
     * @brief Get the tail feature of the set
     *
     * @param[in] feature_set pointer referring to the feature set object
     * @param[out] tail pointer to the output array to store the tail feature
     *
     */
    void get_tail_feature_set(feature_set_t *feature_set, FLOAT *tail);

    /**
     * @brief Get a feature of the set, given its index
     *
     * index starts from 0 and is incremented by one to access arbitrary features in the set
     * For example, set index = 0 always refers to the head feature
     * For a set of size 5 that currently stores 3 features, the valid indices are 0, 1 and 2
     *
     * @param[in] feature_set pointer referring to the feature set object
     * @param[in] index index of the requested feature
     * @param[out] feature pointer to the output array to store the requested feature
     *
     */
    void get_feature_at_index(feature_set_t *feature_set, uint16_t index, FLOAT *feature);

    /**
     * @brief Get a feature of the set, given its index with respect to the internal structure of the set
     *
     * @param[in] feature_set pointer referring to the feature set object
     * @param[in] index index of the requested feature
     * @param[out] feature pointer to the output array to store the requested feature
     *
     */
    void get_feature_at_dynamic_index_(feature_set_t *feature_set, uint16_t index, FLOAT *feature);

    /**
     * @brief Get the internal index of the head feature
     *
     * @param[in] feature_set pointer referring to the feature set object
     *
     * @returns the base index of the set
     *
     */
    uint16_t base_index_feature_set(feature_set_t *feature_set);

#ifdef __cplusplus
}
#endif
// clang-format on

#endif //_KB_POSTPROCESSING_H_
