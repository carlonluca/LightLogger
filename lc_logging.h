/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    05.12.2011
 *
 * Copyright (c) 2013-2015, Luca Carlon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the author nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Classes and structures for logging purposes. Tested on Android, iOS, Linux Embedded,
 * Mac OS X, Windows Vista/7/8, Linux.
 *
 * Available conf macros:
 * 1. COLORING_ENABLED: if defined it forces coloring. If not defined, it is defined or
 *    not according to the platform.
 * 2. ENABLE_LOG_*: if defined, it enables the specific log level.
 * 3. BUILD_LOG_LEVEL_*: if defined it enables all the levels with equal or higher
 *    priority:
 *    - BUILD_LOG_LEVEL_DEBUG;
 *    - BUILD_LOG_LEVEL_VERBOSE;
 *    - BUILD_LOG_LEVEL_INFORMATION;
 *    - BUILD_LOG_LEVEL_WARNING;
 *    - BUILD_LOG_LEVEL_ERROR;
 *    - BUILD_LOG_LEVEL_CRITICAL.
 * 4. BUILD_LOG_LEVEL_ALL: enables all the logs.
 * 5. XCODE_COLORING_ENABLED: Enables coloring with XCode coloring format. This also
 *    enables COLORING_ENABLED automatically.
 * 6. CUSTOM_LOG_FILE: path to the log file.
 * 7. ENABLE_CODE_LOCATION: prepends the location in the sources for all the logs.
 * 8. LOG_TAG: tag to be used when printing logs (on Android this is the tag used by
 *    logcat).
 *
 * Chaging default logger delegate
 * LC_LogDef is a typedef used in all the convenience functions. By default its value is
 * LC_Log<LC_Output2Std>. To change it to use one of the other delegates or to use a custom
 * defined delegate, just define the macro CUSTOM_LOGGER to the name of the custom
 * delegate to use *before* including this header. Creating a wrapper header may be a good
 * solution to do this in your entire sources.
 *
 * Version: 1.2.0
 */

#ifndef LC_LOGGING_H
#define LC_LOGGING_H

/*------------------------------------------------------------------------------
|    includes
+-----------------------------------------------------------------------------*/
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <set>
#if !defined(_WIN32) && !defined(_WIN32_WCE)
#include <libgen.h>
#endif
#if !defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__ANDROID__)
#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>
#elif defined(_WIN32) || defined(_WIN32_WCE)
#include <WinSock2.h>
#include <Windows.h>
#include <DbgHelp.h>
#include <datetimeapi.h>
#endif // WINVER<0x0602
#ifdef __ANDROID__
#include <android/log.h>
#else
#include <assert.h>
#endif // __ANDROID__

#ifdef QT_QML_LIB
#include <QObject>
#include <QQmlContext>
#endif // QT_QML_LIB

// Apple-specific portion
#if defined(__APPLE__) && (__OBJC__ == 1)
#include <Foundation/Foundation.h>
#endif

// Windows specific portion.
#if defined(_WIN32) || defined(_WIN32_WCE)
// __PRETTY_FUNCTION__ is a gcc extension not available on Microsoft
// compiler. I'll use the nearest approximation.
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define VA_LIST_CONTEXT(last, i) \
   {va_list args; va_start(args, last); i; va_end(args);}
#define LOG_UNUSED(x) \
   (void)x
#ifdef __GNUC__
#define LC_LIKELY(x) \
   __builtin_expect((x), 1)
#define LC_UNLIKELY(x) \
   __builtin_expect((x), 0)
#else
#define LC_LIKELY(x) (x)
#define LC_UNLIKELY(x) (x)
#endif // __GNUC__

// Coloring is automatically enabled if XCODE_COLORING_ENABLED is defined.
#ifdef XCODE_COLORING_ENABLED
#define COLORING_ENABLED
#endif

// Coloring is automatically enabled on some platforms.
#ifndef COLORING_ENABLED
#if __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
#define COLORING_ENABLED
#else
// Unsupported platform
#endif
#elif !defined(_WIN32) && !defined(_WIN32_WCE)
#define COLORING_ENABLED
#else
// Probably some Windows. You might want to enable coloring if you intend
// to run from cygwin or similar.
#endif // __APPLE__
#endif // COLORING_ENABLED

// Define a LOG_TAG macro project-widely or none is used.
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

// Enable logs according to log level.
#ifdef BUILD_LOG_LEVEL_DEBUG
#define BUILD_LOG_LEVEL_ALL
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
#define ENABLE_LOG_CRITICAL
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_WARNING
#define ENABLE_LOG_INFORMATION
#define ENABLE_LOG_VERBOSE
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
#define ENABLE_LOG_CRITICAL
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_WARNING
#define ENABLE_LOG_INFORMATION
#elif defined(BUILD_LOG_LEVEL_WARNING)
#define ENABLE_LOG_CRITICAL
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_WARNING
#elif defined(BUILD_LOG_LEVEL_ERROR)
#define ENABLE_LOG_CRITICAL
#define ENABLE_LOG_ERROR
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
#define ENABLE_LOG_CRITICAL
#else
// If none is selected select all.
#ifndef BUILD_LOG_LEVEL_ALL
#define BUILD_LOG_LEVEL_ALL
#endif
#endif // BUILD_LOG_LEVEL_DEBUG

#ifdef BUILD_LOG_LEVEL_ALL
#define ENABLE_LOG_CRITICAL
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_WARNING
#define ENABLE_LOG_INFORMATION
#define ENABLE_LOG_VERBOSE
#define ENABLE_LOG_DEBUG
#endif // BUILD_LOG_LEVEL_ALL

