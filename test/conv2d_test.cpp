#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <assert.h>

void conv(float *dst, float *src, float *kernel, unsigned int channel_input, unsigned int height, unsigned int width, unsigned int kernel_height, unsigned int kernel_width, unsigned int stride_width, unsigned int stride_height, unsigned int channel_output) {
    // Calculate output dimensions
    unsigned int output_height = (height - kernel_height) / stride_height + 1;
    unsigned int output_width = (width - kernel_width) / stride_width + 1;

    // Perform convolution
    for (unsigned int co = 0; co < channel_output; co++) {
        for (unsigned int ho = 0; ho < output_height; ho++) {
            for (unsigned int wo = 0; wo < output_width; wo++) {
                // Calculate output index
                unsigned int dst_index = co * output_height * output_width + ho * output_width + wo;

                // Initialize output value
                float output_value = 0;

                for (unsigned int ci = 0; ci < channel_input; ci++) {
                    for (unsigned int hk = 0; hk < kernel_height; hk++) {
                        for (unsigned int wk = 0; wk < kernel_width; wk++) {
                            // Calculate input and kernel indices
                            unsigned int src_index = ci * height * width + (ho * stride_height + hk) * width + (wo * stride_width + wk);
                            unsigned int kernel_index = co * channel_input * kernel_height * kernel_width + ci * kernel_height * kernel_width + hk * kernel_width + wk;

                            // Perform element-wise multiplication and accumulation
                            output_value += src[src_index] * kernel[kernel_index];
                        }
                    }
                }

                // Store the result in the destination tensor
                dst[dst_index] = output_value;
            }
        }
    }
}
void mlu_conv(float *dst, float *src, float *kernel, unsigned int channel_input, unsigned int height, unsigned int width, unsigned int kernel_height, unsigned int kernel_width, unsigned int stride_width, unsigned int stride_height, unsigned int channel_output);

// void print_matrix()
int main() {
    // 定义输入数据和卷积核

    // The byte size of <kernel> must be 64-byte aligned
    // Ci * sizeof(typeof<kernel>) must be 64-byte aligned;-> C_i * 4 must be 64byte align -> C_i must be 16 align
    //  the co dimension of memory space of <kernel> must be 64 aligned;

    // 16 8 8 4 4 1 1 16 kernel = 1
    // 2/3/4/8 8 8 4 4 1 1 64 kernel = 1
    // 16(15) 8 8 4 4 1 1 64 kernel = i


    unsigned int channel_input = 48; // 输入通道数
    unsigned int height = 8;         // 输入高度
    unsigned int width = 8;          // 输入宽度
    unsigned int kernel_height = 3;  // 卷积核高度
    unsigned int kernel_width = 3;   // 卷积核宽度
    unsigned int stride_width = 1;   // 横向步长
    unsigned int stride_height = 1;  // 纵向步长
    unsigned int channel_output = 32; // 输出通道数

    // 创建输入数据和卷积核数组（这里只是示例数据）
    float *src = (float*)malloc(sizeof(float) * height * width * channel_input);
    for (int i=0; i < height * width * channel_input; i++)
        src[i] = 1;

    float *kernel = (float*)malloc(sizeof(float) * channel_output * kernel_height * kernel_width * channel_input);
    for (int i=0; i < channel_output * kernel_height * kernel_width * channel_input; i++)
        kernel[i] = i;

    unsigned int output_height = (height - kernel_height) / stride_height + 1;
    unsigned int output_width = (width - kernel_width) / stride_width + 1;

    float *dst = (float*)malloc(sizeof(float) * channel_output * output_height * output_width);

    conv((float *)dst, (float *)src, (float *)kernel, channel_input, height, width, kernel_height, kernel_width, stride_width, stride_height, channel_output);

    std::cout << "Convolution Result:" << std::endl;
    for (unsigned int output_channel = 0; output_channel < channel_output; ++output_channel) {
        std::cout << "Channel " << output_channel << ":" << std::endl;
        for (unsigned int output_y = 0; output_y < output_height; ++output_y) {
            for (unsigned int output_x = 0; output_x < output_width; ++output_x) {
                std::cout << dst[output_channel * (output_height * output_width) + output_y * output_width + output_x] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "Total_size = " << channel_output * output_height * output_width << std::endl;
    
    float *dst0 = (float*)malloc(sizeof(float) * channel_output * output_height * output_width);
    memset(dst0, 0, sizeof(dst0));
    mlu_conv((float *)dst0, (float *)src, (float *)kernel, channel_input, height, width, kernel_height, kernel_width, stride_width, stride_height, channel_output);

    std::cout << "MLU Convolution Result:" << std::endl;
    assert(dst0[0] == dst[0]);
    for (unsigned int output_channel = 0; output_channel < channel_output; ++output_channel) {
        std::cout << "Channel " << output_channel << ":" << std::endl;
        for (unsigned int output_y = 0; output_y < output_height; ++output_y) {
            for (unsigned int output_x = 0; output_x < output_width; ++output_x) {
                std::cout << dst0[output_y * (output_width * channel_output) + output_x * channel_output + output_channel] << " ";
                assert(dst0[output_y * (output_width * channel_output) + output_x * channel_output + output_channel] - dst[output_channel * (output_height * output_width) + output_y * output_width + output_x] <= 10e-6);
            }
            std::cout << std::endl;
        }
    }
    return 0;
}