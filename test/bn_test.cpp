#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

void bn(float* dst, float* source, float* moving_mean, float* moving_var, unsigned int len, float weight, float bias, float eps) {
    for (unsigned int i = 0; i < len; ++i) {
        float inv_sqrt_var = 1.0f / sqrtf(moving_var[i] + eps);
        dst[i] = (source[i] - moving_mean[i]) * inv_sqrt_var * weight + bias;
    }
}
void batch_norm2d_mlu(float* dst, float* source, float *moving_mean, float *moving_var, unsigned int len, float weight, float bias, float eps = 1e-05);

int main() {
    const unsigned int len = 5; // 用于测试的数组长度
    float source[len] = {2.0f, 4.0f, 6.0f, 8.0f, 10.0f};
    float moving_mean[len] = {3.0f, 3.0f, 3.0f, 3.0f, 3.0f};
    float moving_var[len] = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f};
    float weight = 2.0f;
    float bias = 1.0f;
    float eps = 1e-5f;

    float result[len];

    bn(result, source, moving_mean, moving_var, len, weight, bias, eps);

    std::cout << "Input: ";
    for (unsigned int i = 0; i < len; ++i) {
        std::cout << source[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Output: ";
    for (unsigned int i = 0; i < len; ++i) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;

    batch_norm2d_mlu(result, source, moving_mean, moving_var, len, weight, bias, eps);
    std::cout << "Output: ";
    for (unsigned int i = 0; i < len; ++i) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}