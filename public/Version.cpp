/*
 * Version.cpp
 *
 *  Created on: Dec 10, 2015
 *  Author: kamuszhou@tencent.com
 *  website: wblog.ykyi.net 
 */

#include "Version.h"

const char VERSION[] =
{
    VERSION_MAJOR_INIT,
    '.',
    VERSION_MINOR_INIT,
    '-',
    BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
    '-',
    BUILD_MONTH_CH0, BUILD_MONTH_CH1,
    '-',
    BUILD_DAY_CH0, BUILD_DAY_CH1,
    'T',
    BUILD_HOUR_CH0, BUILD_HOUR_CH1,
    ':',
    BUILD_MIN_CH0, BUILD_MIN_CH1,
    ':',
    BUILD_SEC_CH0, BUILD_SEC_CH1,
    '\0'
};
