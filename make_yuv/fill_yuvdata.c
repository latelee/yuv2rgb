#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ITU-R BT.601给出的彩色信号的YCbCr值，8种颜色(视频解密第3节，SDTV)
// white yellow cyan green magenta red blue black
unsigned int g_test_yuv[] ={
    0xb48080, 0xa22c8e, 0x839c2c, 0x70483a, 0x54b8c6,
    0x4164d4, 0x23d472, 0x108080};

#define YUV_NUM (sizeof(g_test_yuv)/sizeof(g_test_yuv[0]))
/// 填充YUV数据，格式：UYVY
/// 横条
void fill_yuv_uyvy_1(unsigned char* buffer, int width, int height)
{
    unsigned char *src = buffer;
    int i, j;

    int slice = height / YUV_NUM;
    for (i = 0; i < height; i++) // h
    {
        int index = i / slice;
        unsigned char y = (g_test_yuv[index] & 0xff0000 ) >> 16;
        unsigned char u = (g_test_yuv[index] & 0x00ff00) >> 8;
        unsigned char v = (g_test_yuv[index] & 0x0000ff);
        for (j=0; j<width*2; j+=4)    // w
        {
            src[i*width*2+j+0] = u; // U
            src[i*width*2+j+1] = y; // Y0
            src[i*width*2+j+2] = v; // V
            src[i*width*2+j+3] = y; // Y1
        }
    }
}

/// 填充YUV数据，格式：UYVY 
/// 竖条
void fill_yuv_uyvy_2(unsigned char* buffer, int width, int height)
{
    unsigned char *src = buffer;
    int i, j, k;

    int slice = width / YUV_NUM;
    for (i = 0; i < (int)YUV_NUM; i++) // 彩条个数
    {
        int index = i;
        unsigned char y = (g_test_yuv[index] & 0xff0000 ) >> 16;
        unsigned char u = (g_test_yuv[index] & 0x00ff00) >> 8;
        unsigned char v = (g_test_yuv[index] & 0x0000ff);
        // 竖条
        for (j = 0; j < height; j++) // h
        {
            // 竖条的一行
            for (k = 0; k < slice*2; k += 4)
            {
                src[index*slice*2 + j*width*2+k+0] = u; // U
                src[index*slice*2 + j*width*2+k+1] = y; // Y0
                src[index*slice*2 + j*width*2+k+2] = v; // V
                src[index*slice*2 + j*width*2+k+3] = y; // Y1
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// yuv422
// 横条
void fill_yuv_yuv422_1(unsigned char* buffer, int width, int height)
{
    unsigned char *src = buffer;
    int i, j;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;

    p_y = src;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;

    int slice = height / YUV_NUM;
    //printf("slice: %d\n", slice);
    for (i = 0; i < height; i++) // h
    {
        int index = i / slice;
        unsigned char y = (g_test_yuv[index] & 0xff0000 ) >> 16;
        unsigned char u = (g_test_yuv[index] & 0x00ff00) >> 8;
        unsigned char v = (g_test_yuv[index] & 0x0000ff);

        for (j=0; j<width*2; j+=4)
        {
            p_y[0] = y;
            p_y[1] = y;
            p_u[0] = u;
            p_v[0] = v;

            p_y += 2;
            p_u += 1;
            p_v += 1;
        }
    }
}

// yuv422
// 竖条
void fill_yuv_yuv422_2(unsigned char* buffer, int width, int height)
{
    unsigned char *src = buffer;
    int i, j, k;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;

    p_y = src;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;

    // 计出每种颜色有多少像素
    int slice = width / YUV_NUM;

    for (i = 0; i < height; i++)
    {
        // 按竖条填充每一行的像素
        for (j = 0; j < (int)YUV_NUM; j++)
        {
            int index = j;
            unsigned char y = (g_test_yuv[index] & 0xff0000 ) >> 16;
            unsigned char u = (g_test_yuv[index] & 0x00ff00) >> 8;
            unsigned char v = (g_test_yuv[index] & 0x0000ff);
            for (k = 0; k < slice*2; k += 4)    // 一种颜色
            {
                p_y[0] = y;
                p_y[1] = y;
                p_u[0] = u;
                p_v[0] = v;

                p_y += 2;
                p_u += 1;
                p_v += 1;
            }
        }
    }
}

// yuv420
// 横条
void fill_yuv_yuv420_1(unsigned char* buffer, int width, int height)
{
    unsigned char *src = buffer;
    int i, j;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;

    p_y = src;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;

    int slice = height / YUV_NUM;
    //printf("slice: %d\n", slice);
    for (i = 0; i < height; i++) // h
    {
        int index = i / slice;
        unsigned char y = (g_test_yuv[index] & 0xff0000 ) >> 16;
        unsigned char u = (g_test_yuv[index] & 0x00ff00) >> 8;
        unsigned char v = (g_test_yuv[index] & 0x0000ff);

        for (j=0; j<width; j+=4)
        {
            p_y[0] = y;
            p_y[1] = y;
            p_y[2] = y;
            p_y[3] = y;
            p_u[0] = u;
            p_v[0] = v;

            p_y += 4;
            p_u += 1;
            p_v += 1;
        }
    }
}

// yuv420
// 竖条
// 还有问题，继续找
void fill_yuv_yuv420_2(unsigned char* buffer, int width, int height)
{
    unsigned char *src = buffer;
    int i, j, k;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;

    p_y = src;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;

    // 计出每种颜色有多少像素
    int slice = width / YUV_NUM;

    printf("slice: %d\n", slice);
    for (i = 0; i < height; i++)
    {
        // 按竖条填充每一行的像素
        for (j = 0; j < (int)YUV_NUM; j++)
        {
            int index = j;
            unsigned char y = (g_test_yuv[index] & 0xff0000 ) >> 16;
            unsigned char u = (g_test_yuv[index] & 0x00ff00) >> 8;
            unsigned char v = (g_test_yuv[index] & 0x0000ff);
            for (k = 0; k < slice; k += 4)    // 一种颜色
            {
            p_y[0] = y;
            p_y[1] = y;
            p_y[2] = y;
            p_y[3] = y;
            p_u[0] = u;
            p_v[0] = v;

            p_y += 4;
            p_u += 1;
            p_v += 1;
            }
        }
    }
}
