#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bmp_utils.h"

int analyse_bmp_file(const char* bmp_file)
{
    FILE* fp;
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfo;
    RGBQUAD bmpQuad;

    int rgb_size1 = 0;
    int rgb_size2 = 0;
    int rgb_size3 = 0;
    int width = 0;
    int height = 0;
    int padding = 0;
    int stride_byte = 0;
    int color_num = 0;
    int paltette_len = 0;
    char* palette = NULL;
    int color_rgb = 0;
    int i = 0;

    fp = fopen(bmp_file, "rb");
    if (fp == NULL)
    {
        printf("open file %s failed.\n", bmp_file);
        return -1;
    }

    fread(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), fp);
    fread(&bmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);

    if (bmpHeader.bfType != (('M' << 8) | 'B'))
    {
        printf("Sorry, not bmp picture.\n");
        return -1;
    }

    width = bmpInfo.biWidth;
    height = (int)fabs((double)bmpInfo.biHeight);

    switch(bmpInfo.biBitCount) 
    {
    case 1:
        color_num = 2;
        break;
    case 4:
        color_num = 16;
        break;
    case 8:
        color_num = 256;
        break;
    case 24:
    default:
        color_num = 0;
        break;
    }

    stride_byte = ALIGN(width*bmpInfo.biBitCount/8, 4);
    padding = stride_byte - width*bmpInfo.biBitCount/8;
    paltette_len = color_num * sizeof(RGBQUAD);

    rgb_size1 = bmpHeader.bfSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - paltette_len;
    rgb_size2 = stride_byte*height;
    rgb_size3 = width*height*bmpInfo.biBitCount/8;
    // ��ӡ�ṹ����ÿ����Ա
    printf("file name: %s\n", bmp_file);
    printf("file type: %c%c %x\n", (bmpHeader.bfType)>>8, (bmpHeader.bfType)&0xff, bmpHeader.bfType);
    printf("file size: %d(B) = %0.2f(KB) = %0.2f(MB)\n", bmpHeader.bfSize, (float)bmpHeader.bfSize/1024.00, (float)bmpHeader.bfSize/1024.00/1024.00);
    printf("offset of image data: %d\n", bmpHeader.bfOffBits);
    //////////////////////////////////

    printf("biSize: %d\n", bmpInfo.biSize);
    printf("width: %d\n", bmpInfo.biWidth);
    printf("height: %d\n", bmpInfo.biHeight);
    printf("Plane: %d\n", bmpInfo.biPlanes);
    printf("BitCount: %d\n", bmpInfo.biBitCount);
    printf("biCompression: %d\n", bmpInfo.biCompression);
    printf("biSizeImage: %d\n", bmpInfo.biSizeImage);
    printf("XPelsPerMeter: %d\n", bmpInfo.biXPelsPerMeter);
    printf("YPelsPerMeter: %d\n", bmpInfo.biYPelsPerMeter);
    printf("biClrUsed: %d\n", bmpInfo.biClrUsed);
    printf("biClrImportant: %d\n", bmpInfo.biClrImportant);

    printf("width*3: %d stride byte: %d padding: %d\n", width*3, stride_byte, padding);

    printf("rgb buffer size: %d %d %d\n", rgb_size1,rgb_size2, rgb_size3);

    if (color_num != 0)
    {
        palette = (char *)malloc(paltette_len * sizeof(char));
        fread(palette, paltette_len, 1, fp);
        printf("palette:\n");
        // ��ӡ��ɫ����Ϣ
        for (i = 0; i < color_num; i++)
        {
            memcpy(&bmpQuad, palette + i * sizeof(RGBQUAD), sizeof(RGBQUAD));
            color_rgb = bmpQuad.rgbBlue | (bmpQuad.rgbGreen<<8) | (bmpQuad.rgbRed<<16);
            printf("%03d: %06x \t r: %02x g: %02x b: %02x\n", i, color_rgb, 
                bmpQuad.rgbRed, bmpQuad.rgbGreen, bmpQuad.rgbBlue);

        }
        
    }
    fclose(fp);

    return 0;
}

