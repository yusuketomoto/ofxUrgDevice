/*!
  \file
  \brief string 用の split 関数

  boost を使わずに実装 \n
  boost を使って良いのならば、boost::algorithm::split を使うべき

  \author Satofumi KAMIMURA

  $Id: split.cpp 1811 2010-04-30 16:12:05Z satofumi $
*/

#include "split.h"

using namespace std;


size_t qrk::split(std::vector<std::string>& tokens,
                  const std::string& line, const char* split_pattern,
                  bool continious_pattern)
{
    string buffer = line;
    char* q = &buffer[0];
    size_t n = line.size();
    for (size_t i = 0; i < n; ++i, ++q) {
        for (const char* p = split_pattern; *p != '\0'; ++p) {
            if (*q == *p) {
                *q = '\0';
                break;
            }
        }
    }

    for (size_t i = 0; i < n; ++i) {
        if ((buffer[i] != '\0') || (! continious_pattern)) {
            string line = &buffer[i];
            tokens.push_back(line);
            i += line.size();
        }
    }
    return tokens.size();
}
