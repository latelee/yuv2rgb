#include <stdio.h>
#include "yuv2rgb.h"
#include "bmp_utils.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static long U[256], V[256], Y1[256], Y2[256];

void init_yuv422p_table(void)
{
    int i;

    // Initialize table
    for (i = 0; i < 256; i++)
    { 
        V[i]  = 15938 * i - 2221300;
        U[i]  = 20238 * i - 2771300;
        Y1[i] = 11644 * i;
        Y2[i] = 19837 * i - 311710;
    }
}

/**
内存分布
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0U1      |
            |...       |   h
            |...       |
            |          |
            +----------+
            |V0V1      |
            |...       |  h
            |...       |
            |          |
            +----------+
                w/2
*/
void yuv422p_to_rgb24(unsigned char* yuv422p, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;
	static int init_yuv422p = 0;	// just do it once

    p_y = yuv422p;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;
    p_rgb = rgb;

	if (init_yuv422p == 0)
	{
		init_yuv422p_table();
		init_yuv422p = 1;
	}

    for (i = 0; i < width * height / 2; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        // 此处可调整RGB排序，BMP图片排序为BGR
        // 默认排序为：RGB
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}

/*
//转换时从缓冲区最后向前转换，RGB排序为B、G、R，
//转换得到的图像是倒立的，直接将转换后缓冲区写入BMP文件即可正常查看
void yuv422p_to_rgb24(unsigned char *yuvbuffer, unsigned char *rgbbuffer, int width, int height)
{
    unsigned char *pY, *pU, *pV, *pUbase, *pVbase;
    unsigned char *pR, *pG, *pB;
    int i = 0;
    int j = 0;
    long RGB_SIZE = 0;
    
    RGB_SIZE = width * height * 3;

    pY = yuvbuffer;
    pU = pUbase = yuvbuffer + width * height;
    pV = pVbase = pUbase + width * height / 2;

    for (i = 0; i < height; i++)
    {
        pB = rgbbuffer + RGB_SIZE - 3*width*(i+1);
        pG = rgbbuffer + RGB_SIZE - 3*width*(i+1) + 1;
        pR = rgbbuffer + RGB_SIZE - 3*width*(i+1) + 2;

        for (j=0; j < width; j += 2)
        {
            *pR = MAX (0, MIN (255, (V[*pV] + Y1[*pY])/10000));   //R value
            *pB = MAX (0, MIN (255, (U[*pU] + Y1[*pY])/10000));   //B value
            *pG = MAX (0, MIN (255, (Y2[*pY] - 5094*(*pR) - 1942*(*pB))/10000)); //G value

            pR += 3;
            pB += 3;
            pG += 3;
            pY++;

            *pR = MAX (0, MIN (255, (V[*pV] + Y1[*pY])/10000) );  //R value
            *pB = MAX (0, MIN (255, (U[*pU] + Y1[*pY])/10000) );  //B value
            *pG = MAX (0, MIN (255, (Y2[*pY] - 5094*(*pR) - 1942*(*pB))/10000) ); //G value

            pR += 3;
            pB += 3;
            pG += 3;

            pY++;
            pU++;
            pV++;
        }
    }
}
*/

////////////////////////////////////////////////////////////////////////////

static long int crv_tab[256];   
static long int cbu_tab[256];   
static long int cgu_tab[256];   
static long int cgv_tab[256];   
static long int tab_76309[256]; 
static unsigned char clp[1024];   //for clip in CCIR601   

void init_yuv420p_table() 
{   
    long int crv,cbu,cgu,cgv;   
    int i,ind;      
   
    crv = 104597; cbu = 132201;  /* fra matrise i global.h */   
    cgu = 25675;  cgv = 53279;   
   
    for (i = 0; i < 256; i++)    
    {   
        crv_tab[i] = (i-128) * crv;   
        cbu_tab[i] = (i-128) * cbu;   
        cgu_tab[i] = (i-128) * cgu;   
        cgv_tab[i] = (i-128) * cgv;   
        tab_76309[i] = 76309*(i-16);   
    }   
   
    for (i = 0; i < 384; i++)   
        clp[i] = 0;   
    ind = 384;   
    for (i = 0;i < 256; i++)   
        clp[ind++] = i;   
    ind = 640;   
    for (i = 0;i < 384; i++)   
        clp[ind++] = 255;   
}

/**
内存分布
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0U1      |
            |...       |   h/2
            |...       |
            |          |
            +----------+
            |V0V1      |
            |...       |  h/2
            |...       |
            |          |
            +----------+
                w/2
 */
void yuv420p_to_rgb24(unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)   
{
    int y1, y2, u, v;    
    unsigned char *py1, *py2;   
    int i, j, c1, c2, c3, c4;   
    unsigned char *d1, *d2;   
    unsigned char *src_u, *src_v;
	static int init_yuv420p = 0;
    
    src_u = yuvbuffer + width * height;   // u
    src_v = src_u + width * height / 4;  // v

    py1 = yuvbuffer;   // y
    py2 = py1 + width;   
    d1 = rgbbuffer;   
    d2 = d1 + 3 * width;   

	if (init_yuv420p == 0)
	{
		init_yuv420p_table();
		init_yuv420p = 1;
	}

    for (j = 0; j < height; j += 2)    
    {    
        for (i = 0; i < width; i += 2)    
        {
            u = *src_u++;   
            v = *src_v++;   
   
            c1 = crv_tab[v];   
            c2 = cgu_tab[u];   
            c3 = cgv_tab[v];   
            c4 = cbu_tab[u];   
   
            //up-left   
            y1 = tab_76309[*py1++];    
            *d1++ = clp[384+((y1 + c1)>>16)];     
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];   
            *d1++ = clp[384+((y1 + c4)>>16)];   
   
            //down-left   
            y2 = tab_76309[*py2++];   
            *d2++ = clp[384+((y2 + c1)>>16)];     
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];   
            *d2++ = clp[384+((y2 + c4)>>16)];   
   
            //up-right   
            y1 = tab_76309[*py1++];   
            *d1++ = clp[384+((y1 + c1)>>16)];     
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];   
            *d1++ = clp[384+((y1 + c4)>>16)];   
   
            //down-right   
            y2 = tab_76309[*py2++];   
            *d2++ = clp[384+((y2 + c1)>>16)];     
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];   
            *d2++ = clp[384+((y2 + c4)>>16)];   
        }
        d1  += 3*width;
        d2  += 3*width;
        py1 += width;
        py2 += width;
    }
}

