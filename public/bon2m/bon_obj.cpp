#include "bon_obj.h"
#include "bon_hash.h"
#include <new>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace IS_BON;

#define CHECK_MOVE_PTR(PTR1, PTR2, x) do {PTR1 += x; if(PTR1 > PTR2) return -0x1000;} while(0)

#ifdef _WIN32
#define snprintf _snprintf
#endif

#pragma pack(1)
struct BonHead
{
    char STX;
    int32_t iSize;
    char Reserve[8];
    int32_t iProtoVer;
    int32_t iElemCount;
};
#pragma pack()


BonPtr IS_BON::BonObj::Allocate()
{
    uint32_t MSize = sizeof(BonObj);
    void * pTmp = BonObj::Malloc(MSize);
    if (pTmp == NULL)
    {
        return BonPtr();
    }
    BonObj * pBon = new(pTmp) BonObj();
    return BonPtr(pBon);
}

void IS_BON::BonObj::Release( BonObj * pBon )
{
    if (pBon)
    {
        pBon->~BonObj();
        BonObj::Free(pBon);
    }
}

IS_BON::BonObj::BonObj()
{
    m_Dict = NULL;
    m_RefCnt = 0;
    m_Size = 0;
    m_Type = NIL;
    m_Dict = NULL;
}

IS_BON::BonObj::~BonObj()
{
    if (/*m_Type == OBJECT && */m_Dict != NULL)
    {
        m_Dict->~BonDict();
        BonObj::Free(m_Dict);
        m_Dict = NULL;
    }
    if ((m_Type == CSTRING || m_Type == BINARY) && m_Value.Binary != NULL)
    {
        BonObj::Free(m_Value.Binary);
    }
}


void * IS_BON::BonObj::Malloc( uint32_t size )
{
    return BonObj::pBonMemAllocator->Alloc(size);
}

void IS_BON::BonObj::Free( void * p)
{
    if (p != NULL)
    {
        BonObj::pBonMemAllocator->Free(p);
    }
}

int IS_BON::BonObj::Set( const char * Key, const BonPtr & pBon )
{
    if (Key == NULL || pBon.IsNull())
    {
        return -__LINE__;
    }
    if (m_Dict == NULL)
    {
        if (InitDict() < 0)
        {
            return -__LINE__;
        }
    }
    const char * p = strstr(Key, ".");
    if (p == NULL)
    {
        if(m_Dict->Set(Key, pBon) < 0)
        {
            return -__LINE__;
        }
        m_Type = OBJECT;
    }
    else
    {
        char Tmp[1024];
        bon_strcpy(Tmp, Key, sizeof(Tmp)); 
        const char * Key = strtok(Tmp, ".");
        const char * PreKey = NULL;
        BonPtr pObj(this);
        BonPtr pPreObj;
        while(1)
        {
            pPreObj = pObj;
            pObj = pObj->Get(Key);
            PreKey = Key;
            Key = strtok(NULL, ".");
            if (pObj.IsNull() || Key == NULL)
            {
                if (Key == NULL)
                {
                    pPreObj->Set(PreKey, pBon);
                    break;
                }
                else
                {
                    pObj = BonObj::Allocate();
                    pPreObj->Set(PreKey, pObj);
                }
            }

        }
    }
    return 0;
}


int IS_BON::BonObj::SetElem( const char * Key, BONType Type, const void * v, int Len )
{
    if (Key == NULL || v == NULL || Len < 0)
    {
        return -__LINE__;
    }

    BonPtr pBon = BonObj::Allocate();
    if (pBon.IsNull())
    {
        return -__LINE__;
    }
    if(pBon->SetVal(Type, v, Len) < 0)
    {
        return -__LINE__;
    }
    const char * p = strstr(Key, ".");
    if(Set(Key, pBon) < 0)
    {
        return -__LINE__;
    }
    return 0;
}


