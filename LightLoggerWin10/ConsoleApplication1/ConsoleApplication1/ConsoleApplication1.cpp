// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#define COLORING_ENABLED
#define ENABLE_MSVS_OUTPUT
#include "../../../lc_logging.h"
lightlogger::custom_log_func lightlogger::global_log_func = log_to_stdout;

int main()
{
	lightlogger::log_info("App started! ;-)");
	lightlogger::log_info_t("LightLoggingWin8", "App started! ;-)");

	lightlogger::log_verbose("You shouldn't see this log!");

	lightlogger::log_warn("Nothing bad happened, just showing how a warning behaves... :-)");
}
