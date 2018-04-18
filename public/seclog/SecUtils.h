#ifndef _SEC_UTILS_HEADER_
#define _SEC_UTILS_HEADER_

#include <string.h>
#include <stdint.h>
#include <netinet/in.h>

namespace SEC
{

/**
 *  ͬ��safe_ntohll���޸�
 *  modified by robintang 2013-06-09
 */
inline uint64_t sec_ntohll(const uint64_t &ullIn)
{
    if (1 != ntohs(1))  
    {
        //���x86
        union 
    	{
    		uint64_t ddwVal;
    		uint32_t adwVal[2];
    	}uSrc, uDst;
    	
    	uSrc.ddwVal     = ullIn;
    	uDst.adwVal[0]  = htonl(uSrc.adwVal[1]);
    	uDst.adwVal[1]  = htonl(uSrc.adwVal[0]);
        
    	return uDst.ddwVal;     
    } 
    else    
    {
        return ullIn;         
    }
}

// strncpy ���������
// ע: ����C��� strncpy ���Bufferĩβȫ����0���������ܺ���, ������ʵ��
inline char *sec_strncpy(char *dest, const char *src, size_t n)
{
    if (n > 0)
    {
        size_t iLen = strlen(src);
        if (iLen >= n)
        {
            iLen = n - 1;
        }
        
        memcpy(dest, src, iLen);
        dest[iLen] = '\0';
    }
    
    return dest;
}

}



#endif