int read_bmp_file(const char* bmp_file, unsigned char** rgb_buffer,
                  int* rgb_size, int* width, int* height)
{
    FILE* fp;
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfo;
    int tmp_width = 0;
    int tmp_height = 0;
    int tmp_rgb_size = 0;
    int stride_byte = 0; // ÿ��ռ���ֽ���(4�ֽڶ���)
    int width_byte = 0;  // ÿ��������Ч�ֽ���
    int padding = 0;    // ��Ҫ������ֽ���
    unsigned char* tmp_p = 0;
    int color_num = 0;
    int palette_len = 0;
    int i = 0;

    fp = fopen(bmp_file, "rb");
    if (fp == NULL)
    {
        printf("open file %s failed.\n", bmp_file);
        return -1;
    }

    fread(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), fp);
    fread(&bmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);

    if (bmpHeader.bfType != (('M' << 8) | 'B'))
    {
        printf("Sorry, not bmp picture.\n");
        return -1;
    }
    tmp_width = bmpInfo.biWidth;
    tmp_height = (int)fabs((double)bmpInfo.biHeight);   // Ԥ����Ϊ���������

    // ����RGB���ݴ�С
    tmp_rgb_size = tmp_width * tmp_height * bmpInfo.biBitCount/8;

    width_byte = tmp_width * bmpInfo.biBitCount/8;
    /**
     * ÿ��ռ���ֽ���������ʽ�����ͬ
     * stride_byte = (width * bmpInfo.biBitCount/8+3)/4*4;
     */
    stride_byte = ALIGN(width_byte, 4); // 4�ֽڶ���

    /**
     * �����ֽڣ�����ʽ�����ͬ
     * padding = (4 - width * 3 % 4) % 4;
     * ʵ����δʹ��
     */
    padding = stride_byte - width_byte;

    // �жϵ�ɫ��
    switch(bmpInfo.biBitCount) 
    {
    case 1:
        color_num = 2;
        break;
    case 4:
        color_num = 16;
        break;
    case 8:
        color_num = 256;
        break;
    case 24:
    default:
        color_num = 0;
        break;
    }
    // ��ɫ�峤��
    palette_len = color_num * sizeof (RGBQUAD);

    // ����ƫ������ʵ��ƫ�����Ƚϣ��粻�ȣ���ɫ������
    if (bmpHeader.bfOffBits != sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + palette_len)
    {
        return -1;
    }

    printf("debug--:\nfile size: %d rgb size: %d %d stride byte: %d padding: %d BitCount: %d\n", 
        bmpHeader.bfSize, tmp_rgb_size, stride_byte*tmp_height, stride_byte, padding, bmpInfo.biBitCount);

    if (color_num != 0)
    {
        // ����ͼ�����ݴ�
        fseek(fp, palette_len, SEEK_CUR);
    }

    // ������ʵ��ڴ�
    *rgb_buffer = (unsigned char *)malloc(sizeof(char) * tmp_rgb_size);
    if (*rgb_buffer == NULL)
    {
        return -1;
    }
    // ����ȡ�����ݵ��Ŵ�ŵ�������(��BMPͼ���һ�����ݷŵ����������һ�У��ȵ�)��
    // ����ͼ����������ģ�����ͼ���ǵ�����
    tmp_p = *rgb_buffer + tmp_rgb_size;
    for (i = 0; i < tmp_height; i++)
    {
        tmp_p -= width_byte;
        fread(tmp_p, 1, width_byte, fp);
        fseek(fp, padding, SEEK_CUR);
    }

#if 0
    // ˳����ļ���������ͼ���ǵ�����
    unsigned char* tmp_p = *rgb_buffer;
    size_t readByte = 0;
    for (int i = 0; i < tmp_height; i++)
    {
        readByte += fread(tmp_p, 1, width_byte, fp);
        fseek(fp, padding, SEEK_CUR);
        tmp_p += width_byte;
    }
#endif

    *width = tmp_width;
    *height = tmp_height;
    *rgb_size = tmp_rgb_size;

    fclose(fp);

    return 0;
}

