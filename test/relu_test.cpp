#include <stdio.h>
#include <stdlib.h>
#include "relu_api.h"

int main() {
    int test_len = 5;
    float* host_dst = (float*)malloc(test_len * sizeof(float));
    float* host_src = (float*)malloc(test_len * sizeof(float));

    host_src[0] = -2.0;host_src[1]= -1.0;host_src[2]=0.0;host_src[3]=1.0;host_src[4]=2.0;
    relu_mlu(host_dst, host_src, test_len);

    for (int i = 0; i < test_len; i++) {
        printf("%f ", host_dst[i]);
    }
}