int IS_BON::BonObj::SetVal( BONType Type, const void * v, int Len )
{
    if (v == NULL || Len < 0)
    {
        return -__LINE__;
    }
    
    if (m_Type == BINARY || m_Type == CSTRING)
    {
        BonObj::Free(m_Value.Binary);
        m_Value.Binary = NULL;
    }
    if (m_Dict != NULL)
    {
        m_Dict->~BonDict();
        BonObj::Free(m_Dict);
        m_Dict = NULL;
    }
    if (Type == BINARY || Type == CSTRING)
    {
        m_Value.Binary = BonObj::Malloc(Len);
        if (m_Value.Binary == NULL)
        {
            return -__LINE__;
        }
        memcpy(m_Value.Binary, v, Len);
    }
    else
    {
        memcpy(&m_Value, v, Len);
    }
    m_Type = Type;
    m_Size = Len;
    return 0;
}

void IS_BON::BonObj::Remove( const char * Key )
{
    m_Dict->Remove(Key);
}

int IS_BON::BonObj::InitDict()
{
    if (m_Dict != NULL)
    {
        return -__LINE__;
    }
    uint32_t size = sizeof(*m_Dict);
    void * pTmp = BonObj::Malloc(size);
    if (pTmp == NULL)
    {
        return -__LINE__;
    }
    m_Dict = new(pTmp) BonDict();
    return 0;
}

BonPtr IS_BON::BonObj::Get( const char * Key )
{
    if (m_Type != OBJECT)
    {
        return BonPtr::null;
    }
    const char * p = strstr(Key, ".");
    if (p == NULL)
    {
        return m_Dict->Get(Key);
    }
    else
    {
        char Tmp[1024];
        bon_strcpy(Tmp, Key, sizeof(Tmp)); 
        const char * Key = strtok(Tmp, ".");
        BonPtr pObj(this);
        while(Key)
        {
            pObj = pObj->Get(Key);
            if (pObj.IsNull())
            {
                return BonPtr::null;
            }
            Key = strtok(NULL, ".");
        }
        return pObj;
    }
}

int IS_BON::BonObj::Pack( const void * Buf, int Len )
{
    if (Buf == NULL)
    {
        return -__LINE__;
    }

    char * pEnd = (char *)Buf + Len;
    char * pPreMove = (char *)Buf;
    char * pTmp = (char *)Buf;

    CHECK_MOVE_PTR(pPreMove, pEnd, sizeof(BonHead));
    BonHead * pHead = (BonHead *)Buf;
    pHead->STX = 0x02;
    int32tobyte(Count(), (char *)&pHead->iElemCount);
    int32tobyte(BON_PROTO_VERS, (char *)&(pHead->iProtoVer));
    pTmp = pPreMove;
    if (m_Type == OBJECT)
    {
        for(const HashEntry * pE = m_Dict->Front(); pE != NULL; pE = m_Dict->Next(pE))
        {
            uint32_t NameLen = pE->key_len;
            CHECK_MOVE_PTR(pPreMove, pEnd, sizeof(NameLen) + NameLen + 1);
            uint32tobyte(NameLen + 1, pTmp);
            pTmp += sizeof(NameLen);
            memcpy(pTmp, pE->key, NameLen + 1);
            pTmp = pPreMove;

            BonPtr pEntryObj = pE->value;
            if (pEntryObj->m_Type != OBJECT)
            {
                CHECK_MOVE_PTR(pPreMove, pEnd, sizeof(char) + sizeof(uint32_t) + pEntryObj->m_Size);
                *pTmp = (char)pEntryObj->m_Type;
                pTmp += sizeof(char);
                uint32tobyte((uint32_t)pEntryObj->m_Size, pTmp);
                pTmp += sizeof(uint32_t);
                if (pEntryObj->m_Type != BINARY && pEntryObj->m_Type != CSTRING)
                {
                    memcpy(pTmp, &pEntryObj->m_Value, pEntryObj->m_Size);
                }
                else
                {
                    memcpy(pTmp, pEntryObj->m_Value.Binary, pEntryObj->m_Size);
                }
                pTmp = pPreMove;
            }
            else
            {
                CHECK_MOVE_PTR(pPreMove, pEnd, sizeof(char) + sizeof(uint32_t));
                *pTmp = (char)pEntryObj->m_Type;
                pTmp += sizeof(char);
                char * pSubLen = pTmp;
                pTmp += sizeof(uint32_t);
                int SubLen = pE->value->Pack(pTmp, pEnd - pTmp);
                if (SubLen < 0)
                {
                    return -__LINE__;
                }
                uint32tobyte((uint32_t)SubLen, pSubLen);
                pPreMove += SubLen;
                pTmp = pPreMove;
            }
        }
    }
    
    if (pEnd - pTmp < 1)
    {
        return -__LINE__;
    }
    *pTmp = 0x03;
    pTmp += 1;
    pHead->iSize = pTmp - (char*)Buf;

    return pTmp - (char *)Buf;
}

