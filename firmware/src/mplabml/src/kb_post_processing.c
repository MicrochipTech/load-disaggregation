
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
#include "kb_post_processing.h"

void setup_feature_set(feature_set_t *feature_set, uint16_t size, uint16_t feature_length, FLOAT *pbuffer)
{

    feature_set->data = pbuffer;
    feature_set->sum = pbuffer + size * feature_length;
    feature_set->average = pbuffer + size * feature_length + feature_length;

    feature_set->size = size;
    feature_set->feature_length = feature_length;

    init_feature_set(feature_set);
}

uint_fast16_t get_buffer_size(uint16_t size, uint16_t feature_length)
{
    return size * feature_length + feature_length + feature_length;
}

int init_feature_set(feature_set_t *feature_set)
{
    if (feature_set == NULL)
    {
        return 1;
    }
    feature_set->index = 0;
    feature_set->num_elements = 0;

    for (int i = 0; i < (int)feature_set->feature_length * feature_set->size; i++)
    {
        feature_set->data[i] = 0;
    }

    for (int i = 0; i < (int)feature_set->feature_length; i++)
    {
        feature_set->sum[i] = 0;
        feature_set->average[i] = 0;
    }

    return 0;
}

int push_feature(feature_set_t *feature_set, FLOAT *feature)
{
    if (feature_set == NULL)
        return 1;

    int ix = (int)feature_set->index;
    int N = (int)feature_set->feature_length;

    for (int i = 0; i < N; i++)
    {
        feature_set->sum[i] += (feature[i] - feature_set->data[ix * N + i]);
        feature_set->data[ix * N + i] = feature[i];
    }

    feature_set->num_elements = min(feature_set->num_elements + 1, feature_set->size);

    for (int i = 0; i < N; i++)
    {
        feature_set->average[i] = (FLOAT)feature_set->sum[i] / feature_set->num_elements;
    }

    feature_set->index = MOD(feature_set->index + 1, feature_set->size);

    return 0;
}

int pop_first_feature(feature_set_t *feature_set)
{
    if (feature_set == NULL)
        return 1;

    if (feature_set->num_elements == 0)
        return 2;

    int N = (int)feature_set->feature_length;
    uint16_t head_index = head_index_feature_set(feature_set);

    for (int i = 0; i < N; i++)
    {
        feature_set->sum[i] -= feature_set->data[head_index * N + i];
    }

    feature_set->num_elements = feature_set->num_elements - 1;

    for (int i = 0; i < N; i++)
    {
        feature_set->average[i] = (FLOAT)feature_set->sum[i] / feature_set->num_elements;
    }

    return 0;
}

int pop_last_feature(feature_set_t *feature_set)
{
    if (feature_set == NULL)
        return 1;

    if (feature_set->num_elements == 0)
        return 2;

    int N = (int)feature_set->feature_length;
    uint16_t tail_index = tail_index_feature_set(feature_set);

    for (int i = 0; i < N; i++)
    {
        feature_set->sum[i] -= feature_set->data[tail_index * N + i];
    }

    feature_set->num_elements = feature_set->num_elements - 1;

    for (int i = 0; i < N; i++)
    {
        feature_set->average[i] = (FLOAT)feature_set->sum[i] / feature_set->num_elements;
    }

    feature_set->index = MOD(feature_set->index - 1, feature_set->size);

    return 0;
}

uint16_t head_index_feature_set(feature_set_t *feature_set)
{
    return MOD(feature_set->index - feature_set->num_elements, feature_set->size);
}

uint16_t tail_index_feature_set(feature_set_t *feature_set)
{
    return MOD(feature_set->index - 1, feature_set->size);
}

uint16_t base_index_feature_set(feature_set_t *feature_set)
{
    return MOD(feature_set->index - feature_set->num_elements, feature_set->size);
}

void get_feature_at_index(feature_set_t *feature_set, uint16_t index, FLOAT *feature)
{
    uint16_t i = MOD(base_index_feature_set(feature_set) + index, feature_set->size);

    get_feature_at_dynamic_index_(feature_set, i, feature);
}

void get_feature_at_dynamic_index_(feature_set_t *feature_set, uint16_t i, FLOAT *feature)
{
    for (int j = 0; j < feature_set->feature_length; j++)
    {
        feature[j] = feature_set->data[feature_set->feature_length * i + j];
    }
}

void get_head_feature_set(feature_set_t *feature_set, FLOAT *head)
{
    uint16_t index = head_index_feature_set(feature_set);
    get_feature_at_dynamic_index_(feature_set, index, head);
}

void get_tail_feature_set(feature_set_t *feature_set, FLOAT *tail)
{
    uint16_t index = tail_index_feature_set(feature_set);
    get_feature_at_dynamic_index_(feature_set, index, tail);
}