/**
内存分布
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0V0U1V1            |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
                w/2
*/
void yuv422sp_to_rgb24(unsigned char* yuv422sp, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_uv;
    unsigned char* p_rgb;
	static int init_yuv422sp = 0;	// just do it once

    p_y = yuv422sp;
    p_uv = p_y + width * height;	// uv分量在Y后面
    p_rgb = rgb;

	if (init_yuv422sp == 0)
	{
		init_yuv422p_table();
		init_yuv422sp = 1;
	}

    for (i = 0; i < width * height / 2; i++)
    {
        y  = p_y[0];
        cb = p_uv[0];
        cr = p_uv[1];	// v紧跟u，在u的下一个位置

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        // 此处可调整RGB排序，BMP图片排序为BGR
        // 默认排序为：RGB
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_uv[0];
        cr = p_uv[1];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_uv += 2;
        p_rgb += 6;
    }
}

//===================================================================

/**
yyyy yyyy
uv    uv
->
yyyy yyyy
uu
vv
*/
void yuv422sp_to_yuv422p(unsigned char* yuv422sp, unsigned char* yuv422p, int width, int height)
{
    int i, j;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;

    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;

    y_size = uv_size = width * height;

    p_y1 = yuv422sp;
    p_uv = yuv422sp + y_size;

    p_y2 = yuv422p;
    p_u  = yuv422p + y_size;
    p_v  = p_u + width * height / 2;

    memcpy(p_y2, p_y1, y_size);

    for (j = 0, i = 0; j < uv_size; j+=2, i++)
    {
        p_u[i] = p_uv[j];
        p_v[i] = p_uv[j+1];
    }
}

/**
yyyy yyyy
uv    uv
->
yyyy yyyy
uu
vv
*/
void yuv420sp_to_yuv420p(unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height)
{
    int i, j;
    int y_size = width * height;

    unsigned char* y = yuv420sp;
    unsigned char* uv = yuv420sp + y_size;

    unsigned char* y_tmp = yuv420p;
    unsigned char* u_tmp = yuv420p + y_size;
    unsigned char* v_tmp = yuv420p + y_size * 5 / 4;

    // y
    memcpy(y_tmp, y, y_size);

    // u
    for (j = 0, i = 0; j < y_size/2; j+=2, i++)
    {
        u_tmp[i] = uv[j];
        v_tmp[i] = uv[j+1];
    }
}
