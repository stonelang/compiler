#ifndef CLANG_BASIC_MACROS_H
#define CLANG_BASIC_MACROS_H

#ifndef CLANG_BUG_REPORT_URL
#define CLANG_BUG_REPORT_URL                                                   \
  "https://stonelang.org/contributing/#reporting-bugs"
#endif

#define CLANG_BUG_REPORT_MESSAGE_BASE                                          \
  "Submit a bug report (" CLANG_BUG_REPORT_URL ") and include the project"

#define CLANG_BUG_REPORT_MESSAGE "please " CLANG_BUG_REPORT_MESSAGE_BASE

#define CLANG_CRASH_BUG_REPORT_MESSAGE                                         \
  "Please " CLANG_BUG_REPORT_MESSAGE_BASE " and the crash backtrace."

#endif