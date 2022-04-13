#pragma once

#include <RTNeural/RTNeural.h>
#include <nlohmann/json.hpp>

class RT_LSTM
{
public:
    RT_LSTM() = default;

    void reset();
    void load_json3(const nlohmann::json& weights_json);

    void process(const float* inData, float param1, float* outData, int numSamples);

    int input_size = 2;

    float previousParam1 = 0.0;
    float steppedValue1 = 0.0;
    bool changedParam1 = false;


private:
    RTNeural::ModelT<float, 2, 1,
        RTNeural::LSTMLayerT<float, 2, 16>,
        RTNeural::DenseT<float, 16, 1>> model_cond2;

    float inArray alignas(16)[2] = { 0.0, 0.0 };
};
