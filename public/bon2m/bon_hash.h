#ifndef BON_TYPE_INC
#define BON_TYPE_INC

#include "list.h"
#include <string.h>
#include <stdint.h>
#include "bon_obj.h"

namespace IS_BON
{
    struct HashEntry
    {
        uint64_t hash;
        uint32_t key_len;
        char key[64];
        BonPtr value;
        list_node node;
        bool used;
    };

    class BonDict
    {
    public:
        BonDict();
        ~BonDict();
        int Set(const char * key, const BonPtr & value);
        BonPtr & Get(const char * key);
        int Remove(const char * key);
        void Clear();
        const HashEntry * Front()
        {
            return (HashEntry *)(m_EntryList.Front()->value);
        }
        const HashEntry * Next(const HashEntry * pE)
        {
            if (pE == NULL || pE->node.next == NULL || pE->node.next == m_EntryList.End())
            {
                return NULL;
            }
            return (HashEntry *)(pE->node.next->value);
        }
        bool Has(const char * key)
        {
            return !Get(key).IsNull();
        }
        int Count() { return m_Used; }
    private:
        HashEntry * Lookup(const char * key, uint64_t Hash);
        void SetSize(int size)
        {
            m_Size = size;
            m_Mask = m_Size - 1;
        }
        int Resize(int MiniUsed);
    public:
        static uint64_t StringHash(const char * str);
    private:
        const static int SMALL_TABLE_SIZE = 64;
        HashEntry * m_Table;
        int m_Size;
        int m_Used;
        uint64_t m_Mask;
        HashEntry m_SmallTable[SMALL_TABLE_SIZE];
        CBiList m_EntryList;
        
    };
}

#endif