int IS_BON::BonObj::Unpack( const void * Buf, int Len )
{
    if (Buf == NULL || Len < sizeof(BonHead))
    {
        return -__LINE__;
    }
    char * pTmp = (char *)Buf;
    BonHead * pHead = (BonHead *)pTmp;
    if (pHead->STX != 0x2)
    {
        return -__LINE__;
    }
    int Size = byte2int32((char *)&(pHead->iSize));
    int BonProtoVer = byte2int32((char *)&(pHead->iProtoVer));
    if (BonProtoVer != BON_PROTO_VERS)
    {
        return -__LINE__;
    }
    if (Size > Len)
    {
        return -__LINE__;
    }
    if (pTmp[Size - 1] != 0x3)
    {
        return -__LINE__;
    }

    int ElemCount = byte2int32((char *)&(pHead->iElemCount));
    pTmp += sizeof(BonHead);

    int Count = 0;
    while (pTmp <= (char *)Buf + Len && Count < ElemCount)
    {
        int NameLen = *(int *)pTmp;
        pTmp += sizeof(NameLen);
        char * Name = pTmp;
        pTmp += NameLen;

        char Type = (int32_t)*pTmp;
        pTmp += sizeof(Type);

        int ElemLen = byte2int32(pTmp);
        pTmp += sizeof(ElemLen);

        if (Type != OBJECT)
        {
            SetElem(Name, (BONType)Type, pTmp, ElemLen);
        }
        else
        {
            BonPtr pSubBon = BonObj::Allocate();
            if(pSubBon.IsNull())
            {
                return -__LINE__;
            }
            int SubLen = pSubBon->Unpack(pTmp, Len - (pTmp - (char *)Buf));
            if(SubLen < 0 || SubLen != ElemLen)
            {
                return -__LINE__;
            }
            Set(Name, pSubBon);
        }
        pTmp += ElemLen;
        Count ++;
    }

    if (*pTmp != 0x3)
    {
        return -__LINE__;
    }
    pTmp += 1;
    return pTmp - (char *)Buf;
}




