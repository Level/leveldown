/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <leveldb/env.h>
#include <sys/time.h>

#include "database.h"

namespace leveldown {

class LevelDOWNLogger : public leveldb::Logger {
 private:
  Database *database;

 public:
  LevelDOWNLogger (Database *database) : database(database) { }

  virtual ~LevelDOWNLogger() {}

  virtual void Logv(const char* format, va_list ap) {
    int bufsize = 30000;
    char *base = new char[bufsize];
    char *p = base;
    char *limit = base + bufsize;
    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    const time_t seconds = now_tv.tv_sec;
    struct tm t;
    localtime_r(&seconds, &t);

    // print time
    p += strftime(p, 20, "%Y-%m-%d %H:%M:%S", &t);
    // add millisecond component to time and current thread id
    p += snprintf(
        p
      , limit - p
      , ".%03d %lu "
      , static_cast<int>(now_tv.tv_usec) / 1000
      , uv_thread_self()
    );

    // print the message
    if (p < limit) {
      va_list backup_ap;
      va_copy(backup_ap, ap);
      p += vsnprintf(p, limit - p, format, backup_ap);
      va_end(backup_ap);
    }

    // truncate to available space if necessary
    if (p >= limit)
      p = limit - 1;

    // remove newline if present
    if (p[-1] == '\n')
      p--;

    // end the string
    *p++ = '\0';

    assert(p <= limit);
    database->Log(base);
  }
};

}