int read_bmp_file_ex(const char* bmp_file,
                     unsigned char** rgb_buffer, int* rgb_size,
                     unsigned char** palette_buf, int* palette_len,
                     int* width, int* height)
{
    FILE* fp;
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfo;
    unsigned char* tmp_rgb_buffer = NULL;
    int tmp_rgb_size = 0;
    unsigned char* tmp_palette_buf = NULL;
    int tmp_palette_len = 0;
    int tmp_width = 0;
    int tmp_height = 0;
    int stride_byte = 0; // ÿ��ռ���ֽ���(4�ֽڶ���)
    int width_byte = 0;  // ÿ��������Ч�ֽ���
    int padding = 0;    // ��Ҫ������ֽ���
    int color_num = 0;
    int i = 0;
    unsigned char* tmp_p = 0;

    fp = fopen(bmp_file, "rb");
    if (fp == NULL)
    {
        printf("open file %s failed.\n", bmp_file);
        return -1;
    }

    fread(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), fp);
    fread(&bmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);

    if (bmpHeader.bfType != (('M' << 8) | 'B'))
    {
        printf("Sorry, not bmp picture.\n");
        return -1;
    }
    tmp_width = bmpInfo.biWidth;
    tmp_height = (int)fabs((double)bmpInfo.biHeight);   // Ԥ����Ϊ���������

    // ����RGB���ݴ�С
    tmp_rgb_size = tmp_width * tmp_height * bmpInfo.biBitCount/8;

    width_byte = tmp_width * bmpInfo.biBitCount/8;
    /**
     * ÿ��ռ���ֽ���������ʽ�����ͬ
     * stride_byte = (width * bmpInfo.biBitCount/8+3)/4*4;
     */
    stride_byte = ALIGN(width_byte, 4); // 4�ֽڶ���

    /**
     * �����ֽڣ�����ʽ�����ͬ
     * padding = (4 - width * 3 % 4) % 4;
     * ʵ����δʹ��
     */
    padding = stride_byte - width_byte;

    // �жϵ�ɫ��
    switch(bmpInfo.biBitCount) 
    {
    case 1:
        color_num = 2;
        break;
    case 4:
        color_num = 16;
        break;
    case 8:
        color_num = 256;
        break;
    case 24:
    default:
        color_num = 0;
        break;
    }
    // ��ɫ���С
    tmp_palette_len = color_num * sizeof (RGBQUAD);

    // ����ƫ������ʵ��ƫ�����Ƚϣ��粻�ȣ���ɫ������
    if (bmpHeader.bfOffBits != sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tmp_palette_len)
    {
        return -1;
    }

    printf("debug--:\nfile size: %d rgb size: %d %d stride byte: %d padding: %d BitCount: %d palette len: %d\n", 
        bmpHeader.bfSize, tmp_rgb_size, stride_byte*tmp_height, stride_byte, padding, bmpInfo.biBitCount, tmp_palette_len);

    // ���е�ɫ�壬���ȡ��ɫ��
    if (tmp_palette_len != 0)
    {
        tmp_palette_buf = (unsigned char *)malloc(sizeof(char) * tmp_palette_len);
        if (tmp_palette_buf == NULL)
        {
            return -1;
        }
        fread(tmp_palette_buf, 1, tmp_palette_len, fp);
    }

    // ������ʵ��ڴ�
    tmp_rgb_buffer = (unsigned char *)malloc(sizeof(char) * tmp_rgb_size);
    if (tmp_rgb_buffer == NULL)
    {
        return -1;
    }
    // ����ȡ�����ݵ��Ŵ�ŵ�������(��BMPͼ���һ�����ݷŵ����������һ�У��ȵ�)��
    // ����ͼ����������ģ�����ͼ���ǵ�����
    tmp_p = tmp_rgb_buffer + tmp_rgb_size;
    for (i = 0; i < tmp_height; i++)
    {
        tmp_p -= width_byte;
        fread(tmp_p, 1, width_byte, fp);
        fseek(fp, padding, SEEK_CUR);
    }

    *rgb_buffer = tmp_rgb_buffer;
    *rgb_size = tmp_rgb_size;
    if (tmp_palette_buf)
    {
        *palette_buf = tmp_palette_buf;
    }
    else
    {
        *palette_buf = NULL;
    }
    if (tmp_palette_len)
    {
        *palette_len = tmp_palette_len;
    }
    else
    {
        *palette_len = 0;
    }
    
    *width = tmp_width;
    *height = tmp_height;

    fclose(fp);

    return 0;
}

