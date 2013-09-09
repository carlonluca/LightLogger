/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    09.10.2013
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

#define XCODE_COLORING_ENABLED
#define BUILD_LOG_LEVEL_WARNING

#import "LCAppDelegate.h"

#import "lc_logging.h"

@implementation LCAppDelegate

- (void)dealloc
{
   [_window release];
   [super dealloc];
}

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
   self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
   self.window.backgroundColor = [UIColor whiteColor];
   [self.window makeKeyAndVisible];
   
   // Test of log functions.
   log_debug(@"Some debug log.");
   log_debug_t("MyTag", @"Some debug log with int: %d.", 1234);
   log_debug_t("MyTag", @"Some debug log with flaot: %f.", 123.4);
   
   log_info(@"Information...");
   log_info(@"This color seems pretty %s :-)", "gooooood");
   
   LC_Log<LC_Output2XCodeColors>(LC_LOG_INFO).printf("Test!");
   LC_Log<LC_Output2XCodeColors>(LC_LOG_VERBOSE).printf("Test!");
   
   log_warn("Some warn log with const char*.");
   log_warn(@"Some warn log with NSString*.");
   log_warn_t("MyTag", @"Some warn log with int: %d.", 1234);
   log_warn_t("MyTag", @"Some warn log with int: %f.", 123.4);
   
   log_err(@"Ooops! Severe error here!");
   log_critical_t("MyTag", @"Oh my god, someone screwed pretty badly :-D");
   
   // Test using colors directly.
   LC_LogXCodeColors(LC_LOG_COL_MAGENTA).printf("Some text in magenta.");
   
   {
      LC_LogDef logger(LC_LOG_COL_CYAN);
      logger.stream() << "Stream based cyan log.";
   }
   
   {
      LC_LogDef logger(LC_LOG_WARN);
      logger.stream() << "Warning stream based log.";
   }
   
   {
      LC_LogDef logger(LC_LOG_DEBUG);
      logger.stream() << "Debug stream based log.";
   }
   
   return YES;
}

@end
