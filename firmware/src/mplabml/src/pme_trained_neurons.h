#ifndef _PME_TRAINED_NEURONS_H_
#define _PME_TRAINED_NEURONS_H_

#include "pme.h"
#include "kb_classifier.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KB_DISTANCE_L1 QM_PME_L1_DISTANCE
#define KB_DISTANCE_LSUP QM_PME_LSUP_DISTANCE
#define KB_DISTANCE_DTW QM_PME_DTW_DISTANCE
#define KB_CLASSIFICATION_RBF QM_PME_RBF_CLASSIFICATION
#define KB_CLASSIFICATION_KNN QM_PME_KNN_CLASSIFICATION

#define NUM_NEURONS_0 (6)
#define MAX_NUM_NEURONS_0 (6)
#define NUM_CLASSES_0 (6)
#define KB_TOTAL_NUMBER_OF_NEURONS (6)
#define KB_NUM_PME_CLASSIFIERS (1)

extern kb_classifier_row_t kb_classifier_rows[];

extern const int32_t neurons_count;

#ifdef __cplusplus
}
#endif

#endif //_PME_TRAINED_NEURONS_H_
