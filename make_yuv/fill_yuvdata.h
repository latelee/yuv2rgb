/**
 * @file   fill_yuvdata.h
 * @author Late Lee <latelee@163.com>
 * @date   Fri Feb 14 15:46:52 2014
 * 
 * @brief  填充YUV彩条数据，颜色参考ITU-R BT.601标准，共8种颜色
 * 
 * 
 */

#ifndef FILL_YUVDATA_H
#define FILL_YUVDATA_H

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * 填充UYVY格式数据，横条
 * 
 * @param buffer: 数据缓冲区，由调用者申请并保证有效 
 * @param width : 图像宽
 * @param height: 图像高
 */
void fill_yuv_uyvy_1(unsigned char* buffer, int width, int height);

/** 
 * 填充UYVY格式数据，竖条
 * 
 * @param buffer: 数据缓冲区，由调用者申请并保证有效 
 * @param width : 图像宽
 * @param height: 图像高
 */
void fill_yuv_uyvy_2(unsigned char* buffer, int width, int height);

/** 
 * 填充YUV422P格式数据，横条
 * 
 * @param buffer: 数据缓冲区，由调用者申请并保证有效 
 * @param width : 图像宽
 * @param height: 图像高
 */
void fill_yuv_yuv422_1(unsigned char* buffer, int width, int height);

/** 
 * 填充YUV422P格式数据，竖条
 * 
 * @param buffer: 数据缓冲区，由调用者申请并保证有效 
 * @param width : 图像宽
 * @param height: 图像高
 */
void fill_yuv_yuv422_2(unsigned char* buffer, int width, int height);

/** 
 * 填充YUV420P格式数据，横条
 * 
 * @param buffer: 数据缓冲区，由调用者申请并保证有效 
 * @param width : 图像宽
 * @param height: 图像高
 */
void fill_yuv_yuv420_1(unsigned char* buffer, int width, int height);

/** 
 * 填充YUV420P格式数据，竖条
 * 
 * @param buffer: 数据缓冲区，由调用者申请并保证有效 
 * @param width : 图像宽
 * @param height: 图像高
 */
void fill_yuv_yuv420_2(unsigned char* buffer, int width, int height);

#ifdef __cplusplus
}
#endif

#endif /* FILL_YUVDATA_H */