namespace lightlogger {

/*------------------------------------------------------------------------------
|    definitions
+-----------------------------------------------------------------------------*/
enum LC_LogLevel {
    // Do not mess with the order.
    LC_LOG_CRITICAL = 0,
    LC_LOG_ERROR = 1,
    LC_LOG_WARN = 2,
    LC_LOG_INFO = 3,
    LC_LOG_VERBOSE = 4,
    LC_LOG_DEBUG = 5,
    LC_LOG_NONE = 1000
};

// Text attributes.
#define C_RESET         0
#define C_BRIGHT        1 // Bold
#define C_DIM           2 // Faint
#define C_ITALIC        3
#define C_UNDERLINE     4 // Underscore
#define C_BLINK         5
#define C_RAPID_BLINK   6
#define C_REVERSE       7
#define C_HIDDEN        8
#define C_STRIKETHROUGH 9

enum LC_LogAttrib {
    LC_LOG_ATTR_RESET =         C_RESET,
    LC_LOG_ATTR_BRIGHT =        C_BRIGHT,
    LC_LOG_ATTR_DIM =           C_DIM,
    LC_LOG_ATTR_ITALIC =        C_ITALIC,
    LC_LOG_ATTR_UNDERLINE =     C_UNDERLINE,
    LC_LOG_ATTR_BLINK =         C_BLINK,
    LC_LOG_ATTR_RAPID_BLINK =   C_RAPID_BLINK,
    LC_LOG_ATTR_REVERSE =       C_REVERSE,
    LC_LOG_ATTR_HIDDEN =        C_HIDDEN,
    LC_LOG_ATTR_STRIKETHROUGH = C_STRIKETHROUGH
};

// Foreground colors.
#define LC_BLACK      0
#define LC_RED        1
#define LC_GREEN      2
#define LC_YELLOW     3
#define LC_BLUE       4
#define LC_MAGENTA    5
#define LC_CYAN       6
#define LC_WHITE      7
#define LC_DEFAULT    9

enum LC_LogColor {
    LC_FORG_COL_BLACK =          30 + LC_BLACK,
    LC_FORG_COL_RED =            30 + LC_RED,
    LC_FORG_COL_GREEN =          30 + LC_GREEN,
    LC_FORG_COL_YELLOW =         30 + LC_YELLOW,
    LC_FORG_COL_BLUE =           30 + LC_BLUE,
    LC_FORG_COL_MAGENTA =        30 + LC_MAGENTA,
    LC_FORG_COL_CYAN =           30 + LC_CYAN,
    LC_FORG_COL_WHITE =          30 + LC_WHITE,
    LC_FORG_BRIGHT_COL_BLACK =   90 + LC_BLACK,
    LC_FORG_BRIGHT_COL_RED =     90 + LC_RED,
    LC_FORG_BRIGHT_COL_GREEN =   90 + LC_GREEN,
    LC_FORG_BRIGHT_COL_YELLOW =  90 + LC_YELLOW,
    LC_FORG_BRIGHT_COL_BLUE =    90 + LC_BLUE,
    LC_FORG_BRIGHT_COL_MAGENTA = 90 + LC_MAGENTA,
    LC_FORG_BRIGHT_COL_CYAN =    90 + LC_CYAN,
    LC_FORG_BRIGHT_COL_WHITE =   90 + LC_WHITE,
    LC_FORG_COL_DEFAULT =        30 + LC_DEFAULT
};

enum LC_BackColor {
    LC_BACK_COL_BLACK =          40 + LC_BLACK,
    LC_BACK_COL_RED =            40 + LC_RED,
    LC_BACK_COL_GREEN =          40 + LC_GREEN,
    LC_BACK_COL_YELLOW =         40 + LC_YELLOW,
    LC_BACK_COL_BLUE =           40 + LC_BLUE,
    LC_BACK_COL_MAGENTA =        40 + LC_MAGENTA,
    LC_BACK_COL_CYAN =           40 + LC_CYAN,
    LC_BACK_COL_WHITE =          40 + LC_WHITE,
    LC_BACK_BRIGHT_COL_BLACK =   100 + LC_BLACK,
    LC_BACK_BRIGHT_COL_RED =     100 + LC_RED,
    LC_BACK_BRIGHT_COL_GREEN =   100 + LC_GREEN,
    LC_BACK_BRIGHT_COL_YELLOW =  100 + LC_YELLOW,
    LC_BACK_BRIGHT_COL_BLUE =    100 + LC_BLUE,
    LC_BACK_BRIGHT_COL_MAGENTA = 100 + LC_MAGENTA,
    LC_BACK_BRIGHT_COL_CYAN =    100 + LC_CYAN,
    LC_BACK_BRIGHT_COL_WHITE =   100 + LC_WHITE,
    LC_BACK_COL_DEFAULT =        40 + LC_DEFAULT
};

#ifdef XCODE_COLORING_ENABLED
/*------------------------------------------------------------------------------
 |    lc_xc_col
 +-----------------------------------------------------------------------------*/
inline const NSString* lc_xc_col(const int& index)
{
   // By using an inline function I ensure a single presence in memory of the
   // array.
   static const NSString* LC_XC_COL[] = {
      @"fg0,0,0;",
      @"fg255,0,0;",
      @"fg34,139,34;",
      @"fg255,215,0;",  // Gold should be more visible.
      @"fg0,0,255;",
      @"fg255,20,147;", // Magenta.
      @"fg0,255,255;",
      @"fg255,255,255;",
      @"fg0,0,0;"
   };

   return LC_XC_COL[index];
}
#endif // XCODE_COLORING_ENABLED

inline std::string lc_current_time();

/*------------------------------------------------------------------------------
|    lc_font_change
+-----------------------------------------------------------------------------*/
/**
* @brief lc_font_change Changes font attributes. These will be used for the text
* following this call.
* @param f The FILE* on which the color change should be written.
* @param attribs A set of attributes to enable.
* @param color A color.
* @param foreground The background color.
*/
inline void lc_font_change(FILE* f, std::set<LC_LogAttrib> attribs, LC_LogColor color, LC_BackColor foreground)
{
    fprintf(f, "%c[%d;%d", 0x1B, color, foreground);
    for (const LC_LogAttrib& attrib : attribs)
        fprintf(f, ";%d", attrib);
    fprintf(f, "m");
}

/*------------------------------------------------------------------------------
|    lc_font_change
+-----------------------------------------------------------------------------*/
/**
* @brief lc_font_change Changes font attributes. These will be used for the text
* following this call.
* @param f The FILE* on which the color change should be written.
* @param attrib An attribute.
* @param color A color.
* @param foreground The background color.
*/
inline void lc_font_change(FILE* f, LC_LogAttrib attrib, LC_LogColor color, LC_BackColor foreground)
{
    fprintf(f, "%c[%d;%dm%c[%dm", 0x1B, attrib, color, 0x1B, foreground);
}

/*------------------------------------------------------------------------------
|    lc_font_change
+-----------------------------------------------------------------------------*/
/**
* @brief lc_font_change Changes font attributes. These will be used for the text
* following this call.
* @param f The FILE* on which the color change should be written.
* @param attrib An attribute.
* @param color A color.
*/
inline void lc_font_change(FILE* f, LC_LogAttrib attrib, LC_LogColor color)
{
   fprintf(f, "%c[%d;%dm", 0x1B, attrib, color);
}

/*------------------------------------------------------------------------------
|    lc_font_reset
+-----------------------------------------------------------------------------*/
/**
* @brief lc_font_reset Resets font attributes and color to the default params.
* @param f The file descriptor to use.
*/
inline void lc_font_reset(FILE* f)
{
   fprintf(f, "%c[%dm", 0x1B, C_RESET);
}

/*------------------------------------------------------------------------------
|    lc_color_change
+-----------------------------------------------------------------------------*/
/**
* @brief lc_color_change Changes the color that will be used for the following text.
* @param f The file descriptor the color change will be written.
* @param color The color to be used.
*/
inline void lc_color_change(FILE* f, LC_LogColor color)
{
   lc_font_change(f, LC_LOG_ATTR_RESET, color);
}

/*------------------------------------------------------------------------------
|    lc_formatted_printf
+-----------------------------------------------------------------------------*/
/**
* @brief lc_formatted_printf printf-like function accepting attributes and color for
* the text being written.
* @param f The output file descriptor to use.
* @param attrib Attribute to use for the font.
* @param color Color to use for the font.
* @param format The text to write followed by the parameters.
*/
inline void lc_formatted_printf(FILE* f, LC_LogAttrib attrib, LC_LogColor color, LC_BackColor foreground, const char* format, ...)
{
   std::stringstream sink;
   sink << (char) 0x1B
      << "[" << (int) attrib << ";"
      << (int)color << "m";
   sink << (char) 0x1B
      << "[" << (int) attrib << ";"
      << (int)foreground << "m";
   sink << format;
   sink << (char) 0x1B
      << "[" << (int) LC_LOG_ATTR_RESET << "m";
   std::string final = sink.str();

   VA_LIST_CONTEXT(format, vfprintf(f, final.c_str(), args));
}

/*------------------------------------------------------------------------------
|    prepend_location
+-----------------------------------------------------------------------------*/
inline std::string prepend_location(const char* file, int line, const char* f, const char* format)
{
   std::stringstream ss;
#ifndef _WIN32
	ss << "[" << basename(const_cast<char*>(file));
#else
	static char fname[_MAX_FNAME];
	static char ext[_MAX_EXT];
	if (0 != _splitpath_s(file, NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT))
		ss << "[" << "-";
	else
		ss << "[" << fname << ext;
#endif // _WIN32

	ss << ":" << line << "/" << f << "] " << format;
   return ss.str();
}

#if defined(__APPLE__) && __OBJC__ == 1
/*------------------------------------------------------------------------------
|    prepend_location
+-----------------------------------------------------------------------------*/
inline std::string prepend_location(const char* file, int line, const char* f, NSString* format)
{
   std::stringstream ss;
   ss << "[" << basename(const_cast<char*>(file)) << ":";
   ss << line << "/" << f << "] " << [format cStringUsingEncoding:NSUTF8StringEncoding];
   return ss.str();
}
#endif // defined(__APPLE__) && __OBJC__ == 1

#define log_location_t_v(logfunc, tag, format, args) \
   (logfunc(tag, lightlogger::prepend_location(__FILE__, __LINE__, __FUNCTION__, format).data(), args))
#define log_location_t(logfunc, tag, format, ...) \
   (logfunc(tag, lightlogger::prepend_location(__FILE__, __LINE__, __FUNCTION__, format).data(), ##__VA_ARGS__))
#define log_location_v(logfunc, format, args) \
   (logfunc(lightlogger::prepend_location(__FILE__, __LINE__, __FUNCTION__, format).data(), args))
#define log_location(logfunc, format, ...) \
   (logfunc(lightlogger::prepend_location(__FILE__, __LINE__, __FUNCTION__, format).data(), ##__VA_ARGS__))

#ifdef ENABLE_CODE_LOCATION
#define FUNC(name) f_log_ ##name
#else
#define FUNC(name) log_ ##name
#endif // ENABLE_CODE_LOCATION

#define SHOW(name) show_ ##name

#define GENERATE_LEVEL(name, enumname, retval)                                          \
   inline bool FUNC(name ##_t_v)(const char* log_tag, const char* format, va_list args) \
   {                                                                                    \
      LC_Log(log_tag, enumname).printf(format, args);                                \
      return retval;                                                                    \
   }                                                                                    \
                                                                                        \
   inline bool FUNC(name ##_t)(const char* log_tag, const char* format, ...)            \
   {                                                                                    \
      VA_LIST_CONTEXT(format, LC_Log(log_tag, enumname).printf(format, args));       \
      return retval;                                                                    \
   }                                                                                    \
                                                                                        \
   inline bool FUNC(name ##_v)(const char* format, va_list args)                        \
   {                                                                                    \
      LC_Log(enumname).printf(format, args);                                         \
      return retval;                                                                    \
   }                                                                                    \
   inline bool FUNC(name)(const char* format, ...)                                      \
   {                                                                                    \
      VA_LIST_CONTEXT(format, LC_Log(enumname).printf(format, args));                \
      return retval;                                                                    \
   }                                                                                    \
   inline bool SHOW(name ##_t_v)(const char* log_tag, const char* format, va_list args) \
   {                                                                                    \
      LC_Log(log_tag, enumname, false).printf(format, args);                         \
      return retval;                                                                    \
   }                                                                                    \
                                                                                        \
   inline bool SHOW(name ##_t)(const char* log_tag, const char* format, ...)            \
   {                                                                                    \
      VA_LIST_CONTEXT(format, LC_Log(log_tag, enumname, false).printf(format, args));\
      return retval;                                                                    \
   }                                                                                    \
                                                                                        \
   inline bool SHOW(name ##_v)(const char* format, va_list args)                        \
   {                                                                                    \
      LC_Log(enumname, false).printf(format, args);                                  \
      return retval;                                                                    \
   }                                                                                    \
   inline bool SHOW(name)(const char* format, ...)                                      \
   {                                                                                    \
      VA_LIST_CONTEXT(format, LC_Log(enumname, false).printf(format, args));         \
      return retval;                                                                    \
   }

#if defined(__APPLE__) && __OBJC__ == 1
#define GENERATE_LEVEL_OBJC(name, enumname, retval)                                   \
   inline bool FUNC(name ##_t_v)(const char* log_tag, NSString* format, va_list args) \
   {                                                                                  \
      LC_Log(log_tag, enumname).printf(format, args);                              \
      return retval;                                                                  \
   }                                                                                  \
                                                                                      \
   inline bool FUNC(name ##_t)(const char* log_tag, NSString* format, ...)            \
   {                                                                                  \
      VA_LIST_CONTEXT(format, LC_Log(log_tag, enumname).printf(format, args));     \
      return retval;                                                                  \
   }                                                                                  \
                                                                                      \
   inline bool FUNC(name ##_v)(NSString* format, va_list args)                        \
   {                                                                                  \
      LC_Log(enumname).printf(format, args);                                       \
      return retval;                                                                  \
   }                                                                                  \
                                                                                      \
   inline bool FUNC(name)(NSString* format, ...)                                      \
   {                                                                                  \
      VA_LIST_CONTEXT(format, LC_Log(enumname).printf(format, args));              \
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

// Assertions.
#ifdef __ANDROID__
#define LOG_ASSERT(cond, text) \
   {if (!(cond)) __android_log_assert(0, LOG_TAG, text); }
#else
#define LOG_ASSERT(cond, text) assert(cond)
#endif

/*------------------------------------------------------------------------------
|    LC_NullStreamBuf class
+-----------------------------------------------------------------------------*/
/**
* @brief The LC_NullStreamBuf class implements a null sink for a char stream.
*/
class LC_NullStreamBuf : public std::basic_streambuf<char> {};

/*------------------------------------------------------------------------------
|    LC_NullStream class
+-----------------------------------------------------------------------------*/
/**
* @brief The LC_NullStream struct A type that can be used as a null stream. This should
* be optimized by the compiler.
*/
class LC_NullStream : public std::ostream {
public:
   LC_NullStream() : std::ostream(&buf) {}
private:
   LC_NullStreamBuf buf;
};

/*------------------------------------------------------------------------------
|    LC_LogPriv class
+-----------------------------------------------------------------------------*/
/**
* Internal class used for logging. This class is used to prepare the string that
* will be printed by some other delegate class.
*/
class LC_Log
{
public:
   LC_Log(LC_LogLevel level, bool nl = true);
   LC_Log(LC_LogColor color, bool nl = true);
   LC_Log(const char* log_tag, bool nl = true);
   LC_Log(const char* log_tag, LC_LogLevel level, bool nl = true);
   LC_Log(const char* log_tag, LC_LogAttrib attrib, LC_LogColor color, bool nl = true);
   LC_Log(const char* log_tag, LC_LogAttrib attrib, LC_LogColor color, LC_BackColor foreground, bool nl = true);

   ~LC_Log();

   std::ostream& stream();

   void printf(const char* format, ...);
   void printf(const char* format, va_list args);

#if defined(__APPLE__) && defined(__OBJC__)
   void printf(NSString* format, ...);
   void printf(NSString* format, va_list args);
#endif

   static std::string toString(LC_LogLevel level);
   static LC_LogLevel fromString(const std::string& level);

   void prependHeader(std::string& s);
   void prependLogTagIfNeeded(std::string& s);

   std::stringstream m_string;

   // If level is set, then attrib and color are not unless level
   // is none.
   LC_LogLevel m_level;
   const char* m_log_tag;
   LC_LogAttrib m_attrib;
   LC_LogColor m_color;
   LC_BackColor m_foreground;
   bool m_nl;

private:
   LC_Log(const LC_Log&);
   LC_Log& operator =(const LC_Log&) ;

   void initForLevel(const LC_LogLevel& level);

   std::ostringstream m_stream;
};

typedef void (*custom_log_func)(LC_Log&, va_list);
extern custom_log_func global_log_func;

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
   LC_Log(log_tag, a, c).printf(format, args);
   return true;
}

/*------------------------------------------------------------------------------
|    log_formatted_t
+-----------------------------------------------------------------------------*/
inline bool log_formatted_t(const char* log_tag, LC_LogAttrib a, LC_LogColor c, const char* format, ...)
{
   VA_LIST_CONTEXT(format, LC_Log(log_tag, a, c).printf(format, args));
   return true;
}

/*------------------------------------------------------------------------------
|    log_formatted_v
+-----------------------------------------------------------------------------*/
inline bool log_formatted_v(LC_LogAttrib a, LC_LogColor c, const char* format, va_list args)
{
   LC_Log(LOG_TAG, a, c).printf(format, args);
   return true;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted(LC_LogAttrib a, LC_LogColor c, const char* format, ...)
{
   VA_LIST_CONTEXT(format, LC_Log(LOG_TAG, a, c).printf(format, args));
   return true;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted(LC_LogColor c, const char* format, ...)
{
   VA_LIST_CONTEXT(format, LC_Log(LOG_TAG, LC_LOG_ATTR_RESET, c).printf(format, args));
   return true;
}

#if defined(__APPLE__) && __OBJC__ == 1
/*------------------------------------------------------------------------------
|    log_formatted_t
+-----------------------------------------------------------------------------*/
inline bool log_formatted_t(const char* log_tag, LC_LogAttrib a, LC_LogColor c, NSString* format, va_list args)
{
   LC_Log(log_tag, a, c).printf(format, args);
   return YES;
}

/*------------------------------------------------------------------------------
|    log_formatted
+-----------------------------------------------------------------------------*/
inline bool log_formatted_t(const char* log_tag, LC_LogAttrib a, LC_LogColor c, NSString* format, ...)
{
   VA_LIST_CONTEXT(format, LC_Log(log_tag, a, c).printf(format, args));
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

#if !defined(__ANDROID__)
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
      LC_Log(log_tag, level).printf("%s", stream.str().c_str());
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

   LC_Log(log_tag, level).printf("%s", stream.str().c_str());

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

   LC_Log logger(log_tag, level);
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

/*------------------------------------------------------------------------------
|    LC_Log::LC_Log
+-----------------------------------------------------------------------------*/
inline LC_Log::LC_Log(LC_LogColor color, bool nl) :
    m_level(LC_LOG_NONE)
  , m_log_tag(LOG_TAG)
  , m_attrib(LC_LOG_ATTR_RESET)
  , m_color(color)
  , m_nl(nl)
{
   // Do nothing.
}

/*------------------------------------------------------------------------------
|    LC_Log::LC_Log
+-----------------------------------------------------------------------------*/
inline LC_Log::LC_Log(const char* log_tag, LC_LogLevel level, bool nl) :
    m_level(level)
  , m_log_tag(log_tag)
  , m_nl(nl)
{
   // Do nothing.
}

/*------------------------------------------------------------------------------
|    LC_Log::LC_Log
+-----------------------------------------------------------------------------*/
inline LC_Log::LC_Log(const char *log_tag, bool nl) :
    m_level(LC_LOG_INFO)
  , m_log_tag(log_tag)
  , m_nl(nl)
{
   // Do nothing.
}

/*------------------------------------------------------------------------------
|    LC_Log::LC_Log
+-----------------------------------------------------------------------------*/
inline LC_Log::LC_Log(LC_LogLevel level, bool nl) :
    m_level(level)
  , m_log_tag(LOG_TAG)
  , m_nl(nl)
{
   // Do nothing.
}

/*------------------------------------------------------------------------------
|    LC_Log::LC_Log
+-----------------------------------------------------------------------------*/
inline LC_Log::LC_Log(const char* log_tag, LC_LogAttrib attrib, LC_LogColor color, bool nl) :
    m_level(LC_LOG_NONE)
  , m_log_tag(log_tag)
  , m_attrib(attrib)
  , m_color(color)
  , m_foreground(LC_BACK_COL_DEFAULT)
  , m_nl(nl)
{
    // Do nothing.
}

inline LC_Log::LC_Log(const char* log_tag, LC_LogAttrib attrib, LC_LogColor color, LC_BackColor foreground, bool nl) :
    m_level(LC_LOG_NONE)
  , m_log_tag(log_tag)
  , m_attrib(attrib)
  , m_color(color)
  , m_foreground(foreground)
  , m_nl(nl)
{
    // Do nothing.
}

/*------------------------------------------------------------------------------
|    LC_Log::appendHeader
+-----------------------------------------------------------------------------*/
inline void LC_Log::prependHeader(std::string& s)
{
   if (LC_LIKELY(m_level != LC_LOG_NONE))
      s.insert(0, toString(m_level) + ":\t ");
   s.insert(0, lc_current_time() + " ");
}

/*------------------------------------------------------------------------------
|    LC_Log::appendlog_tagIfNeeded
+-----------------------------------------------------------------------------*/
inline void LC_Log::prependLogTagIfNeeded(std::string& s)
{
   if (m_log_tag)
      s.insert(0, std::string("[") + std::string(m_log_tag) + std::string("]: "));
}

/*------------------------------------------------------------------------------
|    LC_Log::printf
+-----------------------------------------------------------------------------*/
inline void LC_Log::printf(const char* format, ...)
{
   if (LC_LIKELY(m_level != LC_LOG_NONE)) {
#ifdef BUILD_LOG_LEVEL_DEBUG
      // Always return a valid stream.
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
      if (m_level > LC_LOG_VERBOSE)
         return;
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
      if (m_level > LC_LOG_INFO)
         return;
#elif defined(BUILD_LOG_LEVEL_WARNING)
      if (m_level > LC_LOG_WARN)
         return;
#elif defined(BUILD_LOG_LEVEL_ERROR)
      if (m_level > LC_LOG_ERROR)
         return;
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
      if (m_level > LC_LOG_CRITICAL)
         return;
#endif
   }

   VA_LIST_CONTEXT(format, this->printf(format, args));
}

/*------------------------------------------------------------------------------
|    LC_Log::printf
+-----------------------------------------------------------------------------*/
inline void LC_Log::printf(const char* format, va_list args)
{
   if (LC_LIKELY(m_level != LC_LOG_NONE)) {
#ifdef BUILD_LOG_LEVEL_DEBUG
      // Always return a valid stream.
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
      if (m_level > LC_LOG_VERBOSE)
         return;
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
      if (m_level > LC_LOG_INFO)
         return;
#elif defined(BUILD_LOG_LEVEL_WARNING)
      if (m_level > LC_LOG_WARN)
         return;
#elif defined(BUILD_LOG_LEVEL_ERROR)
      if (m_level > LC_LOG_ERROR)
         return;
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
      if (m_level > LC_LOG_CRITICAL)
         return;
#endif
   }

   m_string << format;

   // Delegate log handling.
   if (global_log_func)
      global_log_func(*this, args);
}

#if defined(__APPLE__) && (__OBJC__ == 1)
/*------------------------------------------------------------------------------
|    LC_Log::printf
+-----------------------------------------------------------------------------*/
inline void LC_Log::printf(NSString* format, ...)
{
   if (LC_LIKELY(m_level != LC_LOG_NONE)) {
#ifdef BUILD_LOG_LEVEL_DEBUG
      // Always return a valid stream.
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
      if (m_level > LC_LOG_VERBOSE)
         return;
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
      if (m_level > LC_LOG_INFO)
         return;
#elif defined(BUILD_LOG_LEVEL_WARNING)
      if (m_level > LC_LOG_WARN)
         return;
#elif defined(BUILD_LOG_LEVEL_ERROR)
      if (m_level > LC_LOG_ERROR)
         return;
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
      if (m_level > LC_LOG_CRITICAL)
         return;
#endif
   }

   VA_LIST_CONTEXT(format, printf(format, args));
}

/*------------------------------------------------------------------------------
|    LC_Log::printf
+-----------------------------------------------------------------------------*/
inline void LC_Log::printf(NSString* format, va_list args)
{
   if (LC_LIKELY(m_level != LC_LOG_NONE)) {
#ifdef BUILD_LOG_LEVEL_DEBUG
      // Always return a valid stream.
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
      if (m_level > LC_LOG_VERBOSE)
         return;
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
      if (m_level > LC_LOG_INFO)
         return;
#elif defined(BUILD_LOG_LEVEL_WARNING)
      if (m_level > LC_LOG_WARN)
         return;
#elif defined(BUILD_LOG_LEVEL_ERROR)
      if (m_level > LC_LOG_ERROR)
         return;
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
      if (m_level > LC_LOG_CRITICAL)
         return;
#endif
   }

   // Build the NSString from the format. This includes NSString's in args.
   NSString* s1 = [NSString stringWithUTF8String : m_string.str().c_str()];
#if __has_feature(objc_arc)
   NSString* s2 = [[NSString alloc] initWithFormat:format arguments : args];
#else
   NSString* s2 = [[[NSString alloc] initWithFormat:format arguments : args] autorelease];
#endif // __has_feature(objc_arc)
   NSString* s = [NSString stringWithFormat : @"%@%@", s1, s2];

   printf([s cStringUsingEncoding : NSUTF8StringEncoding]);
}
#endif // defined(__APPLE__) && (__OBJC__ == 1)

/*------------------------------------------------------------------------------
|    LC_Log::~LC_Log
+-----------------------------------------------------------------------------*/
inline LC_Log::~LC_Log()
{
   if (LC_LIKELY(m_level != LC_LOG_NONE)) {
#ifdef BUILD_LOG_LEVEL_DEBUG
      // Always return a valid stream.
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
      if (m_level > LC_LOG_VERBOSE)
         return;
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
      if (m_level > LC_LOG_INFO)
         return;
#elif defined(BUILD_LOG_LEVEL_WARNING)
      if (m_level > LC_LOG_WARN)
         return;
#elif defined(BUILD_LOG_LEVEL_ERROR)
      if (m_level > LC_LOG_ERROR)
         return;
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
      if (m_level > LC_LOG_CRITICAL)
         return;
#endif
   }

   std::string s = m_stream.str();
   if (s.empty())
      return;
   printf(s.c_str());
}

/*------------------------------------------------------------------------------
|    LC_Log::stream
+-----------------------------------------------------------------------------*/
inline std::ostream& LC_Log::stream()
{
   static LC_NullStream nullStream;

   if (LC_LIKELY(m_level != LC_LOG_NONE)) {
#ifdef BUILD_LOG_LEVEL_DEBUG
      // Always return a valid stream.
#elif defined(BUILD_LOG_LEVEL_VERBOSE)
      if (m_level > LC_LOG_VERBOSE)
         return nullStream;
#elif defined(BUILD_LOG_LEVEL_INFORMATION)
      if (m_level > LC_LOG_INFO)
         return nullStream;
#elif defined(BUILD_LOG_LEVEL_WARNING)
      if (m_level > LC_LOG_WARN)
         return nullStream;
#elif defined(BUILD_LOG_LEVEL_ERROR)
      if (m_level > LC_LOG_ERROR)
         return nullStream;
#elif defined(BUILD_LOG_LEVEL_CRITICAL)
      if (m_level > LC_LOG_CRITICAL)
         return nullStream;
#endif
   }

   return m_stream;
}

/*------------------------------------------------------------------------------
|    LC_Log::toString
+-----------------------------------------------------------------------------*/
inline std::string LC_Log::toString(LC_LogLevel level)
{
   static const char* const buffer [] = {
       "CRIT",
       "ERR",
       "WARN",
       "INFO",
       "VERB",
       "DBG"
   };

   return buffer[level];
}

/*------------------------------------------------------------------------------
|    LC_Log::fromString
+-----------------------------------------------------------------------------*/
inline LC_LogLevel LC_Log::fromString(const std::string& level)
{
   if (level == "DEBUG")
      return LC_LOG_DEBUG;
   if (level == "VERBOSE")
      return LC_LOG_VERBOSE;
   if (level == "INFO")
      return LC_LOG_INFO;
   if (level == "WARNING")
      return LC_LOG_WARN;
   if (level == "ERROR")
      return LC_LOG_ERROR;
   if (level == "CRITICAL")
      return LC_LOG_CRITICAL;

   // TODO
   //LC_Log().prependHeader(LC_LOG_WARN)
   //   << "Unknown logging level '" << level << "'. Using INFO level as default.";
   return LC_LOG_INFO;
}

/*------------------------------------------------------------------------------
|    get_color_for_level
+-----------------------------------------------------------------------------*/
inline LC_LogColor get_color_for_level(LC_LogLevel level)
{
   static const LC_LogColor LC_COLOR_MAP [] = {
       LC_FORG_COL_RED,
       LC_FORG_COL_RED,
       LC_FORG_COL_YELLOW,
       LC_FORG_COL_GREEN,
       LC_FORG_COL_WHITE,
       LC_FORG_COL_BLUE
   };

   return LC_COLOR_MAP[level];
}

/*------------------------------------------------------------------------------
|    log_to_stdout
+-----------------------------------------------------------------------------*/
inline void log_to_stdout(LC_Log& logger, va_list args)
{
#ifdef COLORING_ENABLED
   LC_LogAttrib attrib;
   LC_LogColor  color;
   LC_BackColor foreground;
   if (LC_LIKELY(logger.m_level != LC_LOG_NONE)) {
      attrib = LC_LOG_ATTR_RESET;
      color = get_color_for_level(logger.m_level);
      foreground = LC_BACK_COL_DEFAULT;
   }
   else {
      attrib = logger.m_attrib;
      color = logger.m_color;
      foreground = logger.m_foreground;
   }

   std::ostringstream sink;
   if (logger.m_log_tag)
      sink << "[" << logger.m_log_tag << "]: ";
   if (LC_LIKELY(logger.m_level != LC_LOG_NONE))
      sink << logger.toString(logger.m_level) << ":\t";
   sink << lc_current_time() << " ";
   sink << (char) 0x1B
      << "[" << (int) attrib << ";"
      << (int)color << "m";
   sink << (char) 0x1B
      << "[" << (int)foreground << "m";
   sink << logger.m_string.str();
   sink << (char) 0x1B
      << "[" << (int) LC_LOG_ATTR_RESET << "m";
   if (LC_LIKELY(logger.m_nl))
       sink << std::endl;
   std::string final = sink.str();
#else
   std::stringstream s;
   s << logger.m_string.str();
   if (logger.m_nl)
      s << std::endl;
   std::string final = s.str();
#endif // COLORING_ENABLED

   // I prefer to flush to avoid missing buffered logs in case of crash.
   FILE* stdOut = stdout;
   if (logger.m_level == LC_LOG_ERROR || logger.m_level == LC_LOG_CRITICAL)
      stdOut = stderr;
   ::vfprintf(stdOut, final.c_str(), args);
   ::fflush(stdOut);
}

/*------------------------------------------------------------------------------
|    LC_Output2File::stream
+-----------------------------------------------------------------------------*/
inline FILE*& file_stream()
{
#ifdef _MSC_VER
   static FILE* pStream = NULL;
   if (!pStream)
      if (errno_t err = fopen_s(&pStream, "output.log", "a"))
         ::printf("Failed to open output.log: %d,", err);
#else
#ifndef CUSTOM_LOG_FILE
#define CUSTOM_LOG_FILE "output.log"
#endif
   static FILE* pStream = fopen("output.log", "a");
#endif // _MSC_VER
   return pStream;
}

/*------------------------------------------------------------------------------
|    LC_Output2File::output
+-----------------------------------------------------------------------------*/
inline void log_to_file(LC_Log& logger, va_list args)
{
   // Prepend.
   std::string final = logger.m_string.str();
   logger.prependHeader(final);
   logger.prependLogTagIfNeeded(final);
   final.append("\n");

   FILE* pStream = file_stream();
   if (!pStream)
      return;

   vfprintf(pStream, final.c_str(), args);
   fflush(pStream);
}

#ifdef ENABLE_MSVS_OUTPUT
#include <memory>
/*------------------------------------------------------------------------------
|    string_format
+-----------------------------------------------------------------------------*/
inline std::string string_format(const std::string fmt_str, va_list ap)
{
	size_t final_n, n = fmt_str.size() * 2;
	std::string str;
	std::unique_ptr<char[]> formatted;
	while (1) {
		formatted.reset(new char[n]);
		strcpy_s(&formatted[0], n, fmt_str.c_str());
		final_n = vsnprintf_s(&formatted[0], n, n - 1, fmt_str.c_str(), ap);
		if (final_n < 0 || final_n >= n)
			n += abs((long long)(final_n - n + 1));
		else
			break;
	}
	return std::string(formatted.get());
}

/*------------------------------------------------------------------------------
|    LC_Output2MSVS::printf
+-----------------------------------------------------------------------------*/
inline void log_to_msvs(LC_Log& logger, va_list args)
{
   // Prepend.
   std::string final = logger.m_string.str();
   logger.prependHeader(final);
   logger.prependLogTagIfNeeded(final);
   final.append("\n");

   OutputDebugStringA(string_format(final, args).c_str());
}
#endif // ENABLE_MSVS_OUTPUT

#ifdef __ANDROID__
/*------------------------------------------------------------------------------
|    LC_OutputAndroid::printf
+-----------------------------------------------------------------------------*/
inline void log_to_logcat(LC_Log& logger, va_list args)
{
   static const android_LogPriority android_logPriority [] = {
      // Do not mess with the order. Must map the LC_LogLevel enum.
      ANDROID_LOG_FATAL,
      ANDROID_LOG_ERROR,
      ANDROID_LOG_WARN,
      ANDROID_LOG_INFO,
      ANDROID_LOG_VERBOSE,
      ANDROID_LOG_DEBUG
   };

   android_LogPriority p = (logger.m_level > 5 || logger.m_level < 0) ? ANDROID_LOG_INFO : android_logPriority[logger.m_level];
   __android_log_vprint(
      p,
      logger.m_log_tag,
      logger.m_string.str().c_str(),
      args);
}
#endif // __ANDROID__

#ifdef XCODE_COLORING_ENABLED
/*------------------------------------------------------------------------------
|    LC_Output2XCodeColors::printf
+-----------------------------------------------------------------------------*/
inline void log_to_xcodecolors(LC_Log& logger, va_list args)
{
#define XC_COL_ESC_MAC @"\033["
#define XC_COL_ESC_IOS @"\xC2\xA0["

#define XC_COL_ESC XC_COL_ESC_MAC

#define XC_COL_RESET_FG XC_COL_ESC @"fg;" // Clear any foreground color
#define XC_COL_RESET_BG XC_COL_ESC @"bg;" // Clear any background color
#define XC_COL_RESET XC_COL_ESC @";"      // Clear any foreground or background color

   std::string final = logger.m_string.str();
   logger.prependHeader(final);
   logger.prependLogTagIfNeeded(final);

   const NSString* colorCode;
   if (logger.m_level == LC_LOG_NONE)
      colorCode = lc_xc_col((int)logger.m_color);
   else
      colorCode = getColorForLevel(logger.m_level);

   NSString* s;
#ifdef XCODE_COLORING_ENABLED
   //if (checkEnv())
      s = [NSString stringWithFormat : XC_COL_ESC @"%@%s" XC_COL_RESET @"\n", colorCode, final.c_str()];
#else
   //else
      s = [NSString stringWithFormat : @"%s\n", final.c_str()];
#endif // XCODE_COLORING_ENABLED

   ::printf("%s", [s cStringUsingEncoding : NSUTF8StringEncoding]);
}

/*------------------------------------------------------------------------------
|    LC_Output2XCodeColors::checkEnv
+-----------------------------------------------------------------------------*/
inline bool LC_Output2XCodeColors::checkEnv()
{
   static bool checkPerformed = false;
   static bool available = true;
   if (checkPerformed)
      return available;

#define XCODE_COLORS "XcodeColors"

   char* xcode_colors = getenv(XCODE_COLORS);
   if (!(xcode_colors && (strcmp(xcode_colors, "YES") == 0))) {
      NSLog(@"XCodeColors not available.");
      checkPerformed = true;
      available = false;
   }

   return available;
}

/*------------------------------------------------------------------------------
|    LC_OutputXCodeColors::getColorForLevel
+-----------------------------------------------------------------------------*/
inline NSString* LC_Output2XCodeColors::getColorForLevel(LC_LogLevel level)
{
   switch (level) {
   case LC_LOG_VERBOSE:
      return @"fg0,0,0;";   // Black here. XCode is commonly on light bg.
   case LC_LOG_DEBUG:
      return @"fg0,0,255;";
   case LC_LOG_INFO:
      return @"fg34,139,34;";
   case LC_LOG_NONE:
      return @"fg0,0,0;";
   case LC_LOG_WARN:
      return @"fg255,165,0;";
   case LC_LOG_ERROR:
      return @"fg255,0,0;";
   case LC_LOG_CRITICAL:
      return @"fg255,0,0;";
   default:
      return @"fg0,0,255;";
   }

   return nil;
}
#endif // XCODE_COLORING_ENABLED

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
inline int gettimeofday(struct ::timeval * tp, struct timezone * tzp)
{
    (void)tzp;

    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#else
#include <sys/time.h>
#endif // WIN32

/*------------------------------------------------------------------------------
|    lc_current_time
+-----------------------------------------------------------------------------*/
inline std::string lc_current_time()
{
   char buffer[11];
   time_t t;
   time(&t);

#ifdef WIN32
   struct tm timeinfo;
   localtime_s(&timeinfo, &t);
   strftime(buffer, sizeof(buffer), "%T", &timeinfo);
#else
   struct tm* timeinfo = localtime(&t);
   strftime(buffer, sizeof(buffer), "%T", timeinfo);
#endif


   struct timeval tv;
   gettimeofday(&tv, 0);

   char result[100] = { 0 };
   std::snprintf(result, 100, "%s.%03ld", buffer, (long) tv.tv_usec / 1000);

   return result;
}

#ifdef QT_CORE_LIB
#include <QtGlobal>
#include <QString>

/*------------------------------------------------------------------------------
|    log_handler
+-----------------------------------------------------------------------------*/
/**
 * @brief log_handler Function to handle Qt debugging output.
 * @param type
 * @param msg
 */
inline
void log_handler(QtMsgType type, const QMessageLogContext&, const QString& s)
{
	switch (type) {
	case QtDebugMsg:
        log_verbose(qPrintable(s));
		break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
	case QtInfoMsg:
        log_info(qPrintable(s));
		break;
#endif
	case QtWarningMsg:
        log_warn(qPrintable(s));
		break;
	case QtCriticalMsg:
        log_err(qPrintable(s));
		break;
	case QtFatalMsg:
        log_critical(qPrintable(s));
		break;
	}
}

/*------------------------------------------------------------------------------
|    log_handler_with_category
+-----------------------------------------------------------------------------*/
/**
 * @brief log_handler Function to handle Qt debugging output including the category.
 * @param type
 * @param msg
 */
inline
void log_handler_with_category(QtMsgType type, const QMessageLogContext& c, const QString& s)
{
    switch (type) {
    case QtDebugMsg:
        log_verbose_t(c.category, qPrintable(s));
        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
        log_info_t(c.category, qPrintable(s));
        break;
#endif
    case QtWarningMsg:
        log_warn_t(c.category, qPrintable(s));
        break;
    case QtCriticalMsg:
        log_err_t(c.category, qPrintable(s));
        break;
    case QtFatalMsg:
        log_critical_t(c.category, qPrintable(s));
        break;
    }
}

#endif // QT_CORE_LIB

#ifdef QT_QML_LIB
/*------------------------------------------------------------------------------
|    LC_QMLLogger
+-----------------------------------------------------------------------------*/
class LC_QMLLogger : public QObject
{
   Q_OBJECT
public:
   static LC_QMLLogger& instance() {
      static LC_QMLLogger instance;
      return instance;
   }

   Q_INVOKABLE void debug(QString s) const {
      FUNC(debug)(qPrintable(s));
   }

   Q_INVOKABLE bool verbose(QString s) const {
      return FUNC(verbose)(qPrintable(s));
   }

   Q_INVOKABLE bool info(QString s) const {
      return FUNC(info)(qPrintable(s));
   }

   Q_INVOKABLE bool warn(QString s) const {
      return FUNC(warn)(qPrintable(s));
   }

   Q_INVOKABLE bool error(QString s) const {
      return FUNC(err)(qPrintable(s));
   }

   Q_INVOKABLE bool critical(QString s) const {
      return FUNC(err)(qPrintable(s));
   }

   static void registerObject(QQmlContext* context) {
      context->setContextProperty(QStringLiteral("logger"), &(instance()));
   }

private:
   LC_QMLLogger() : QObject() {}
};
#endif // QT_QML_LIB

inline void log_to_default(LC_Log& logger, va_list args)
{
#ifdef __ANDROID__
   log_to_logcat(logger, args);
#elif defined(XCODE_COLORING_ENABLED)
   log_to_xcodecolors(logger, args);
#elif defined(ENABLE_MSVS_OUTPUT)
   log_to_msvs(logger, args);
#else
   log_to_stdout(logger, args);
#endif
}

}

// __cplusplus in VS2015 is still terribly old. So check for the damn
// VS compiler separately.
#if __cplusplus >= 201103L || _MSC_VER >= 1800
#include <mutex>
static std::once_flag log_info_once_flag;
#define log_info_once(...) \
	std::call_once(log_info_once_flag, []() { \
		log_info(__VA_ARGS__); \
	});
#endif

// Prevent from using outside.
#undef VA_LIST_CONTEXT
#undef LOG_UNUSED
#undef LC_LIKELY
#undef LC_UNLIKELY

#endif // LC_LOGGING_H
