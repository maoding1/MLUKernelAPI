#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <assert.h>

#define EPS 1e-6

void conv_with_padding(float *dst, float *src, float *kernel, unsigned int channel_input, unsigned int height, unsigned int width, unsigned int kernel_height, unsigned int kernel_width, unsigned int stride_width, unsigned int stride_height, unsigned int channel_output, unsigned int padding) {
    // Calculate output dimensions
    unsigned int output_height = (height - kernel_height + 2*padding) / stride_height + 1;
    unsigned int output_width = (width - kernel_width + 2*padding) / stride_width + 1;

    // Create a temporary padded source tensor
    unsigned int padded_height = height + 2 * padding;
    unsigned int padded_width = width + 2 * padding;
    unsigned int padded_size = channel_input * padded_height * padded_width;
    float *padded_src = new float[padded_size];

    // Initialize the padded source tensor with zeros
    for (unsigned int ci = 0; ci < channel_input; ci++) {
        for (unsigned int h = 0; h < padded_height; h++) {
            for (unsigned int w = 0; w < padded_width; w++) {
                unsigned int padded_index = h * padded_width * channel_input + w * channel_input + ci;
                if (h < padding || h >= (padding + height) || w < padding || w >= (padding + width)) {
                    // Set padding region to zero
                    padded_src[padded_index] = 0.0;
                } else {
                    // Copy the corresponding value from the original source tensor
                    // 2 1
                    unsigned int src_index = (h -padding) * width * channel_input + (w - padding) * channel_input + ci;
                    padded_src[padded_index] = src[src_index];
                    // printf("padd %d : %f\n", padded_index, padded_src[padded_index]);
                }
            }
        }
    }

    // Perform convolution
    for (unsigned int co = 0; co < channel_output; co++) {
        for (unsigned int ho = 0; ho < output_height; ho++) {
            for (unsigned int wo = 0; wo < output_width; wo++) {
                // Calculate output index
                unsigned int dst_index = ho * output_width * channel_output + wo * channel_output + co;

                // Initialize output value
                float output_value = 0;

                for (unsigned int ci = 0; ci < channel_input; ci++) {
                    for (unsigned int hk = 0; hk < kernel_height; hk++) {
                        for (unsigned int wk = 0; wk < kernel_width; wk++) {
                            // Calculate input and kernel indices
                            int input_h = ho * stride_height + hk;
                            int input_w = wo * stride_width + wk;

 
                            unsigned int padded_index = input_h * padded_width * channel_input + input_w * channel_input + ci;
                            unsigned int kernel_index = co * channel_input * kernel_height * kernel_width + hk * kernel_width * channel_input + wk * channel_input + ci;

                            // Perform element-wise multiplication and accumulation
                            output_value += padded_src[padded_index] * kernel[kernel_index];
                        }
                    }
                }

                // Store the result in the destination tensor
                dst[dst_index] = output_value;
            }
        }
    }

    // Free the memory allocated for the padded source tensor
    delete[] padded_src;
}
void conv2d_mlu(float *dst, float *src, float *kernel, unsigned int channel_input, unsigned int height, unsigned int width, unsigned int kernel_height, unsigned int kernel_width, unsigned int stride_width, unsigned int stride_height, unsigned int channel_output, unsigned int padding_h, unsigned int padding_w);