int IS_BON::BonObj::ToString( char * Str, int Len, int Indent )
{
    if (Str == NULL)
    {
        return 0;
    }
    Str[0] = 0;
    int Offset = 0;
    switch (m_Type)
    {
    case INT32:
        {
            //int len = snprintf(Str + Offset, Len - Offset, "[int32]: %d %x\n", Int32(), Int32());
            int len = snprintf(Str + Offset, Len - Offset, "[int32]: %d\n", Int32());
            Offset += len;
            break;
        }
    case UINT32:
        {
//            int len = snprintf(Str + Offset, Len - Offset, "[uint32]: %u %x\n", UInt32(), UInt32());
            int len = snprintf(Str + Offset, Len - Offset, "[uint32]: %u\n", UInt32());
            Offset += len;
            break;
        }
    case INT64:
        {
            //int len = snprintf(Str + Offset, Len - Offset, "[int64]: %lld %llx\n", Int64(), Int64());
            int len = snprintf(Str + Offset, Len - Offset, "[int64]: %ld\n", Int64());
            Offset += len;
            break;
        }
    case UINT64:
        {
            //int len = snprintf(Str + Offset, Len - Offset, "[uint64]: %lld %llx\n", UInt64(), UInt64());
            int len = snprintf(Str + Offset, Len - Offset, "[uint64]: %lu\n", UInt64());
            Offset += len;
            break;
        }
    case DOUBLE:
        {
            int len = snprintf(Str + Offset, Len - Offset, "[double]: %f\n", Double());
            Offset += len;
            break;
        }
    case BYTE:
        {
//            int len = snprintf(Str + Offset, Len - Offset, "[byte]: %d %x\n", Byte(), Byte());
            int len = snprintf(Str + Offset, Len - Offset, "[byte]: %d \n", Byte());
            Offset += len;
            break;
        }
    case CSTRING:
        {
            int len = snprintf(Str + Offset, Len - Offset, "[cstr]: %s\n", CStr());
            Offset += len;
            break;
        }
    case BINARY:
        {
            int len = snprintf(Str + Offset, Len - Offset, "[bin]: ");
            Offset += len;
            unsigned char * pTmp = (unsigned char *)m_Value.Binary;
            for (int i = 0; i < m_Size; i++)
            {
                len = snprintf(Str + Offset, Len - Offset, "%02x", pTmp[i]);
                Offset += len;
                if (((i + 1) % 2) == 0)
                {
                    len = snprintf(Str + Offset, Len - Offset, " ");
                    Offset += len;
                }
            }
            len = snprintf(Str + Offset, Len - Offset, "\n");
            Offset += len;
            break;
        }
    case OBJECT:
        {
            int len = 0;
            if (Indent != 0)
            {
                len = snprintf(Str + Offset, Len - Offset, ":\n");
                Offset += len;
            }
            for(const HashEntry * pE = m_Dict->Front(); pE != NULL; pE = m_Dict->Next(pE))
            {
                char TmpPrefix[128] = {0};
                for (int n = 0; n < Indent; n++)
                {
                    strcat(TmpPrefix, "\t");
                }
                len = snprintf(Str + Offset, Len - Offset, strcat(TmpPrefix, "[%s]"), pE->key);
                Offset += len;
                len = pE->value->ToString(Str + Offset, Len - Offset, Indent + 1);
                Offset += len;
            }
            break;
        }
    }
    return Offset;
}

bool IS_BON::BonObj::Has( const char * Name )
{
    if (m_Type == OBJECT)
    {
        return !Get(Name).IsNull();
    }
    return false;
}

bool IS_BON::BonObj::Has( const char ** NameArr, int Count, const char ** MissNameArr, int & MissCount )
{
    bool Rc = true;
    int MissIdx = 0;
    for (int i = 0; i < Count; i++)
    {
        if (!Has(NameArr[i]))
        {

            if (MissCount > MissIdx)
            {
                MissNameArr[MissIdx ++] = NameArr[i];
            }
            Rc = false;
        }
    }
    MissCount = MissIdx;
    return Rc;
}

int IS_BON::BonObj::Count()
{
    if (m_Type != OBJECT)
    {
        return 0;
    }
    return m_Dict->Count();
}

int IS_BON::BonObj::GetFields( BonField * Fields, int & NameCount )
{
    if (m_Type != OBJECT)
    {
        NameCount = 0;
        return -__LINE__;
    }
    if ( NameCount < Count())
    {
        return -__LINE__;
    }
    int NameIdx = 0;
    for(const HashEntry * pE = m_Dict->Front(); pE != NULL; pE = m_Dict->Next(pE))
    {
        Fields[NameIdx].Name = pE->key;
        Fields[NameIdx].Value = pE->value;
        NameIdx ++;
    }
    NameCount = NameIdx;
    return 0;
}

int IS_BON::BonObj::SetMemAllocator( BonMemAllocator * pAllocator )
{
    if (pAllocator == NULL)
    {
        return -__LINE__;
    }
    BonObj::pBonMemAllocator = pAllocator;
    return 0;
}

int IS_BON::BonObj::GetLenFromBuff( const void * Buff, int Len )
{
    if (Len < sizeof(BonHead))
    {
        return 0;
    }
    return *(int32_t *)((char *)Buff + 1);
}

IS_BON::BonPtr IS_BON::BonObj::Clone()
{
    BonPtr pNewBon = BonObj::Allocate();
    if (!pNewBon.IsNull())
    {
        pNewBon->Copy(BonPtr(this));
    }
    return pNewBon;
}