int write_bmp_file(const char* bmp_file, unsigned char* rgb_buffer, int width, int height)
{
#define BPP 24  // Ŀǰֻ����24ɫλͼ

    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfo;
    FILE* fp = NULL;
    int offset = 0;
    int stride_byte = 0;    // ÿ��ռ���ֽ���(4�ֽڶ���)
    int width_byte = 0;     // ÿ��������Ч�ֽ���
    int rgb_size = 0;
    int padding = 0;
    unsigned char* tmp_buf = NULL;
    int i = 0;

    fp = fopen(bmp_file, "wb");
    if (fp == NULL)
    {
        printf("open %s failed\n", bmp_file);
        return -1;
    }

    offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);  //54�ֽ�
    printf("bmp header size: %d = %ld + %ld\n", offset, sizeof(BITMAPINFOHEADER), sizeof(BITMAPFILEHEADER));
    // 4�ֽڶ��� ((width * 24 + 31) / 32) * 4
    // ���Ѿ����룬��stride_byte��ʵ�ʿ�һ�£��粻����stride_byte��ȿ��һЩ
    // stride_byte = ((width * 24 + 31) >> 5) << 2;
    stride_byte = ALIGN(width*BPP/8, 4);
    width_byte = width*BPP/8;
    rgb_size = stride_byte * height;  // �ѿ��Ƕ���

    bmpHeader.bfType = ('M' << 8) | 'B';
    bmpHeader.bfSize = offset + rgb_size;    // BMP�ļ��ܴ�С
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfOffBits = offset;

    bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.biWidth  = width;
    bmpInfo.biHeight = height;
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = BPP;
    bmpInfo.biCompression = 0;
    bmpInfo.biSizeImage = rgb_size;
    bmpInfo.biXPelsPerMeter = 0;
    bmpInfo.biYPelsPerMeter = 0;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;

    // ��Ҫ����ֽڣ�BMPҪ��ÿһ�����ݱ���4�ֽڶ��룬������0����
    //padding = (4 - width * 3 % 4) % 4;
    // ʵ��δʹ�õ�
    padding = stride_byte - width_byte;

    printf("debug--:\nwidth: %d height: %d padding: %d rgb_size: %d, stride_byte: %d\n",
                width, height, padding, rgb_size, stride_byte);

    // Ϊ�������ͼƬ��������ʱ������
    tmp_buf = (unsigned char *)malloc(sizeof(char) * rgb_size);
    if (tmp_buf == NULL)
    {
        return -1;
    }
    memset(tmp_buf, '\0', sizeof(char) * rgb_size);
    // ���ſ�����������
    for (i = 0; i < height; i++)
    {
        // ÿһ�е�ʵ������Ϊwidth * 3(R��G��B)
        memcpy(tmp_buf + i * stride_byte, rgb_buffer + (height - i - 1) * width_byte, width_byte);
    }

    fwrite(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), fp);
    fwrite(&bmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);
    fwrite(tmp_buf, 1, rgb_size, fp);

    free(tmp_buf);
    fclose(fp);

    return 0;
}

