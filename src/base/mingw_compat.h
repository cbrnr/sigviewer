// © SigViewer developers
//
// License: GPL-3.0

#ifndef SIGVIEWER_MINGW_COMPAT_H
#define SIGVIEWER_MINGW_COMPAT_H

#ifdef __MINGW32__
// sys/types.h defines pid_t and off_t only while the legacy aliases are enabled.
#include <sys/types.h>
#define NO_OLDNAMES
#endif

#endif // SIGVIEWER_MINGW_COMPAT_H
