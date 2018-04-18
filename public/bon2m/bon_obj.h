#ifndef BON_OBJ_INC
#define BON_OBJ_INC

#include <stdint.h>
#include "version.h"
#include <string.h>

#define RetrieveBonField(bon_ptr, path, result, as_method) do{ if(!bon_ptr.IsNull() && bon_ptr->Has(path)){  \
          result = bon_ptr->Get(path)->as_method();    \
          } }while(0)

#define RetrieveBonField_s(bon_ptr, path, result, as_method) do{ assert(!bon_ptr.IsNull()); if(bon_ptr->Has(path)){  \
          result = bon_ptr->Get(path)->as_method();    \
          } }while(0

#define RetrieveBonField_nocheck(bon_ptr, path, result, as_method) result = bon_ptr->Get(path)->as_method()

#define RetrieveBonArrayElem(bon_ptr, path_format, index, result, as_method) do {  \
          char path[512];   \
          snprintf(path, sizeof(path), path_format, index);   \
          RetrieveBonField(bon_ptr, path, result, as_method); \
          } while (0)

#define SetBonArrayElem(bon_ptr, path_format, index, value) do {  \
          char path[512];   \
          snprintf(path, sizeof(path), path_format, index);   \
          bon_ptr->Set(path, value); \
          } while (0)

namespace IS_BON
{

    inline int32_t byte2int32(char * buf)
    {
        return *(int32_t *)buf;
    }

    inline void int32tobyte(int32_t v, char * buf)
    {
        *(int32_t *)buf = v;
    }

    inline uint32_t byte2uint32(char * buf)
    {
        return *(uint32_t *)buf;
    }

    inline void uint32tobyte(uint32_t v, char * buf)
    {
        *(uint32_t *)buf = v;
    }

    inline int64_t byte2int64(char * buf)
    {
        return *(int64_t *)buf;
    }

    inline void int64tobyte(int64_t v, char * buf)
    {
        *(int64_t *)buf = v;
    }

    inline uint64_t byte2uint64(char * buf)
    {
        return *(uint64_t *)buf;
    }

    inline void uint64tobyte(uint64_t v, char * buf)
    {
        *(uint64_t *)buf = v;
    }

    inline double byte2double(char * buf)
    {
        return *(double *)buf;
    }

    inline void double2byte(double v, char * buf)
    {
        *(double *)buf = v;
    }

    inline int bon_strcpy(char * dst, const char * src, int size)
    {
        char * pTmp = dst;
        char * pSrc = (char *) src;
        if (size <= 0)
        {
            return -1;
        }
        while ((*dst++ = *pSrc++) != 0)
        {
            --size;
            if(size == 0)
            {
                *dst = 0;
                break;
            }
        }
        return dst - pTmp -1;
    }


    class BonMemAllocator
    {
    public:
        virtual void * Alloc(int size) = 0;
        virtual void Free(void * p) = 0;
    };

    enum BONType
    {
        BYTE = 0x01,
        INT32,
        UINT32,
        INT64,
        UINT64,
        DOUBLE,
        CSTRING,
        BINARY,
        OBJECT,
        ARRAY,
        NIL = 100
    };

    class BonDict;
    class BonObj;
    class BonPtr;
    struct BonField;


