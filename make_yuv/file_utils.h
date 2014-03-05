/**
 * @file   file_utils.h
 * @author Late Lee <latelee@163.com>
 * @date   Tue May 14 08:56:48 2013
 * 
 * @brief  文件操作的一些常用函数
 * 
 * 
 */
#include <stdio.h>
#include <stdlib.h>

#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

int open_file_fd(const char* pathname, int flags);

FILE* open_file(const char* filename, const char *mode);

/////////////

int read_file(const char* filename, char** buffer, int* len);

int write_file(const char* filename, char* buffer, int len);

int get_filesize(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
