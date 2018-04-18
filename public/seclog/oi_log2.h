#ifndef _OI_LOG_2_H
#define _OI_LOG_2_H

#include <stdio.h>
#include <time.h>

namespace SEC{
namespace LOG{

typedef struct {
        FILE    *pLogFile;
        char    sBaseFileName[256];
        char    sLogFileName[256];
        int     iShiftType;             // 0 -> shift by size,  1 -> shift by LogCount, 2 -> shift by interval, 3 ->shift by day, 4 -> shift by hour, 5 -> shift by min
        int     iMaxLogNum;
        long    lMaxSize;
        long    lMaxCount;
        long    lLogCount;
        time_t  lLastShiftTime;
} LogFile;

enum
{
	SHIFT_BY_SIZE = 0,
	SHIFT_BY_LOGCOUNT,
	SHIFT_BY_INTERVAL,
	SHIFT_BY_DAY,
	SHIFT_BY_HOUR,
	SHIFT_BY_MINUTE
};

#define TIME_TO_SEC		1
#define TIME_TO_USEC	2

/*
iShiftType;     0 -> shift by size,  1 or default -> shift by LogCount, 
                                2 -> shift by interval, seconds > iMAX
                                3 ->shift by day, 4 -> shift by hour 5->shift by minute
*/
// init
long InitLogFile(LogFile* pstLogFile, char* sLogBaseName, long iShiftType, long iMaxLogNum, long iMAX);
// ilogtime = 1 精确到秒 =2 精确到微秒
int Log(LogFile* pstLogFile, bool bForceFlush, int iLogTime, char* sFormat, ...);
// added by sogu
void UnInitLog(LogFile* pstLogFile);


}
}

#endif
