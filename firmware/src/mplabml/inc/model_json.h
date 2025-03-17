#ifndef __MODEL_JSON_H__
#define __MODEL_JSON_H__

const char recognition_model_string_json[] = {"{\"NumModels\":1,\"ModelIndexes\":{\"0\":\"PIPELINE_LOAD_2_RANK_0\"},\"ModelDescriptions\":[{\"Name\":\"PIPELINE_LOAD_2_RANK_0\",\"ClassMaps\":{\"1\":\"fan\",\"2\":\"heater_R1\",\"3\":\"heater_R2\",\"4\":\"lightbulb\",\"5\":\"noLoad\",\"0\":\"Unknown\"},\"ModelType\":\"PME\",\"FeatureFunctions\":[\"75thPercentile\",\"AbsoluteSum\",\"AbsoluteSum\",\"AbsoluteSum\",\"AbsoluteSum\",\"Mean\",\"Mean\",\"Mean\"]}]}"};

int32_t recognition_model_string_json_len = sizeof(recognition_model_string_json);

#endif /* __MODEL_JSON_H__ */
