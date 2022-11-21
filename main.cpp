#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include "yuv2rgb.h"
#include "rgb2yuv.h"
#include "bmp_utils.h"

// yuv422p
int save_bmp422(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    int frameSize = 0; 
    int picSize = 0;
    int ret = 0;
    unsigned char* framePtr = NULL;
    unsigned char* rgbPtr = NULL;
    long rgbSize = 0;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422p: w * h * 2
    // yuv420p: w * h * 3 / 2
    picSize   = frameSize * 2;;

    framePtr = (unsigned char *) malloc(sizeof(unsigned char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (unsigned char *) malloc(sizeof(unsigned char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);
	if(ret <= 0) {
		printf("[%s:%d] failed to read picture file(%s)\n", __FILE__, __LINE__, yuvfile);
		return -1;
	}

    //yuv422p_to_rgb24(framePtr, rgbPtr, width, height);
	yuv_to_rgb24(YUV422P, framePtr, rgbPtr, width, height);

    // rgb --> bgr
    swap_rgb(rgbPtr, rgbSize);

    // save file
    write_bmp_file(bmpfile, rgbPtr, width, height);

    fclose(fp1);


    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

// yuv422sp
int save_bmp422sp(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    int frameSize = 0; 
    int picSize = 0;
    int ret = 0;
    unsigned char* framePtr = NULL;
    unsigned char* rgbPtr = NULL;
    long rgbSize = 0;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422sp: w * h * 2
    // yuv420p: w * h * 3 / 2
    picSize   = frameSize * 2;;

    framePtr = (unsigned char *) malloc(sizeof(unsigned char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (unsigned char *) malloc(sizeof(unsigned char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);
	if(ret <= 0) {
		printf("[%s:%d] failed to read picture file(%s)\n", __FILE__, __LINE__, yuvfile);
		return -1;
	}

    //yuv422sp_to_rgb24(framePtr, rgbPtr, width, height);
	yuv_to_rgb24(YUV422SP, framePtr, rgbPtr, width, height);

    // rgb --> bgr
    swap_rgb(rgbPtr, rgbSize);

    // save file
    write_bmp_file(bmpfile, rgbPtr, width, height);

    fclose(fp1);


    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

// yuv420
int save_bmp420(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    int frameSize = 0; 
    int picSize = 0;
    int ret = 0;
    unsigned char* framePtr = NULL;
    unsigned char* rgbPtr = NULL;
    long rgbSize = 0;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422p: w * h * 2
    // yuv420p: w * h * 3 / 2
    picSize   = frameSize * 3 / 2;

    framePtr = (unsigned char *) malloc(sizeof(char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (unsigned char *) malloc(sizeof(unsigned char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);
	if(ret <= 0) {
		printf("[%s:%d] failed to read picture file(%s)\n", __FILE__, __LINE__, yuvfile);
		return -1;
	}

    //yuv420p_to_rgb24(framePtr, rgbPtr, width, height);
	//yuv_to_rgb24(YUV420P, framePtr, rgbPtr, width, height);
	yuv420_to_rgb24_2(framePtr, rgbPtr, width, height);

    // rgb --> bgr
    swap_rgb(rgbPtr, rgbSize);

    // save file
    write_bmp_file(bmpfile, rgbPtr, width, height);

    fclose(fp1);

    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

// type - 0: yuv420, 1: yuv422
// 从多帧YUV文件抽取第一帧
void split_file(const char* src, const char* dst, int width, int height, int type)
{
    FILE* fp1;
    FILE* fp2;
    int file_size;

    fp1 = fopen(src, "rb");
    if (fp1 == NULL)
    {
        printf("open %s failed!\n", src);
        return;
    }
    fp2 = fopen(dst, "wb");
    if (fp2 == NULL)
    {
        printf("open %s failed!\n", dst);
        return;
    }

    if (type == 0)
    {
        file_size = width * height * 3 / 2;
    }
    else if (type == 1)
    {
        file_size = width * height * 2;
    }
    else
    {
        return;
    }

    unsigned char* buffer1 = (unsigned char*)malloc(file_size);

    if ( NULL == buffer1)
    {
        printf("malloc failed!\n");
        return;
    }

    int ret = 0;
    ret = (int)fread(buffer1, 1, file_size, fp1);
    if (ret != file_size)
    {
        printf("fread failed! real: %d ret: %d\n", file_size, ret);
        return;
    }

    ret = (int)fwrite(buffer1, 1, file_size, fp2);
    if (ret != file_size)
    {
        printf("fwrite failed! real: %d ret: %d\n", file_size, ret);
        return;
    }

    fclose(fp1);
    fclose(fp2);
}

void ConvertImage(const char* src, const char* dst, int width, int height)
{
    FILE* fp1;
    FILE* fp2;

    fp1 = fopen(src, "rb");
    if (fp1 == NULL)
    {
        printf("open %s failed!\n", src);
        return;
    }
    fp2 = fopen(dst, "wb");
    if (fp2 == NULL)
    {
        printf("open %s failed!\n", dst);
        return;
    }

    int file_size = width * height * 2;

    unsigned char* buffer1 = (unsigned char*)malloc(file_size);
    unsigned char* buffer2 = (unsigned char*)malloc(file_size);

    if ( NULL == buffer1 || NULL == buffer2)
    {
        printf("malloc failed!\n");
        return;
    }

    int ret = 0;
    ret = (int)fread(buffer1, 1, file_size, fp1);
    if (ret != file_size)
    {
        printf("fread failed! real: %d ret: %d\n", file_size, ret);
        return;
    }

    yuv422sp_to_yuv422p(buffer1, buffer2, width, height);

    //yuv420sp_to_yuv420p(buffer1, buffer2, width, height);

    ret = (int)fwrite(buffer2, 1, file_size, fp2);
    if (ret != file_size)
    {
        printf("fwrite failed! real: %d ret: %d\n", file_size, ret);
        return;
    }

    fclose(fp1);
    fclose(fp2);
}

void save_yuv420p(const char* src, const char* dst, int w, int h)
{
	printf("[%s:%d] src = %s; dst = %s; w = %d; h = %d;\n", __FILE__, __LINE__, src, dst, w, h);
    FILE* fp1;
    FILE* fp2;

    fp1 = fopen(src, "rb");
    if (fp1 == NULL)
    {
        printf("open %s failed!\n", src);
        return;
    }
    fp2 = fopen(dst, "wb");
    if (fp2 == NULL)
    {
        printf("open %s failed!\n", dst);
		fclose(fp1);
        return;
    }

    int yuv_file_size = w * h * 3 / 2;
    int bmp_file_size = w * h * 3;

    unsigned char* buffer1 = (unsigned char*)malloc(bmp_file_size);
    unsigned char* buffer2 = (unsigned char*)malloc(yuv_file_size);
    unsigned char* buffer3 = (unsigned char*)malloc(yuv_file_size);

    if (NULL == buffer1 || NULL == buffer2 || NULL == buffer3)
    {
        printf("malloc failed!\n");
		fclose(fp1);
		fclose(fp2);
        return;
    }

    int ret = 0;
    ret = (int)fread(buffer1, 1, bmp_file_size, fp1);
    if (ret != bmp_file_size)
    {
        printf("fread failed! real: %d ret: %d\n", bmp_file_size, ret);
		fclose(fp1);
		fclose(fp2);
        return;
    }

#if 1
	unsigned char* lum = buffer2;
	unsigned char* cb = buffer2 + (w * h);
	unsigned char* cr = buffer2 + (w * h * 5 / 4);
	rgb24_to_yuv420p(lum, cb, cr, buffer1, w, h);
#endif
#if 0
	rgb24_to_yuv420p_flip (w, h, buffer1, buffer2, 0);
#endif
	yuv420p_to_yuv420sp(buffer3, buffer2, w, h, 1);
    //yuv422sp_to_yuv422p(buffer1, buffer2, w, h);
    //yuv420sp_to_yuv420p(buffer1, buffer2, w, h);

    ret = (int)fwrite(buffer3, 1, yuv_file_size, fp2);
    if (ret != yuv_file_size)
    {
        printf("fwrite failed! real: %d ret: %d\n", yuv_file_size, ret);
    }

    fclose(fp1);
    fclose(fp2);
	free(buffer1);
	buffer1 = NULL;
	free(buffer2);
	buffer2 = NULL;
	free(buffer3);
	buffer3 = NULL;
}

struct option longopts[] = {
    { "path",           required_argument,  NULL,   'p' },
    { "width",          required_argument,  NULL,   'w' },
    { "height",         required_argument,  NULL,   'h' },
	{ "output",			required_argument,	NULL,	'o' },
    { "help",           no_argument,        NULL,   'H' },
    { 0, 0, 0, 0 }
};

int main(int argc, char* argv[])
{
	int c = 0;
	int width = 0;
	int height = 0;
	char* picture_path = NULL;
	char* output_file = NULL;

	while ((c = getopt_long(argc, argv, "p:w:h:o:H", longopts, NULL)) != -1) {
		switch (c) {
			case 'p':                                                                                                   
				picture_path = optarg;
				printf("[%s:%d] picture_path = %s\n", __FILE__, __LINE__, picture_path);
				break;

			case 'w':
				width = atoi(optarg);
				printf("[%s:%d] width = %d\n", __FILE__, __LINE__, width);
				break;

			case 'h':
				height = atoi(optarg);
				printf("[%s:%d] height = %d\n", __FILE__, __LINE__, height);
				break;

			case 'o':
				output_file = optarg;
				printf("[%s:%d] output_file = %s\n", __FILE__, __LINE__, output_file);
				break;

			default:
				printf("%s [-p picture path] [-w width] [-h height] [-m model type] [-H]\n", argv[0]);
				exit(1);
		}
	}

    // 抽取文件
    //split_file("yuvfile/tempete_cif.yuv", "tempete_cif_yuv420p_00.yuv", 352, 288, 0);

    // OK
    //save_bmp420("yuvfile/suzie_qcif_yuv420p_00.yuv", "suzie_qcif_0.bmp", 176, 144);

	// OK
    //save_bmp422("yuvfile/colorbar_cif_yuv422p.yuv", "colorbar_cif_yuv422p.bmp", 352, 288);

	// OK
	//save_bmp422sp("yuvfile/yuv422sp_3000x1024.yuv", "test.bmp", 3000, 1024);
    
	// SP转P
    //ConvertImage("yuvfile/yuv422sp_3000x1024.yuv", "yuvfile/yuv422p.yuv", 3000, 1024);
    
	//save_bmp420("yuvfile/suzie_qcif_yuv420p_00.yuv", "suzie_qcif_1.bmp", 176, 144);

	// rgb to yuv420p
	save_yuv420p(picture_path, output_file, width, height);

	return 0;
}
