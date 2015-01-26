/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    01.15.2015
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

/*------------------------------------------------------------------------------
 |    includes
 +-----------------------------------------------------------------------------*/
#include "lc_logging.h"
#include "lc_logging_swift.h"

/*------------------------------------------------------------------------------
 |    definitions
 +-----------------------------------------------------------------------------*/
#define VA_LIST_CONTEXT(last, i) \
   {va_list args; va_start(args, last); i; va_end(args); }

#ifdef __cplusplus
extern "C" {
#endif
   
   void log_debug_sw(const char* s) {log_debug(s);}
   BOOL log_verbose_sw(const char* s) {return log_verbose(s);}
   BOOL log_info_sw(const char* s) {return log_info(s);}
   BOOL log_warn_sw(const char* s) {return log_warn(s);}
   BOOL log_err_sw(const char* s) {return log_err(s);}
   BOOL log_critical_sw(const char* s) {return log_critical(s);}
   
#ifdef __cplusplus
}
#endif
