/*
 * Copyright (c) 2013 Citrix Systems, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#define _GNU_SOURCE

#include "project.h"
#include <stdarg.h>
#include <dlfcn.h>
#include <execinfo.h>

void bt(void)
{
  void *ba[256];
  Dl_info info;
  int i;

  int n = backtrace (ba, sizeof (ba) / sizeof (ba[0]));
  if (!n)
    return;


  for (i = 0; i < n; ++i)
    {
      syslog (LOG_ERR, "libvgmch %d: %p", i, ba[i]);
      if (dladdr (ba[i], &info))
        {
          char *base, *offset;

          base = info.dli_saddr;
          offset = ba[i];

          syslog (LOG_ERR, "libvgmch (%s %s+0x%x)", info.dli_fname,
                  info.dli_sname, (unsigned int) (offset - base));

          syslog (LOG_ERR, "libvgmch backtrace: (%s %s+0x%x)", info.dli_fname,
                  info.dli_sname, (unsigned int) (offset - base));
        }

    }

}

void surfman_vmessage(surfman_loglvl level, const char *fmt, va_list ap)
{
  va_list ap2;
  int syslog_lvl = LOG_DEBUG;

  va_copy(ap2, ap);

  switch (level)
    {
      case SURFMAN_DEBUG:
        syslog_lvl = LOG_DEBUG;
        break;
      case SURFMAN_INFO:
        syslog_lvl = LOG_INFO;
        break;
      case SURFMAN_WARNING:
        syslog_lvl = LOG_WARNING;
        break;
      case SURFMAN_ERROR:
        syslog_lvl = LOG_ERR;
        break;
      case SURFMAN_FATAL:
        syslog_lvl = LOG_EMERG;
        break;
    }

  // TODO: Make that configurable (output logfile/syslog or not/stderr or not)
  vsyslog(syslog_lvl, fmt, ap);

  vfprintf(stderr, fmt, ap2);

  if (level == SURFMAN_FATAL)
    {
      bt();
      abort();
    }
}

void surfman_message(surfman_loglvl level, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  surfman_vmessage(level, fmt, ap);
  va_end(ap);
}

void *xcalloc (size_t n, size_t s)
{
  void *ret = calloc (n, s);
  if (!ret)
    surfman_fatal ("calloc failed");
  return ret;
}

void *xmalloc (size_t s)
{
  void *ret = malloc (s);
  if (!ret)
    surfman_fatal ("malloc failed");
  return ret;
}

void *xrealloc (void *p, size_t s)
{
  p = realloc (p, s);
  if (!p)
    surfman_fatal ("realloc failed");
  return p;
}

char *xstrdup (const char *s)
{
  char *ret = strdup (s);
  if (!ret)
    surfman_fatal ("strdup failed");
  return ret;
}