IS_BON::BonPtr IS_BON::BonObj::DeepClone()
{
    BonPtr pNewBon = BonObj::Allocate();
    if (!pNewBon.IsNull())
    {
        pNewBon->DeepCopy(BonPtr(this));
    }
    return pNewBon;
}

int IS_BON::BonObj::Copy( BonPtr pSrc )
{
    if (!pSrc.IsNull())
    {
        if (pSrc->m_Type != OBJECT)
        {
            if (pSrc->m_Type == BINARY || pSrc->m_Type == CSTRING)
            {
                SetVal(pSrc->m_Type, pSrc->m_Value.Binary, pSrc->m_Size);
            }
            else
            {
                SetVal(pSrc->m_Type, &pSrc->m_Value, pSrc->m_Size);
            }
        }
        else
        {
            for(const HashEntry * pE = pSrc->m_Dict->Front(); pE != NULL; pE = pSrc->m_Dict->Next(pE))
            {
                Set(pE->key, pE->value);
            }
        }
    }
    return 0;
}

int IS_BON::BonObj::DeepCopy( BonPtr pSrc )
{
    if (pSrc.IsNull())
    {
        return -__LINE__;
    }
    if (pSrc->m_Type != OBJECT)
    {
        Copy(pSrc);
    }
    else
    {
        for(const HashEntry * pE = pSrc->m_Dict->Front(); pE != NULL; pE = pSrc->m_Dict->Next(pE))
        {
            BonPtr pNewElem = BonObj::Allocate();
            if (pNewElem.IsNull())
            {
                return -__LINE__;
            }
            pNewElem->DeepCopy(pE->value);
            Set(pE->key, pNewElem);
        }
    }
    return 0;
}

int IS_BON::BonObj::Cmp( BonPtr Other )
{
    if (m_Type == CSTRING && Other->m_Type == CSTRING)
    {
        return strcmp(CStr(), Other->CStr());
    }
    if (m_Type == BINARY && Other->m_Type == BINARY)
    {
        int MinLen = (m_Size < Other->m_Size) ? m_Size : Other->m_Size;
        return memcmp(m_Value.Binary, Other->m_Value.Binary, MinLen);
    }
    if ((m_Type == BYTE || m_Type == INT32 || m_Type == UINT32 || m_Type == INT64 || m_Type == UINT64)
        && (Other->m_Type == BYTE || Other->m_Type == INT32 || Other->m_Type == UINT32 || Other->m_Type == INT64 || Other->m_Type == UINT64))
    {
        int64_t v1 = Integer();
        int64_t v2 = Other->Integer();
        if (v1 == v2)
        {
            return 0;
        }
        else if (v1 < v2)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    if (m_Type == OBJECT && Other->m_Type == OBJECT)
    {
        return Count() - Other->Count();
    }
    return -1;
}

int64_t IS_BON::BonObj::Integer()
{
    switch(m_Type)
    {
    case BYTE:
        return Byte();
    case INT32:
        return Int32();
    case UINT32:
        return UInt32();
    case INT64:
        return Int64();
    case UINT64:
        return UInt64();
    case CSTRING:
#ifdef  _WIN32
#define atoll _atoi64
#endif
        return atoll(CStr());
    case OBJECT:
        return Count();
    default:
        return 0;
    }
}

class DefaultMemAllocator : public BonMemAllocator
{
public:
    DefaultMemAllocator()
    {
//        mem_allocator_init(&m_allocator, 128, 16);
    }
    virtual void * Alloc(int size)
    {
        //return mem_allocator_alloc(&m_allocator, size);
        return (void *) new(std::nothrow) char[size];
    }
    virtual void Free(void * p)
    {
        //mem_allocator_free(&m_allocator, p);
        delete [] (char *)p;
    }
private:
    //mem_allocator m_allocator;
};

static DefaultMemAllocator DefaultAllocator;

BonMemAllocator * IS_BON::BonObj::pBonMemAllocator = &DefaultAllocator;

IS_BON::BonPtr IS_BON::BonPtr::null;
