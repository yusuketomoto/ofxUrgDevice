#ifndef QRK_SPLIT_H
#define QRK_SPLIT_H

/*!
  \file
  \brief string 用の split 関数

  \author Satofumi KAMIMURA

  $Id: split.h 1811 2010-04-30 16:12:05Z satofumi $
*/

#include <vector>
#include <string>


namespace qrk
{
    /*!
      \brief 指定文字による分割

      \param[out] tokens 分割後の文字列
      \param[in] original 分割対象の文字列
      \param[in] split_pattern 分割を行う文字の列
      \param[in] continious_pattern 連続したセパレータを１つのセパレータとして扱う

      \return 分割後のトークン数
    */
    size_t split(std::vector<std::string>& tokens,
                 const std::string& original, const char* split_pattern = " \t",
                 bool continious_pattern = true);
}

#endif /* !QRK_SPLIT_H */
