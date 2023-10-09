#include <stdio.h>
#include <stdlib.h>
#include "maxpool2d_api.h"

#ifndef MAXPOOL_TEST
#define MAXPOOL_TEST

#define NUMBER 1
#define CHANNEL 16
#define HEIGHT 8
#define WIDTH 8
#define KERNEL_HEIGHT 2
#define KERNEL_WIDTH 2
#define STRIDE_WIDTH 2
#define STRIDE_HEIGHT 2
#endif

int main() {
    int test_len = NUMBER * CHANNEL * HEIGHT * WIDTH;
    float* host_dst = (float*)malloc(test_len * sizeof(float));
    float* host_src = (float*)malloc(test_len * sizeof(float));

    //host_src[0] = 2.0;host_src[1]= 3.0;host_src[2]=0.0;host_src[3]=1.0;
    for(int i = 0; i < test_len; i++) {
        host_src[i] = i+1;
    }
    maxpool2d_mlu(host_dst, host_src, NUMBER, CHANNEL, HEIGHT, WIDTH, KERNEL_HEIGHT, KERNEL_WIDTH, STRIDE_WIDTH, STRIDE_HEIGHT);

    int output_h = (HEIGHT - KERNEL_HEIGHT) / STRIDE_HEIGHT + 1;
    int output_w = (WIDTH  - KERNEL_WIDTH) / STRIDE_WIDTH + 1;
    for (int i = 0; i < NUMBER * CHANNEL * output_h * output_w; i++) {
        printf("%f ", host_dst[i]);
    }
}