    class BonObj
    {
    public:
        static int SetMemAllocator(BonMemAllocator * pAllocator);
        static BonPtr Allocate();
        static void * Malloc(uint32_t size);
        static void Free(void * p);
        static int GetLenFromBuff(const void * Buff, int Len);
        int Set(const char * Key, const BonPtr & pBon );
        int Set(const char * Key, char v)
        {
            return SetElem(Key, BYTE, &v, sizeof(v));
        }
        int Set(const char * Key, int32_t v)
        {
            return SetElem(Key, INT32, &v, sizeof(v));
        }
        int Set(const char * Key, uint32_t v)
        {
            return SetElem(Key, UINT32, &v, sizeof(v));
        }
        int Set(const char * Key, int64_t v)
        {
            return SetElem(Key, INT64, &v, sizeof(v));
        }
        int Set(const char * Key, uint64_t v)
        {
            return SetElem(Key, UINT64, &v, sizeof(v));
        }
        int Set(const char * Key, double v)
        {
            return SetElem(Key, DOUBLE, &v, sizeof(v));
        }
        int Set(const char * Key, const char * v)
        {
            if (Key == NULL || v == NULL)
            {
                return -__LINE__;
            }
            int Len = strlen(v) + 1;
            return SetElem(Key, CSTRING, v, Len);
        }
        int Set(const char * Key, const void * v, int Len)
        {
            return SetElem(Key, BINARY, v, Len);
        }
        BonPtr Get(const char * Key);
        void Remove(const char * Key);
        int ToString(char * Str, int Len, int Indent = 0);
        int Pack(const void * Buf, int Len);
        int Unpack(const void * Buf, int Len);
        BONType Type() { return m_Type; }
        bool Has(const char * Name);
        bool Has(const char ** NameArr, int Count, const char ** MissNameArr, int & MissCount);
        int GetFields(BonField * Fields, int & Count);
        BonPtr Clone();
        BonPtr DeepClone();
        int Count();
    public:
        char Byte() const { return m_Value.Byte; }
        void Byte(char val) { SetVal(BYTE, &val, sizeof(val)); }
        int32_t Int32() const { return m_Value.Int32; }
        void Int32(int32_t val) { SetVal(INT32, &val, sizeof(val)); }
        uint32_t UInt32() const { return m_Value.UInt32; }
        void UInt32(uint32_t val) { SetVal(UINT32, &val, sizeof(val)); }
        int64_t Int64() const { return m_Value.Int64; }
        void Int64(int64_t val) { SetVal(INT64, &val, sizeof(val)); }
        uint64_t UInt64() const { return m_Value.UInt64; }
        void UInt64(uint64_t val) { SetVal(UINT64, &val, sizeof(val)); }
        double Double() const { return m_Value.Double; }
        void Double(double val) { SetVal(DOUBLE, &val, sizeof(val)); }
        const char * CStr() const { return m_Value.CStr; }
        void CStr(const char * val) 
        { 
            if (val == NULL)
            {
                return;
            }
            int Len = strlen(val) + 1;
            SetVal(CSTRING, val, Len); 
        }
        const void * Binary(int & Len) { Len = m_Size; return m_Value.Binary; }
        void Binary(const void * val, int Len)
        {
            SetVal(BINARY, val, Len);
        }
        int64_t Integer();
        int SetVal( BONType Type, const void * v, int Len);
        int Copy(BonPtr pSrc);
        int DeepCopy(BonPtr pSrc);
        int Cmp(BonPtr Other);
    private:
        BonObj();
        ~BonObj();
        int InitDict();
        void IncRef() { m_RefCnt++; }
        void DecRef() { m_RefCnt--; }
        int SetElem(const char * Key, BONType Type, const void * v, int Len);
        static void Release(BonObj * pBon);
    private:
        int32_t m_RefCnt;
        BONType m_Type;
        int32_t m_Size;
        union
        {
            char Byte;
            int32_t Int32;
            uint32_t UInt32;
            int64_t Int64;
            uint64_t UInt64;
            double Double;
            const char * CStr;
            void * Binary;
        } m_Value;
        BonDict * m_Dict;
    private:
        static BonMemAllocator * pBonMemAllocator;
    friend class BonPtr;
    };

    class BonPtr
    {
    public:
        explicit BonPtr() : m_pObj(NULL){} // { m_pObj = NULL; }
        explicit BonPtr(BonObj * pObj)
        {
            m_pObj = pObj;
            IncRef();
        }
        BonPtr(const BonPtr & Bp)
        {
            m_pObj = Bp.m_pObj;
            IncRef();
        }
        BonPtr & operator=(const BonPtr & Bp)
        {
            Bp.IncRef();
            DecRef();
            m_pObj = Bp.m_pObj;
            return *this;
        }
        ~BonPtr()
        {
            DecRef();
        }
        BonObj * Get() const
        {
            return m_pObj;
        }
        void Reset()
        {
            DecRef();
        }
        bool IsNull() const { return m_pObj == NULL; }
        BonObj & operator *() const
        {
            return *m_pObj;
        }
        BonObj * operator->() const 
        {
            return m_pObj;
        }
        int RefCnt()
        {
            if (m_pObj)
            {
                return m_pObj->m_RefCnt;
            }
            return 0;
        }
        bool Unique()
        {
            return RefCnt() == 1;
        }
    public:
        static BonPtr null;
    private:
        void IncRef() const
        {
            if (m_pObj)
            {
                m_pObj->m_RefCnt ++;
            }
        }
        void DecRef()
        {
            if (m_pObj)
            {
                m_pObj->m_RefCnt --;
                if (m_pObj->m_RefCnt == 0)
                {
                    BonObj::Release(m_pObj);
                }
                m_pObj = NULL;
            }
        }
    private:
        BonObj * m_pObj;
    };

    inline bool operator==(BonPtr const & a, BonPtr const & b)
    {
        return a.Get() == b.Get();
    }
    //inline bool operator==(BonPtr const & a, const BonObj * b)
    //{
    //    return a.Get() == b;
    //}
    inline bool operator!=(BonPtr const & a, BonPtr const & b)
    {
        return a.Get() != b.Get();
    }
    //inline bool operator!=(BonPtr const & a, const BonObj * b)
    //{
    //    return a.Get() != b;
    //}
    struct BonField
    {
        const char * Name;
        BonPtr Value;
    };
}


#endif