int write_bmp_file_ex(const char* bmp_file,
                      unsigned char* rgb_buffer,
                      unsigned char* palette_buf, int palette_len,
                      int width, int height)
{
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfo;
    FILE* fp = NULL;
    int offset = 0;
    int stride_byte = 0;    // ÿ��ռ���ֽ���(4�ֽڶ���)
    int width_byte = 0;     // ÿ��������Ч�ֽ���
    int rgb_size = 0;
    int padding = 0;
    unsigned char* tmp_buf = NULL;
    int color_bit = 0;      // ��ɫλ����2��16��256��24����
    int i = 0;

    if ((bmp_file==NULL) || (rgb_buffer == NULL))
        return -1;
    fp = fopen(bmp_file, "wb");
    if (fp == NULL)
    {
        printf("open %s failed\n", bmp_file);
        return -1;
    }

    offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + palette_len;  //54�ֽ�
    color_bit = palette_len / sizeof(RGBQUAD);
    switch(color_bit)
    {
    case 2:
        color_bit = 1;
        break;
    case 16:
        color_bit = 4;
        break;
    case 256:
        color_bit = 8;
        break;
    case 0:
    default:
        color_bit = 24;
        break;
    }
    width_byte = width*color_bit/8;
    // 4�ֽڶ��� ((width * 24 + 31) / 32) * 4
    // ���Ѿ����룬��rowStride��ʵ�ʿ�һ�£��粻����rowStride��ȿ��һЩ
    // stride_byte = ((width * 24 + 31) >> 5) << 2;
    stride_byte = ALIGN(width_byte, 4);
    rgb_size = stride_byte * height;  // �ѿ��Ƕ���

    bmpHeader.bfType = ('M' << 8) | 'B';
    bmpHeader.bfSize = offset + rgb_size;    // BMP�ļ��ܴ�С
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfOffBits = offset;

    bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.biWidth  = width;
    bmpInfo.biHeight = height;
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = color_bit;
    bmpInfo.biCompression = 0;
    bmpInfo.biSizeImage = rgb_size;
    bmpInfo.biXPelsPerMeter = 0;
    bmpInfo.biYPelsPerMeter = 0;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;

    // ��Ҫ����ֽڣ�BMPҪ��ÿһ�����ݱ���4�ֽڶ��룬������0����
    //padding = (4 - width * 3 % 4) % 4;
    // ʵ��δʹ�õ�
    padding = stride_byte - width_byte;

    printf("debug--:\nwidth: %d height: %d padding: %d rgb_size: %d, stride_byte: %d\n",
        width, height, padding, rgb_size, stride_byte);

    // Ϊ�������ͼƬ��������ʱ������
    tmp_buf = (unsigned char *)malloc(sizeof(char) * rgb_size);
    if (tmp_buf == NULL)
    {
        return -1;
    }
    memset(tmp_buf, '\0', sizeof(char) * rgb_size);
    // ���ſ�����������
    for (i = 0; i < height; i++)
    {
        // ÿһ�е�ʵ������Ϊwidth * 3(R��G��B)
        memcpy(tmp_buf + i * stride_byte, rgb_buffer + (height - i - 1) * width_byte, width_byte);
    }

    fwrite(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), fp);    // 1. file header
    fwrite(&bmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);      // 2. info header
    if ((palette_buf != NULL) && palette_len != 0)
    {
        fwrite(palette_buf, 1, palette_len, fp);                // 3. palette data
    }
    fwrite(tmp_buf, 1, rgb_size, fp);                       // 4. rgb data

    free(tmp_buf);
    fclose(fp);

    return 0;
}

// rgb --> bgr or
// bgr --> rgb
void swap_rgb(unsigned char* rgb_buffer, int len)
{
    int i = 0;
    unsigned char tmp;
    for (i = 0; i < len; i += 6)
    {
        tmp = rgb_buffer[i];
        rgb_buffer[i] = rgb_buffer[i + 2];
        //rgb_buffer[i + 1] = rgb_buffer[i + 1];
        rgb_buffer[i + 2] = tmp;

        tmp = rgb_buffer[i+3];
        rgb_buffer[i+3] = rgb_buffer[i + 5];
        //rgb_buffer[i + 4] = rgb_buffer[i + 4];
        rgb_buffer[i + 5] = tmp;
    }
}

void bmp_test()
{
    int width, height;
    unsigned char* rgb_buf;
    int rgb_size;
    unsigned char* pal_buf;
    int pal_size;
    analyse_bmp_file("lenna510.bmp");
    read_bmp_file_ex("lenna510.bmp", &rgb_buf, &rgb_size, &pal_buf, &pal_size, &width, &height);
    printf("width: %d height: %d, rgb size: %d pal size: %d\n",
        width, height, rgb_size, pal_size);

    write_bmp_file_ex("bb.bmp", rgb_buf, pal_buf, pal_size, width, height);
}
