#include "pme_trained_neurons.h"

static qm_pme_neuron_vector_t kb_neuron_vectors_0[MAX_NUM_NEURONS_0] = 
{
	{.vector={252, 250, 0, 6, 11, 0, 130, 185}},
	{.vector={128, 0, 12, 181, 0, 5, 218, 178}},
	{.vector={128, 0, 202, 35, 233, 131, 109, 14}},
	{.vector={127, 0, 184, 174, 1, 253, 40, 177}},
	{.vector={158, 61, 226, 66, 96, 2, 160, 245}},
	{.vector={130, 5, 0, 1, 2, 0, 128, 179}},
};
qm_pme_neuron_attribute_t kb_neuron_attribs_0[MAX_NUM_NEURONS_0] = 
{
	{ .influence=150, .category=5 },
	{ .influence=150, .category=4 },
	{ .influence=150, .category=2 },
	{ .influence=150, .category=3 },
	{ .influence=150, .category=1 },
	{ .influence=150, .category=5 },
};
kb_classifier_row_t kb_classifier_rows[1] = {
	

	{
		.classifier_id=0,
		.num_patterns=NUM_NEURONS_0,
		.pattern_size=8,
		.max_patterns=MAX_NUM_NEURONS_0,
		.num_classes=NUM_CLASSES_0,
		.num_channels=1,
		.classifier_mode=KB_CLASSIFICATION_KNN,
		.norm_mode=KB_DISTANCE_L1,
		.stored_patterns=kb_neuron_vectors_0,
		.stored_attribs=kb_neuron_attribs_0,
	},
};

const int32_t neurons_count = KB_TOTAL_NUMBER_OF_NEURONS;
