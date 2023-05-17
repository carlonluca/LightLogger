#ifndef APPLOGGING_H
#define APPLOGGING_H

#include "lc_logging.h"
#define CUSTOM_LOGGER
class LC_CustomOutput
{
public:
    static void printf(lightlogger::LC_Log<LC_CustomOutput>&, va_list) {
        ::printf("HELLO\n");
    }
};
namespace lightlogger {
typedef LC_Log<LC_CustomOutput> LC_LogDef;
}
#include "lc_logging_utils.h"
#include "lc_logging_qt.h"

#endif // APPLOGGING_H
