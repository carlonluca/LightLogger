/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    05.12.2011
 *
 * Copyright (c) 2013, Luca Carlon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the author nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

/*------------------------------------------------------------------------------
 |    includes
 +-----------------------------------------------------------------------------*/
#ifndef __ANDROID__
#include <QGuiApplication>
#include <QElapsedTimer>
#if defined(QT_QML_LIB) && defined(QT_QUICK_LIB)
#include <QtQuick/QQuickView>
#include <QQmlEngine>
#endif
#endif

#include "lc_logging.h"

#ifdef __ANDROID__
#include <jni.h>

extern "C" void Java_com_luke_android_lightloggerandroid_MainActivity_log(JNIEnv* env, jobject obj)
{
   log_debug_t("LightLoggerAndroid", "Done! ;-)");
   log_warn_t("LightLoggerAndroid", "Warn log! ;-)");
}
#endif

#ifndef __ANDROID__
/*------------------------------------------------------------------------------
 |    test_func2
 +-----------------------------------------------------------------------------*/
void test_func2(int i)
{
   (void)i;

   log_stacktrace(lightlogger::LC_LOG_INFO, 90);
}

/*------------------------------------------------------------------------------
 |    test_func
 +-----------------------------------------------------------------------------*/
void test_func()
{
   test_func2(1);
}

/*------------------------------------------------------------------------------
|    test_args
+-----------------------------------------------------------------------------*/
void test_args(const char* format, ...)
{
   {
      va_list args;
      va_start(args, format);
      log_info_v(format, args);
      va_end(args);
   }

   {
      va_list args;
      va_start(args, format);
      log_info_t_v("MyTag", format, args);
      va_end(args);
   }
}
#endif

#ifndef __ANDROID__
/*------------------------------------------------------------------------------
 |    main
 +-----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
#if defined(QT_QML_LIB) && defined(QT_QUICK_LIB)
   QGuiApplication a(argc, argv);
#endif

#if 0
   QElapsedTimer timer;
   timer.start();

   // Test with lc_logging.
   for (int i = 0; i < 100000; i++)
      log_verbose("Test: %d.", i);
   qDebug("Result lc_logging: %lld.", timer.elapsed());

   // Test with printf.
   timer.restart();
   for (int i = 0; i < 100000; i++) {
      fprintf(stdout, "- DEBUG:%s: Test: %d.\n", lc_current_time().c_str(), i);
      fflush(stdout);
   }
   qDebug("Result printf: %lld.", timer.elapsed());

   // Test with qDebug.
   timer.restart();
   for (int i = 0; i < 100000; i++) {
      qDebug("- DEBUG:%s: Test: %d.\n", lc_current_time().c_str(), i);
      fflush(stderr);
   }
   qDebug("Result qDebug: %lld.", timer.elapsed());
#endif

#ifdef __GNUC__
   LOG_CRITICAL("MyTag", "Oooops!");
#endif

   log_info("Info log.");
   log_info_t("MyTag", "Info log.");
   test_args("Testing %d va_args functions.", 2);

   lightlogger::log_debug("Some message for debugging...");
   lightlogger::log_disabled("A disabled log!!!!!!!!!!! You won't see this.");

   log_critical("Print int: %d.", 5);
   log_critical_t("MyTag", "Print int: %d.", 5);
   log_critical_t("MyTag", "Print with tag only.");

   /*lc_formatted_printf(stdout, LC_LOG_ATTR_UNDERLINE, LC_LOG_COL_MAGENTA,
                     "Underlined %s! ;-)\n", "magenta");*/
   lightlogger::log_formatted(lightlogger::LC_LOG_ATTR_UNDERLINE, lightlogger::LC_FORG_COL_YELLOW, "Formatted text.");
   lightlogger::log_formatted(lightlogger::LC_FORG_COL_YELLOW, "Formatted text with %s.", "param");

#ifndef __ANDROID__
   test_func();
#endif

   // Using streams.
   {
      lightlogger::LC_LogDef logger(NULL, lightlogger::LC_LOG_ATTR_RESET, lightlogger::LC_FORG_COL_BLUE);
      logger.stream() << "Blue log using stream. " << "Params can be added like " << 1234 << ".";

      lightlogger::LC_LogDef l(NULL);
      Q_UNUSED(l);
   }

   {
       lightlogger::LC_LogDef logger(NULL, lightlogger::LC_LOG_ATTR_RESET, lightlogger::LC_FORG_COL_BLUE, lightlogger::LC_BACK_COL_MAGENTA);
       logger.stream() << "Blue text on magenta";
   }

   {
       lightlogger::LC_LogDef logger(NULL, lightlogger::LC_LOG_ATTR_RESET, lightlogger::LC_FORG_COL_BLUE, lightlogger::LC_BACK_BRIGHT_COL_MAGENTA);
       logger.stream() << "Blue text on bright magenta";
   }

   {
      lightlogger::LC_Log<lightlogger::LC_Output2Std> logger(lightlogger::LC_LOG_DEBUG);
      logger.stream() << "Debug log with stream.";
   }

   {
      lightlogger::LC_Log<lightlogger::LC_Output2Std> logger(lightlogger::LC_LOG_WARN);
      logger.stream() << "Warning log with stream.";
   }

   {
      lightlogger::LC_Log<lightlogger::LC_Output2Std> logger(lightlogger::LC_LOG_CRITICAL);
      logger.stream() << "Critical log with stream.";
   }

#if defined(QT_QML_LIB) && defined(QT_QUICK_LIB)
   QQuickView view;
   lightlogger::LC_QMLLogger::registerObject(view.rootContext());
   view.setSource(QUrl("qrc:///main.qml"));
#endif

   //assert(log_verbose("") == true);
   assert(log_info("") == true);
   assert(log_warn("") == false);
   assert(log_err("") == false);
   assert(log_critical("") == false);

   return 0;
}
#endif
