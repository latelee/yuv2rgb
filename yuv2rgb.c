#include <stdio.h>
#include <string.h>

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
�ڴ�ֲ�
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
    static int init_yuv422p = 0;    // just do it once

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

        // �˴��ɵ���RGB����BMPͼƬ����ΪBGR
        // Ĭ������Ϊ��RGB
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
//ת��ʱ�ӻ����������ǰת����RGB����ΪB��G��R��
//ת���õ���ͼ���ǵ����ģ�ֱ�ӽ�ת���󻺳���д��BMP�ļ����������鿴
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
�ڴ�ֲ�
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
�ڴ�ֲ�
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
    static int init_yuv422sp = 0;    // just do it once

    p_y = yuv422sp;
    p_uv = p_y + width * height;    // uv������Y����
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
        cr = p_uv[1];    // v����u����u����һ��λ��

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        // �˴��ɵ���RGB����BMPͼƬ����ΪBGR
        // Ĭ������Ϊ��RGB
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

int yuv_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width, int height)
{
    int ret = 0;

    switch (type)
    {
    case YUV420P:
        yuv420p_to_rgb24(yuvbuffer, rgbbuffer, width, height);
        break;
    case YUV422P:
        yuv422p_to_rgb24(yuvbuffer, rgbbuffer, width, height);
        break;
    case YUV422SP:
        yuv422sp_to_rgb24(yuvbuffer, rgbbuffer, width, height);
        break;
    default:
        printf("unsupport yuv type!\n");
        ret = -1;
        break;
    }

    return ret;
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


/**
yyyy yyyy
uu
vv
->
yyyy yyyy
uv    uv
*/
/* mode: 0 = uvuv, 1 = vuvu */
void yuv420p_to_yuv420sp(unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height, int mode)
{
    int i, j;
    int y_size = width * height;

    unsigned char* y = yuv420sp;
    unsigned char* uv = yuv420sp + y_size;

    unsigned char* y_tmp = yuv420p;
    unsigned char* u_tmp = yuv420p + y_size;
    unsigned char* v_tmp = yuv420p + y_size * 5 / 4;

	if(mode < 0) {
		mode = 0;
	}
	else if(mode > 1) {
		mode = 1;
	}
    // y
    memcpy(y, y_tmp, y_size);

    // uu/vv => uv/uv
    for (j = 0, i = 0; j < y_size/2; j+=2, i++)
    {
		if(mode == 0) {
			uv[j] = u_tmp[i];
			uv[j + 1] = v_tmp[i];
		}
		else {
			uv[j] = v_tmp[i];
			uv[j + 1] = u_tmp[i];
		}
    }
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// �Լ��о���

int rgb2yuv(int r, int g, int b, int* Y, int* Cb, int* Cr)
{
    int y, cb, cr;

    y = (int)( 0.299    * r + 0.587   * g + 0.114   * b);
    cb = (int)(-0.16874 * r - 0.33126 * g + 0.50000 * b + 128);
    if (cb < 0)
        cb = 0;
    cr = (int)( 0.50000 * r - 0.41869 * g - 0.08131 * b + 128);
    if (cr < 0)
        cr = 0;

    *Y = y;
    *Cb = cb;
    *Cr = cr;

    return 0;
}

// TODO����Ϊ��������
int yuv2rgb(int Y, int Cb, int Cr, int* r, int* g, int* b)
{
    unsigned int r_tmp, g_tmp, b_tmp;
    unsigned char r_t, g_t, b_t;

    // OK
    // ͼƬ�����С���ɴ��
#if 0
    // YUV(0~255) JFIF
    //r_t = (unsigned char)(Y + 1.402 * (Cr - 128));
    //g_t = (unsigned char)(Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
    //b_t = (unsigned char)(Y + 1.772 * (Cb - 128));

    r_tmp = (Y + 1.402 * (Cr - 128));
    g_tmp = (Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
    b_tmp = (Y + 1.772 * (Cb - 128));

    r_t =  MAX (0, MIN (255, r_tmp));
    g_t =  MAX (0, MIN (255, g_tmp));
    b_t =  MAX (0, MIN (255, b_tmp));

#endif

    // YCbCr(16~235)
    // �и�����ɫ�������ޡ���ɴ��
#if 01
    //r_t = (unsigned char)(1.164 * (Y - 16) + 1.596 * (Cr - 128));
    //g_t = (unsigned char)(1.164 * (Y - 16) - 0.813 * (Cr - 128) - 0.391 * (Cb - 128));
    //b_t = (unsigned char)(1.164 * (Y - 16) + 2.018 * (Cb - 128));

    r_tmp = (unsigned char)(1.164 * (Y - 16) + 1.596 * (Cr - 128));
    g_tmp = (unsigned char)(1.164 * (Y - 16) - 0.813 * (Cr - 128) - 0.391 * (Cb - 128));
    b_tmp = (unsigned char)(1.164 * (Y - 16) + 2.018 * (Cb - 128));

    r_t =  MAX (0, MIN (255, r_tmp));
    g_t =  MAX (0, MIN (255, g_tmp));
    b_t =  MAX (0, MIN (255, b_tmp));
#endif
    // not OK
#if 0
    r_t = (unsigned char)(Y + 1.403*Cr);
    g_t = (unsigned char)(Y - 0.344*Cb - 0.714*Cr);
    b_t = (unsigned char)(Y + 1.770*Cb);
#endif

    *r = r_t;
    *g = g_t;
    *b = b_t;

    return 0;
}

#if 01
#define RANGE_INT(iVal, iMin, iMax)                     ( ( ( iVal ) > ( iMin ) ) ? ( ( ( iVal ) <= ( iMax ) ) ? ( iVal ) : ( iMax ) ) : ( iMin ) )  
#define ROUND_SHR_POSITIVE(Dividend, iShiftRightCount)  ( ( ( Dividend ) & ( 1 << ( ( iShiftRightCount ) - 1 ) ) ) ? ( ( Dividend ) >> ( iShiftRightCount ) ) + 1 : ( ( Dividend ) >> ( iShiftRightCount ) ) )  
#define ROUND_SHR_NEGATIVE(Dividend, iShiftRightCount)  ( -( ( ( -( Dividend ) ) & ( 1 << ( ( iShiftRightCount ) - 1 ) ) ) ? ( ( -( Dividend ) ) >> ( iShiftRightCount ) ) + 1 : ( ( -( Dividend ) ) >> ( iShiftRightCount ) ) ) )  
#define ROUND_SHR(Dividend, iShiftRightCount)           ( ( ( Dividend ) >= 0 ) ? ROUND_SHR_POSITIVE( Dividend, iShiftRightCount ) : ROUND_SHR_NEGATIVE( Dividend, iShiftRightCount ) )  

void YCbCrConvertToRGB(int Y, int Cb, int Cr, int* R, int* G, int* B)  
{  
    int iTmpR = 0;  
    int iTmpG = 0;  
    int iTmpB = 0;  

    iTmpR = (((int)Y) << 14) + 22970*(((int)Cr) - 128);  
    iTmpG = (((int)Y) << 14) -  5638*(((int)Cb) - 128) - 11700*(((int)Cr) - 128);  
    iTmpB = (((int)Y) << 14) + 29032*(((int)Cb) - 128);  

    iTmpR = ROUND_SHR(iTmpR, 14);  
    iTmpG = ROUND_SHR(iTmpG, 14);  
    iTmpB = ROUND_SHR(iTmpB, 14);  

    *R = (int)RANGE_INT(iTmpR, 0, 255);  
    *G = (int)RANGE_INT(iTmpG, 0, 255);  
    *B = (int)RANGE_INT(iTmpB, 0, 255);  
    //printf("--%d %d %d %d %d %d--\n", iTmpR, iTmpG, iTmpB, *R, *G, *B);
} 
#endif

/*
y - y
u - cb
v - cr
���ڴ˺����е���RGB������
TODO�����ַ���ת���õ���ͼƬ�Ȳ���õ���Ч����һЩ
*/
void yuv422_to_rgb24_1(unsigned char* yuv422, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;
    
    p_y = yuv422;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;
    p_rgb = rgb;

    for (i = 0; i < width * height / 2; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        //yuv2rgb(y, cb, cr, &r, &g, &b);
        YCbCrConvertToRGB(y, cb, cr, &r, &g, &b);
        // �˴��ɵ���RGB����BMPͼƬ����ΪBGR
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        //yuv2rgb(y, cb, cr, &r, &g, &b);
        YCbCrConvertToRGB(y, cb, cr, &r, &g, &b);
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
y - y
u - cb
v - cr
Q:Ϊ�β���4��Y��ͬһ��U��һ��V��
not ok��ͼƬ�°벿���Ǻ�ɫ��
*/
void yuv420_to_rgb24_1(unsigned char* yuv420, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;

    p_y = yuv420;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;
    p_rgb = rgb;

    for (i = 0; i < width * height / 4; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        //YCbCrConvertToRGB(y, cb, cr, &r, &g, &b);
        // �˴��ɵ���RGB����BMPͼƬ����ΪBGR
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        //YCbCrConvertToRGB(y, cb, cr, &r, &g, &b);
        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}

//enum {
//YUV422,
//YUV420,
//};

// OK
// ת����ͼƬ��һ�㡰��ɴ��(�����Ա�)��Ӧ����Y���ַ�Χ����ȷ���¡�
void yuv420_to_rgb24_2(unsigned char *yuv420, unsigned char *rgb24, int width, int height) 
{
    //  int begin = GetTickCount();
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    int R,G,B,Y,U,V;
    int x,y;
    int nWidth = width>>1; //ɫ���źſ��

    p_y = yuv420;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;

    for (y=0;y<height;y++)
    {
        for (x=0;x<width;x++)
        {
            Y = *(p_y + y*width + x);
            U = *(p_u + ((y>>1)*nWidth) + (x>>1));
            V = *(p_v + ((y>>1)*nWidth) + (x>>1));
            /*
            R = Y + 1.402*(V-128);
            G = Y - 0.34414*(U-128) - 0.71414*(V-128);
            B = Y + 1.772*(U-128);
            */

            yuv2rgb(Y, U, V, &R, &G, &B);
            //YCbCrConvertToRGB(Y, U, V, &R, &G, &B);
            //��ֹԽ��
            if (R>255)R=255;
            if (R<0)R=0;
            if (G>255)G=255;
            if (G<0)G=0;
            if (B>255)B=255;
            if (B<0)B=0;

            // ͼƬ����
            //*(rgb24 + ((height-y-1)*width + x)*3) = R;  //B;
            //*(rgb24 + ((height-y-1)*width + x)*3 + 1) = G;
            //*(rgb24 + ((height-y-1)*width + x)*3 + 2) = B;  //R;
            // ͼ������
            *(rgb24 + (y*width + x)*3) = R; //B;
            *(rgb24 + (y*width + x)*3 + 1) = G;
            *(rgb24 + (y*width + x)*3 + 2) = B; //R;   
        }
    }
}

// not ok
void yuv420_to_rgb24_3(unsigned char* yuv, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    int j = 0;
    unsigned char* p_y1;
    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb1;
    unsigned char* p_rgb2;

    p_u = yuv + width * height;
    p_v = p_u + width * height / 4;

    p_y1 = yuv;
    p_y2 = yuv + width;
    p_rgb1 = rgb;
    p_rgb2 = rgb + 3 * width;

    for (i = 0; i < height; i ++)
    {
        for (j = 0; j < width; j ++)
        {
            cb = *p_u++;
            cr = *p_v++;

            y  = *p_y1++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb1++ = r;
            *p_rgb1++ = g;
            *p_rgb1++ = b;
        }
    }
#if 1
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            cb = *p_u++;
            cr = *p_v++;

            y  = *p_y1++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb1++ = r;
            *p_rgb1++ = g;
            *p_rgb1++ = b;

            y  = *p_y2++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb2++ = r;
            *p_rgb2++ = g;
            *p_rgb2++ = b;

            y  = *p_y1++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb1++ = r;
            *p_rgb1++ = g;
            *p_rgb1++ = b;

            y  = *p_y2++;

            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb2++ = r;
            *p_rgb2++ = g;
            *p_rgb2++ = b;

            //p_y1 += 2;
            //p_y2 += 2;

            //p_rgb1 += 6;
            //p_rgb2 += 6;
        }

        p_y1 += width;
        p_y2 += width;
        p_rgb1 += 3*width;
        p_rgb2 += 3*width;
    }
#endif
}

void yuv_to_rgb24_1(unsigned char* yuv, unsigned char* rgb, int width, int height, YUV_TYPE type)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;
    int chroma_v;       // ��ֱ���Ȳ���
    int chroma_h;       // ˮƽ���Ȳ���
    int cbcr_width;     // ɫ�ȿ�
    int cbcr_height;    // ɫ�ȸ�
    int cbcr_size;      // ɫ�ȴ�С

    switch(type)
    {
    case YUV420P:
        chroma_v = 2;
        chroma_h = 2;
        break;
    case YUV422P:
        chroma_v = 4;
        chroma_h = 2;
        break;
    default:
        chroma_v = chroma_h = 2;
        break;
    }
    cbcr_width  = width * (chroma_h / 4.0);
    cbcr_height = height * (chroma_v / 4.0);
    cbcr_size   = cbcr_width * cbcr_height;

    p_y = yuv;
    p_u = p_y + width * height;
    p_v = p_u + cbcr_size;
    p_rgb = rgb;

    for (i = 0; i < cbcr_size; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}
