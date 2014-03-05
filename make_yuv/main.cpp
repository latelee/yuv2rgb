#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fill_yuvdata.h"
#include "file_utils.h"

// 8的整数倍，否则颜色可能错位
#define WIDTH 720
#define HEGHT 480

int main(void)
{
    printf("test of writing yuv...\n");
    unsigned char* buffer = (unsigned char*)malloc(WIDTH*HEGHT*sizeof(char)*3/2);
    //fill_yuv_uyvy_1(WIDTH, HEGHT);
    //fill_yuv_uyvy_2(WIDTH, HEGHT);
    fill_yuv_yuv420_1(buffer, WIDTH, HEGHT);
    write_file("yuv.yuv", (char*)buffer, WIDTH*HEGHT*3/2);
    return 0;
}