/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    2018.04.15
 *
 * Copyright (c) 2011-2018, Luca Carlon
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

#ifndef LC_LOGGING_H
#define LC_LOGGING_H

#include <sstream>

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

#ifdef QT_CORE_LIB
#include <QObject>
#include <QString>
#include <QDebug>
#ifdef QT_QML_LIB
#include <QQmlContext>
#endif // QT_QML_LIB
#endif // QT_CORE_LIB

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
#define C_RESET     0
#define C_BRIGHT    1 // Bold.
#define C_DIM       2
#define C_UNDERLINE 4 // Underscore
#define C_BLINK     5
#define C_REVERSE   7
#define C_HIDDEN    8

enum LC_LogAttrib {
   LC_LOG_ATTR_RESET = C_RESET,
   LC_LOG_ATTR_BRIGHT = C_BRIGHT,
   LC_LOG_ATTR_DIM = C_DIM,
   LC_LOG_ATTR_UNDERLINE = C_UNDERLINE,
   LC_LOG_ATTR_BLINK = C_BLINK,
   LC_LOG_ATTR_REVERSE = C_REVERSE,
   LC_LOG_ATTR_HIDDEN = C_HIDDEN
};

// Foreground colors.
#define C_F_BLACK     0
#define C_F_RED       1
#define C_F_GREEN     2
#define C_F_YELLOW    3
#define C_F_BLUE      4
#define C_F_MAGENTA   5
#define C_F_CYAN      6
#define C_F_WHITE     7
#define C_F_DEFAULT   9

enum LC_LogColor {
   LC_LOG_COL_BLACK = C_F_BLACK,
   LC_LOG_COL_RED = C_F_RED,
   LC_LOG_COL_GREEN = C_F_GREEN,
   LC_LOG_COL_YELLOW = C_F_YELLOW,
   LC_LOG_COL_BLUE = C_F_BLUE,
   LC_LOG_COL_MAGENTA = C_F_MAGENTA,
   LC_LOG_COL_CYAN = C_F_CYAN,
   LC_LOG_COL_WHITE = C_F_WHITE,
   LC_LOG_COL_DEFAULT = C_F_DEFAULT
};

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
template <typename T>
class LC_Log
{
public:
   LC_Log(LC_LogLevel level);
   LC_Log(LC_LogColor color);
   LC_Log(const char* log_tag);
   LC_Log(const char* log_tag, LC_LogLevel level);
   LC_Log(const char* log_tag, LC_LogAttrib attrib, LC_LogColor color);

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

private:
   LC_Log(const LC_Log&);
   LC_Log& operator =(const LC_Log&) ;

   void initForLevel(const LC_LogLevel& level);

   std::ostringstream m_stream;
};

/*------------------------------------------------------------------------------
|    LC_Output2Std class
+-----------------------------------------------------------------------------*/
class LC_Output2Std
{
public:
   static void printf(LC_Log<LC_Output2Std>& logger, va_list args);
   static LC_LogColor getColorForLevel(LC_LogLevel level);
};
typedef LC_Log<LC_Output2Std> LC_LogStd;

/*------------------------------------------------------------------------------
|    LC_Output2File class
+-----------------------------------------------------------------------------*/
class LC_Output2FILE
{
public:
   static void printf(LC_Log<LC_Output2FILE>& logger, va_list args);

private:
   static FILE*& stream();
};
typedef LC_Log<LC_Output2FILE> LC_LogFile;

#ifdef ENABLE_MSVS_OUTPUT
/*------------------------------------------------------------------------------
|    LC_Output2File class
+-----------------------------------------------------------------------------*/
/**
* Used to send debugging messages to Visual Studio instead of standard output.
*/
class LC_Output2MSVS
{
public:
   static void printf(LC_Log<LC_Output2MSVS>& logger, va_list args);
};
typedef LC_Log<LC_Output2MSVS> LC_LogMSVC;
#endif // ENABLE_MSVS_OUTPUT

#ifdef __ANDROID__
/*------------------------------------------------------------------------------
|    LC_OutputAndroid class
+-----------------------------------------------------------------------------*/
class LC_OutputAndroid
{
public:
   static void printf(LC_Log<LC_OutputAndroid>& logger, va_list args);
};
typedef LC_Log<LC_OutputAndroid> LC_LogAndroid;
#endif // __ANDROID__

#ifdef XCODE_COLORING_ENABLED
/*------------------------------------------------------------------------------
|    LC_OutputXCodeColors class
+-----------------------------------------------------------------------------*/
class LC_Output2XCodeColors
{
public:
   static void printf(LC_Log<LC_Output2XCodeColors>& logger, va_list args);
   static bool checkEnv();
   static NSString* getColorForLevel(LC_LogLevel level);
};
typedef LC_Log<LC_Output2XCodeColors> LC_LogXCodeColors;
#endif

#ifdef QT_CORE_LIB
/*------------------------------------------------------------------------------
|    log_handler
+-----------------------------------------------------------------------------*/
#if QT_VERSION >= 0x050000
extern void log_handler(QtMsgType type, const QMessageLogContext&, const QString& s);
#else
extern "C" void log_handler(QtMsgType type, const char* s);
#endif

#ifdef QT_QML_LIB
/*------------------------------------------------------------------------------
|    LC_QMLLogger
+-----------------------------------------------------------------------------*/
class LC_QMLLogger : public QObject
{
   Q_OBJECT
public:
   static LC_QMLLogger& instance();
   static void registerObject(QQmlContext* context);

   Q_INVOKABLE void debug(QString s) const;
   Q_INVOKABLE bool verbose(QString s) const;
   Q_INVOKABLE bool info(QString s) const;
   Q_INVOKABLE bool warn(QString s) const;
   Q_INVOKABLE bool error(QString s) const;
   Q_INVOKABLE bool critical(QString s) const;

private:
   LC_QMLLogger() : QObject() {}
   virtual ~LC_QMLLogger() {}
};
#endif // QT_QML_LIB
#endif // QT_CORE_LIB

}

#endif // LC_LOGGING