// void print_matrix()
int main() {
    // 定义输入数据和卷积核

    // The byte size of <kernel> must be 64-byte aligned
    // Ci * sizeof(typeof<kernel>) must be 64-byte aligned;-> C_i * 4 must be 64byte align -> C_i must be 16 align
    //  the co dimension of memory space of <kernel> must be 64 aligned;

    // 16 8 8 4 4 1 1 16 kernel = 1
    // 2/3/4/8 8 8 4 4 1 1 64 kernel = 1
    // 16(15) 8 8 4 4 1 1 64 kernel = i

// ok
    // 16 8 8 4 4 1 1 64 0 1 i
    // 16 8 8 4 4 1 1 64/32 0 i 1

    unsigned int channel_input = 16; // 输入通道数
    unsigned int height = 8;         // 输入高度
    unsigned int width = 8;          // 输入宽度
    unsigned int kernel_height = 3;  // 卷积核高度
    unsigned int kernel_width = 3;   // 卷积核宽度
    unsigned int stride_width = 1;   // 横向步长
    unsigned int stride_height = 1;  // 纵向步长
    unsigned int channel_output = 64; // 输出通道数
    unsigned int padding = 1;
    // 8*64

    // unsigned int channel_input = 2; // 输入通道数
    // unsigned int height = 4;         // 输入高度
    // unsigned int width = 4;          // 输入宽度
    // unsigned int kernel_height = 3;  // 卷积核高度
    // unsigned int kernel_width = 3;   // 卷积核宽度
    // unsigned int stride_width = 1;   // 横向步长
    // unsigned int stride_height = 1;  // 纵向步长
    // unsigned int channel_output = 2; // 输出通道数
    // unsigned int padding = 1;

    // 创建输入数据和卷积核数组（这里只是示例数据）
    float *src = (float*)malloc(sizeof(float) * height * width * channel_input);
    for (int i=0; i < height * width * channel_input; i++)
        src[i] = i;

    float *kernel = (float*)malloc(sizeof(float) * channel_output * kernel_height * kernel_width * channel_input);
    for (int i=0; i < channel_output * kernel_height * kernel_width * channel_input; i++)
        kernel[i] = 1;

    unsigned int output_height = (height - kernel_height) / stride_height + 1;
    unsigned int output_width = (width - kernel_width) / stride_width + 1;


    unsigned int in_h_pad = height + 2 * padding;
    unsigned int in_w_pad = width + 2 * padding;
    unsigned int out_h_pad = (in_h_pad - kernel_height) / stride_height + 1;
    unsigned int out_w_pad = (in_w_pad - kernel_width) / stride_width + 1;
    float *dst = (float*)malloc(sizeof(float) * channel_output * out_h_pad * out_w_pad);

    conv_with_padding((float *)dst, (float *)src, (float *)kernel, channel_input, height, width, kernel_height, kernel_width, stride_width, stride_height, channel_output, padding);
    
    std::cout << "Padding Convolution Result:" << std::endl;
    for (unsigned int output_channel = 0; output_channel < channel_output; ++output_channel) {
        std::cout << "Channel " << output_channel << ":" << std::endl;
        for (unsigned int output_y = 0; output_y < out_h_pad; ++output_y) {
            for (unsigned int output_x = 0; output_x < out_w_pad; ++output_x) {
                // std::cout << output_channel * (out_h_pad * out_w_pad) + output_y * out_w_pad + output_x << "\n";
                std::cout << dst[output_y * (out_w_pad * channel_output) + output_x * channel_output + output_channel] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "Total_size = " << channel_output * output_height * output_width << std::endl;
    
    float *dst_mlu = (float*)malloc(sizeof(float) * channel_output * out_h_pad * out_w_pad);

    // memset(dst, 0, sizeof(float) * channel_output * out_h_pad * out_w_pad);
    conv2d_mlu((float *)dst_mlu, (float *)src, (float *)kernel, channel_input, height, width, kernel_height, kernel_width, stride_width, stride_height, channel_output, padding, padding);

    std::cout << "MLU Convolution Result:" << std::endl;
    for (unsigned int output_channel = 0; output_channel < channel_output; ++output_channel) {
        std::cout << "Channel " << output_channel << ":" << std::endl;
        for (unsigned int output_y = 0; output_y < out_h_pad; ++output_y) {
            for (unsigned int output_x = 0; output_x < out_w_pad; ++output_x) {
                // std::cout << output_channel * (out_h_pad * out_w_pad) + output_y * out_w_pad + output_x << "\n";
                std::cout << dst_mlu[output_y * (out_w_pad * channel_output) + output_x * channel_output + output_channel] << " ";
            }
            std::cout << std::endl;
        }
    }

    for (unsigned int output_channel = 0; output_channel < channel_output; ++output_channel) {
        std::cout << "Channel " << output_channel << ":" << std::endl;
        for (unsigned int output_y = 0; output_y < out_h_pad; ++output_y) {
            for (unsigned int output_x = 0; output_x < out_w_pad; ++output_x) {
                // std::cout << output_channel * (out_h_pad * out_w_pad) + output_y * out_w_pad + output_x << "\n";
                unsigned int index = output_y * (out_w_pad * channel_output) + output_x * channel_output + output_channel;
                printf("%f %f\n", dst[index], dst_mlu[index]) ;
                assert(abs(dst[index] - dst_mlu[index] < EPS));
            }
            std::cout << std::endl;
        }
    }
    // for (int i = 0; i < channel_input * height * width; i ++)
    // {
    //     std::cout << src[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < channel_output * out_h_pad * out_w_pad; i ++)
    // {
    //     std::cout<< dst[i] << " ";
    // }
    std::cout << "Total_size = " << channel_output * output_height * output_width << std::endl;
    return 0;
}