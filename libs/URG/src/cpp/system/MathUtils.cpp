/*!
  \file
  \brief 数学関数の補助ファイル

  \author Satofumi KAMIMURA

  $Id: MathUtils.cpp 1811 2010-04-30 16:12:05Z satofumi $
*/

#include "MathUtils.h"


#if defined(MSC)
long lrint(double x)
{
    if (x >= 0) {
        x += 0.5;
    } else {
        x -= 0.5;
    }
    return static_cast<long>(x);
}
#endif
