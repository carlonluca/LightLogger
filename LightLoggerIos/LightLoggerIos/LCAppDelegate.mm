/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    09.10.2013
 *
 * Copyright (c) 2013 Luca Carlon. All rights reserved.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
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
