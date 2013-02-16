/*!
  \file
  \brief ログの記録関数

  \author Satofumi KAMIMURA

  $Id: log_printf.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "log_printf.h"
#include <cstdio>
#include <cstdarg>


namespace
{
  const char* LogFile = "error_log.txt";
}


int qrk::log_printf(const char *format, ...)
{
  static FILE* fd = NULL;
  if (fd == NULL) {
    fd = fopen(LogFile, "w");
    if (fd == NULL) {
      return -1;
    }
  }

  va_list ap;

  // 標準エラー出力
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);

  // ファイル出力
  va_start(ap, format);
  int ret = vfprintf(fd, format, ap);
  va_end(ap);

  // fclose() する機会がないので、内容を書き出しておく
  fflush(fd);

  return ret;
}
