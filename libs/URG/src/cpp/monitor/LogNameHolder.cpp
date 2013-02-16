/*!
  \file
  \brief ÉçÉOñºÇÃä«óù

  \author Satofumi KAMIMURA

  $Id: LogNameHolder.cpp 1469 2009-10-30 00:45:14Z satofumi $
*/

#include "LogNameHolder.h"
#include "DetectOS.h"
#include <map>
#include <cstdio>

#ifdef MSC
#define snprintf _snprintf
#endif

using namespace qrk;
using namespace std;


namespace
{
  typedef map<string, int> Names;
}


struct LogNameHolder::pImpl
{
  Names names_number_;


  string name(const char* baseName)
  {
    Names::iterator p = names_number_.find(string(baseName));
    int id = (p == names_number_.end()) ? 0 : names_number_[baseName] + 1;
    names_number_[baseName] = id;

    enum { BufferSize = 14 };
    char buffer[BufferSize];
    snprintf(buffer, BufferSize, "_%d", id);

    return (string(baseName) + buffer + ".log").c_str();
  }
};


LogNameHolder::LogNameHolder(void) : pimpl(new pImpl)
{
}


LogNameHolder::~LogNameHolder(void)
{
}


LogNameHolder* LogNameHolder::object(void)
{
  static LogNameHolder singleton_object;
  return &singleton_object;
}


string LogNameHolder::name(const char* baseName)
{
  return pimpl->name(baseName);
}
