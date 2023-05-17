#ifndef LC_LOGGING_UTILS_H
#define LC_LOGGING_UTILS_H

namespace lightlogger {

// Define the default logger.
#ifndef CUSTOM_LOGGER
#ifdef __ANDROID__
typedef LC_Log<LC_OutputAndroid> LC_LogDef;
#elif defined(XCODE_COLORING_ENABLED)
typedef LC_Log<LC_Output2XCodeColors> LC_LogDef;
#elif defined(ENABLE_MSVS_OUTPUT)
typedef LC_Log<LC_Output2MSVS> LC_LogDef;
#else
typedef LC_Log<LC_Output2Std> LC_LogDef;
#endif
#endif // CUSTOM_LOGGER

#ifdef ENABLE_CODE_LOCATION
#define DEF_FUNC(name) f_log_ ##name
#else
#define DEF_FUNC(name) log_ ##name
#endif // ENABLE_CODE_LOCATION

#define SHOW(name) show_ ##name

#define GENERATE_LEVEL(name, enumname, retval)                                          \
    inline bool DEF_FUNC(name ##_t_v)(const char* log_tag, const char* format, va_list args) \
    {                                                                                    \
            LC_LogDef(log_tag, enumname).printf(format, args);                                \
            return retval;                                                                    \
    }                                                                                    \
                                                                                            \
    inline bool DEF_FUNC(name ##_t)(const char* log_tag, const char* format, ...)            \
    {                                                                                    \
            VA_LIST_CONTEXT(format, LC_LogDef(log_tag, enumname).printf(format, args));       \
            return retval;                                                                    \
    }                                                                                    \
                                                                                            \
        inline bool DEF_FUNC(name ##_v)(const char* format, va_list args)                        \
    {                                                                                    \
            LC_LogDef(enumname).printf(format, args);                                         \
            return retval;                                                                    \
    }                                                                                    \
        inline bool DEF_FUNC(name)(const char* format, ...)                                      \
    {                                                                                    \
            VA_LIST_CONTEXT(format, LC_LogDef(enumname).printf(format, args));                \
            return retval;                                                                    \
    }                                                                                    \
        inline bool SHOW(name ##_t_v)(const char* log_tag, const char* format, va_list args) \
    {                                                                                    \
            LC_LogDef(log_tag, enumname, false).printf(format, args);                         \
            return retval;                                                                    \
    }                                                                                    \
                                                                                            \
        inline bool SHOW(name ##_t)(const char* log_tag, const char* format, ...)            \
    {                                                                                    \
            VA_LIST_CONTEXT(format, LC_LogDef(log_tag, enumname, false).printf(format, args));\
            return retval;                                                                    \
    }                                                                                    \
                                                                                            \
        inline bool SHOW(name ##_v)(const char* format, va_list args)                        \
    {                                                                                    \
            LC_LogDef(enumname, false).printf(format, args);                                  \
            return retval;                                                                    \
    }                                                                                    \
        inline bool SHOW(name)(const char* format, ...)                                      \
    {                                                                                    \
            VA_LIST_CONTEXT(format, LC_LogDef(enumname, false).printf(format, args));         \
            return retval;                                                                    \
    }

#if defined(__APPLE__) && __OBJC__ == 1
#define GENERATE_LEVEL_OBJC(name, enumname, retval)                                   \
inline bool FUNC(name ##_t_v)(const char* log_tag, NSString* format, va_list args) \
{                                                                                  \
        LC_LogDef(log_tag, enumname).printf(format, args);                              \
        return retval;                                                                  \
}                                                                                  \
                                                                                      \
    inline bool FUNC(name ##_t)(const char* log_tag, NSString* format, ...)            \
{                                                                                  \
        VA_LIST_CONTEXT(format, LC_LogDef(log_tag, enumname).printf(format, args));     \
        return retval;                                                                  \
}                                                                                  \
                                                                                      \
    inline bool FUNC(name ##_v)(NSString* format, va_list args)                        \
{                                                                                  \
        LC_LogDef(enumname).printf(format, args);                                       \
        return retval;                                                                  \
}                                                                                  \
                                                                                      \
    inline bool FUNC(name)(NSString* format, ...)                                      \
{                                                                                  \
        VA_LIST_CONTEXT(format, LC_LogDef(enumname).printf(format, args));              \
        return retval;                                                                  \
}
#else
#define GENERATE_LEVEL_OBJC(name, enumname, retval)
#endif // defined(__APPLE__) && __OBJC__ == 1

#define GENERATE_LEVEL_CUSTOM(name, rettype, content)    \
inline rettype log_ ##name ##_t_v(...)   {content;}   \
    inline rettype log_ ##name ##_t(...)     {content;}   \
    inline rettype log_ ##name ##_v(...)     {content;}   \
    inline rettype log_ ##name(...)          {content;}   \
    inline rettype f_log_ ##name ##_t_v(...) {content;}   \
    inline rettype f_log_ ##name ##_t(...)   {content;}   \
    inline rettype f_log_ ##name ##_v(...)   {content;}   \
    inline rettype f_log_ ##name(...)        {content;}

#ifdef ENABLE_LOG_CRITICAL
GENERATE_LEVEL(critical, LC_LOG_CRITICAL, false)
GENERATE_LEVEL_OBJC(critical, LC_LOG_CRITICAL, NO)
#ifdef ENABLE_CODE_LOCATION
#define log_critical_t_v(tag, format, args) \
log_location_t_v(f_log_critical_t_v, tag, format, args)
#define log_critical_t(tag, format, ...) \
    log_location_t(lightlogger::f_log_critical_t, tag, format, ##__VA_ARGS__)
#define log_critical_v(format, args) \
    log_location_v(f_log_critical_v, format, args)
#define log_critical(format, ...) \
    log_location(lightlogger::f_log_critical, format, ##__VA_ARGS__)
#endif // ENABLE_CODE_LOCATION
#else
GENERATE_LEVEL_CUSTOM(critical, bool, return false)
#endif // ENABLE_LOG_CRITICAL

#ifdef ENABLE_LOG_ERROR
    GENERATE_LEVEL(err, LC_LOG_ERROR, false)
    GENERATE_LEVEL_OBJC(err, LC_LOG_ERROR, NO)
#ifdef ENABLE_CODE_LOCATION
#define log_err_t_v(tag, format, args) \
    log_location_t_v(f_log_err_t_v, tag, format, args)
#define log_err_t(tag, format, ...) \
    log_location_t(f_log_err_t, tag, format, ##__VA_ARGS__)
#define log_err_v(format, args) \
    log_location_v(f_log_err_v, format, args)
#define log_err(format, ...) \
    log_location(lightlogger::f_log_err, format, ##__VA_ARGS__)
#endif // ENABLE_CODE_LOCATION
#else
GENERATE_LEVEL_CUSTOM(err, bool, return false)
#endif // ENABLE_LOG_ERROR

#ifdef ENABLE_LOG_WARNING
    GENERATE_LEVEL(warn, LC_LOG_WARN, false)
    GENERATE_LEVEL_OBJC(warn, LC_LOG_WARN, NO)
#ifdef ENABLE_CODE_LOCATION
#define log_warn_t_v(tag, format, args) \
    log_location_t_v(f_log_warn_t_v, tag, format, args)
#define log_warn_t(tag, format, ...) \
    log_location_t(f_log_warn_t, tag, format, ##__VA_ARGS__)
#define log_warn_v(format, args) \
    log_location_v(f_log_warn_v, format, args)
#define log_warn(format, ...) \
    log_location(lightlogger::f_log_warn, format, ##__VA_ARGS__)
#endif // ENABLE_CODE_LOCATION
#else
GENERATE_LEVEL_CUSTOM(warn, bool, return false)
#endif // ENABLE_LOG_WARNING

#ifdef ENABLE_LOG_INFORMATION
    GENERATE_LEVEL(info, LC_LOG_INFO, true)
    GENERATE_LEVEL_OBJC(info, LC_LOG_INFO, YES)
#ifdef ENABLE_CODE_LOCATION
#define log_info_t_v(tag, format, args) \
    log_location_t_v(lightlogger::f_log_info_t_v, tag, format, args)
#define log_info_t(tag, format, ...) \
    log_location_t(lightlogger::f_log_info_t, tag, format, ##__VA_ARGS__)
#define log_info_v(format, args) \
    log_location_v(lightlogger::f_log_info_v, format, args)
#define log_info(format, ...) \
    log_location(lightlogger::f_log_info, format, ##__VA_ARGS__)
#endif // ENABLE_CODE_LOCATION

/*------------------------------------------------------------------------------
|    log_formatted_t
+-----------------------------------------------------------------------------*/
    inline bool log_formatted_t_v(const char* log_tag, LC_LogAttrib a, LC_LogColor c, const char* format, va_list args)
{
    LC_LogDef(log_tag, a, c).printf(format, args);
    return true;
}

/*------------------------------------------------------------------------------
|    log_formatted_t
+-----------------------------------------------------------------------------*/
inline bool log_formatted_t(const char* log_tag, LC_LogAttrib a, LC_LogColor c, const char* format, ...)
{
    VA_LIST_CONTEXT(format, LC_LogDef(log_tag, a, c).printf(format, args));
    return true;
}

/*------------------------------------------------------------------------------
|    log_formatted_v
+-----------------------------------------------------------------------------*/
inline bool log_formatted_v(LC_LogAttrib a, LC_LogColor c, const char* format, va_list args)
{
    LC_LogDef(LOG_TAG, a, c).printf(format, args);
    return true;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted(LC_LogAttrib a, LC_LogColor c, const char* format, ...)
{
    VA_LIST_CONTEXT(format, LC_LogDef(LOG_TAG, a, c).printf(format, args));
    return true;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted(LC_LogColor c, const char* format, ...)
{
    VA_LIST_CONTEXT(format, LC_LogDef(LOG_TAG, LC_LOG_ATTR_RESET, c).printf(format, args));
    return true;
}

#if defined(__APPLE__) && __OBJC__ == 1
/*------------------------------------------------------------------------------
|    log_formatted_t
+-----------------------------------------------------------------------------*/
inline bool log_formatted_t(const char* log_tag, LC_LogAttrib a, LC_LogColor c, NSString* format, va_list args)
{
    LC_LogDef(log_tag, a, c).printf(format, args);
    return YES;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted_t(const char* log_tag, LC_LogAttrib a, LC_LogColor c, NSString* format, ...)
{
    VA_LIST_CONTEXT(format, LC_LogDef(log_tag, a, c).printf(format, args));
    return YES;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted(LC_LogAttrib a, LC_LogColor c, NSString* format, ...)
{
    VA_LIST_CONTEXT(format, log_formatted_t(LOG_TAG, a, c, format, args));
    return YES;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted(LC_LogColor c, NSString* format, ...)
{
    VA_LIST_CONTEXT(format, log_formatted_t(LOG_TAG, LC_LOG_ATTR_RESET, c, format, args));
    return YES;
}
#endif // defined(__APPLE__) && __OBJC__ == 1
#else
GENERATE_LEVEL_CUSTOM(info, bool, return true)
inline bool log_formatted_t_v(...) { return true; }
inline bool log_formatted_t(...)   { return true; }
inline bool log_formatted_v(...)   { return true; }
inline bool log_formatted(...)     { return true; }
#endif // ENABLE_LOG_INFORMATION

#ifdef ENABLE_LOG_VERBOSE
GENERATE_LEVEL(verbose, LC_LOG_VERBOSE, true)
GENERATE_LEVEL_OBJC(verbose, LC_LOG_VERBOSE, YES)
#ifdef ENABLE_CODE_LOCATION
#define log_verbose_t_v(tag, format, args) \
log_location_t_v(f_log_verbose_t_v, tag, format, args)
#define log_verbose_t(tag, format, ...) \
    log_location_t(f_log_verbose_t, tag, format, ##__VA_ARGS__)
#define log_verbose_v(format, args) \
    log_location_v(f_log_verbose_v, format, args)
#define log_verbose(format, ...) \
    log_location(f_log_verbose, format, ##__VA_ARGS__)
#endif // ENABLE_CODE_LOCATION
#else
GENERATE_LEVEL_CUSTOM(verbose, bool, return true)
#endif // ENABLE_LOG_VERBOSE

#ifdef ENABLE_LOG_DEBUG
    GENERATE_LEVEL(debug, LC_LOG_DEBUG, true)
    GENERATE_LEVEL_OBJC(debug, LC_LOG_DEBUG, YES)
#ifdef ENABLE_CODE_LOCATION
#define log_debug_t_v(tag, format, args) \
    log_location_t_v(f_log_debug_t_v, tag, format, args)
#define log_debug_t(tag, format, ...) \
    log_location_t(f_log_debug_t, tag, format, ##__VA_ARGS__)
#define log_debug_v(format, args) \
    log_location_v(f_log_debug_v, format, args)
#define log_debug(format, ...) \
    log_location(f_log_debug, format, ##__VA_ARGS__)
#endif // ENABLE_CODE_LOCATION
#else
GENERATE_LEVEL_CUSTOM(debug, bool, return true)
#endif // ENABLE_LOG_DEBUG

    GENERATE_LEVEL_CUSTOM(disabled, bool, return true)

#if defined(__APPLE__) && __OBJC__ == 1
    /*------------------------------------------------------------------------------
|    log_disabled_t_v
+-----------------------------------------------------------------------------*/
    inline bool log_disabled_t_v(const char* log_tag, NSString* format, va_list args)
{
    LOG_UNUSED(log_tag);
    LOG_UNUSED(format);
    LOG_UNUSED(args);

    return true;
}

/*------------------------------------------------------------------------------
|    log_disabled_t
+-----------------------------------------------------------------------------*/
inline bool log_disabled_t(const char* log_tag, NSString* format, ...)
{
    LOG_UNUSED(log_tag);
    LOG_UNUSED(format);

    return true;
}

/*------------------------------------------------------------------------------
|    log_disabled_v
+-----------------------------------------------------------------------------*/
inline bool log_disabled_v(NSString* format, va_list args)
{
    LOG_UNUSED(format);
    LOG_UNUSED(args);

    return true;
}

/*------------------------------------------------------------------------------
|    log_disabled
+-----------------------------------------------------------------------------*/
inline bool log_disabled(NSString* format, ...)
{
    LOG_UNUSED(format);

    return true;
}
#endif // defined(__APPLE__) && __OBJC__ == 1

// Convenience macros. The same as using the inlined functions.
#ifdef __GNUC__
#define LOG_CRITICAL(tag, f, ...) \
log_critical_t(tag, f, ##__VA_ARGS__)
#define LOG_ERROR(tag, f, ...) \
    log_err_t(tag, f, ##__VA_ARGS__)
#define LOG_WARNING(tag, f, ...) \
    log_warn_t(tag, f, ##__VA_ARGS__)
#define LOG_INFORMATION(tag, f, ...) \
    log_info_t(tag, f, ##__VA_ARGS__)
#define LOG_VERBOSE(tag, f, ...) \
    log_verbose_t(tag, f, ##__VA_ARGS__)
#define LOG_DEBUG(tag, f, ...) \
    log_debug_t(tag, f, ##__VA_ARGS__)
#endif

#define log_func \
    lightlogger::log_debug("Entering: %s.", __PRETTY_FUNCTION__)
#define log_info_func \
    lightlogger::log_info("Entering: %s.", __PRETTY_FUNCTION__)
#define log_verbose_func \
    lightlogger::log_verbose("Entering: %s.", __PRETTY_FUNCTION__)
#define log_debug_func \
    lightlogger::log_debug("Entering: %s.", __PRETTY_FUNCTION__)

#if !defined(__ANDROID__) && (!defined(WINVER) || WINVER < 0x0602)
/* Unfortunately backtrace() is not supported by Bionic */

#if !defined(_WIN32) && !defined(_WIN32_WCE)
/*------------------------------------------------------------------------------
|    log_stacktrace
+-----------------------------------------------------------------------------*/
/**
* Prints a demangled stack backtrace of the caller function. Remember to build adding the
* flag -rdynamic.
*
* @param log_tag The log tag to be placed in the log line.
* @param level The log level of the log.
* @param max_frames The maximum number of lines of stack trace to log.
*/
inline void log_stacktrace(const char* log_tag, LC_LogLevel level, unsigned int max_frames)
{
    std::stringstream stream;
    stream << std::endl;

    // storage array for stack trace address data
    void* addrlist[max_frames + 1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, (int)(sizeof(addrlist) / sizeof(void*)));

    if (addrlen == 0) {
        stream << "<empty, possibly corrupt>";
        LC_LogDef(log_tag, level).printf("%s", stream.str().c_str());
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*) malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++) {
        char* begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char* p = symbollist[i]; *p; ++p) {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(
                begin_name,
                funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string
                stream << "  " << symbollist[i] << ": "
                       << funcname << "+" << begin_offset << std::endl;
            }
            else
                // demangling failed. Output function name as a C function with
                // no arguments.
                stream << "  " << symbollist[i] << ": "
                       << begin_name << "()+" << begin_offset << std::endl;
        }
        else
            // couldn't parse the line? print the whole line.
            stream << "  " << symbollist[i] << std::endl;
    }

    LC_LogDef(log_tag, level).printf("%s", stream.str().c_str());

    free(funcname);
    free(symbollist);
}
#else
/*------------------------------------------------------------------------------
|    log_stacktrace
+-----------------------------------------------------------------------------*/
inline void log_stacktrace(const char* log_tag, LC_LogLevel level, unsigned int max_frames)
{
    unsigned int i;
    void* stack = malloc(sizeof(void*) *max_frames);
    unsigned short frames;
    SYMBOL_INFO* symbol;
    HANDLE process;

    process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);

    frames = CaptureStackBackTrace(0, max_frames, (PVOID*) stack, NULL);
    symbol = (SYMBOL_INFO*) calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char) , 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    LC_LogDef logger(log_tag, level);
    logger.stream() << std::endl;
    for (i = 0; i < frames; i++) {
        // intptr_t should be guaranteed to be able to keep a pointer on any
        // platform. Anyway... anything can happen on Win :-(
        DWORD64 addr = (DWORD64) (*(((intptr_t*) stack + i)));
        SymFromAddr(process, addr, 0, symbol);
        logger.stream() << (frames - i - 1) << ": "
                        << symbol->Name << " 0x" << (void*) symbol->Address << std::endl;
    }

    free(symbol);
    free(stack);
}
#endif // !defined(_WIN32) && !defined(_WIN32_WCE)

/*------------------------------------------------------------------------------
|    log_stacktrace
+-----------------------------------------------------------------------------*/
inline void log_stacktrace(LC_LogLevel level, unsigned int max_frames)
{
    log_stacktrace(LOG_TAG, level, max_frames);
}

/*------------------------------------------------------------------------------
|    log_stacktrace
+-----------------------------------------------------------------------------*/
inline void log_stacktrace(unsigned int max_frames)
{
    log_stacktrace(LOG_TAG, LC_LOG_DEBUG, max_frames);
}

/*------------------------------------------------------------------------------
|    log_stacktrace
+-----------------------------------------------------------------------------*/
inline void log_stacktrace(const char* log_tag, unsigned int max_frames)
{
    log_stacktrace(log_tag, LC_LOG_DEBUG, max_frames);
}
#endif // !defined(__ANDROID__) && (!defined(WINVER) || WINVER < 0x0602)

}

// Prevent from using outside.
#undef VA_LIST_CONTEXT
#undef LOG_UNUSED
#undef LC_LIKELY
#undef LC_UNLIKELY

#endif // LC_LOGGING_UTILS_H
