/*
 * =====================================================================================
 *
 *       Filename:  bon_type.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2012 04:53:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  cosineyu (gk), cosineyu@tencent.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "bon_hash.h"
#include "bon_obj.h"
#include <string>
#include <stdio.h>
using namespace IS_BON;


int IS_BON::BonDict::Set( const char * key, const BonPtr & value)
{
    if (key == NULL || value.IsNull())
    {
        return -__LINE__;
    }

    uint64_t Hash = BonDict::StringHash(key);
    HashEntry * pEntry = Lookup(key, Hash);
    if (pEntry == NULL)
    {
        return -__LINE__;
    }
    if (pEntry->used)
    {
        pEntry->value = value;
    }
    else
    {
        pEntry->key_len = bon_strcpy(pEntry->key, key, sizeof(pEntry->key));
        if (pEntry->key_len < 0)
        {
            return -__LINE__;
        }
        pEntry->value = value;
        pEntry->hash = Hash;
        pEntry->used = true;
        pEntry->node.value = pEntry;
        m_Used ++;
        m_EntryList.PushBack(&pEntry->node);

        if (m_Used * 3 > m_Size * 2)
        {
            return Resize(4 * m_Used);
        }
    }
    
    return 0;
}

BonPtr & IS_BON::BonDict::Get( const char * key )
{
    if (key == NULL)
    {
        return BonPtr::null;
    }
    uint64_t Hash = BonDict::StringHash(key);
    HashEntry * pE = Lookup(key, Hash);
    if (pE == NULL)
    {
        return BonPtr::null;
    }
    return pE->value;
}

int IS_BON::BonDict::Remove( const char * key )
{
    if (key == NULL)
    {
        return -__LINE__;
    }
    uint64_t Hash = BonDict::StringHash(key);
    HashEntry * pE = Lookup(key, Hash);
    if (pE == NULL)
    {
        return -__LINE__;
    }
    if (pE->used)
    {
        pE->used = false;
        m_EntryList.Remove(&pE->node);
        pE->value = BonPtr();
        m_Used --;
    }
    return 0;
}

IS_BON::BonDict::BonDict()
{
    SetSize(sizeof(m_SmallTable) / sizeof(m_SmallTable[0]));
    memset(m_SmallTable, 0, sizeof(m_SmallTable));
    m_Used = 0;
    m_Table = m_SmallTable;
}

HashEntry * IS_BON::BonDict::Lookup( const char * Key, uint64_t Hash )
{
    HashEntry * pEntry = NULL;
    uint64_t Index = Hash & m_Mask;
    pEntry = &m_Table[Index];
    if (!pEntry->used || (pEntry->hash == Hash && strcmp(pEntry->key, Key) == 0))
    {
        return pEntry;
    }
    
    uint64_t Perturb = Hash;
    while (1)
    {
        Index = (Index << 2) + Index + Perturb + 1;
        pEntry = &m_Table[Index & m_Mask];
        if (!pEntry->used || (pEntry->hash == Hash && strcmp(pEntry->key, Key) == 0))
        {
            return pEntry;
        }
        Perturb >>= 5;
    }
    return NULL;
}

uint64_t IS_BON::BonDict::StringHash( const char * str)
{
    uint64_t Hash = 0;
    uint32_t Len = 0;
    const char * p = str;
    Hash = (*p) << 7;
    while (*p != 0)
    {
        Hash = (1000003 * Hash) ^ (*p);
        p ++;
    }
    Len = p - str;
    Hash = Hash ^ Len;
    return Hash;
}

IS_BON::BonDict::~BonDict()
{
    Clear();
    if (m_Table != NULL && m_Table != m_SmallTable)
    {
        BonObj::Free(m_Table);
    }
}

void IS_BON::BonDict::Clear()
{
    for (list_node * pNode = m_EntryList.Front(); pNode != m_EntryList.End(); pNode = pNode->next)
    {
        HashEntry * pE = (HashEntry *)pNode->value;

        if (pE->used)
        {
            pE->used = false;
            pE->value = BonPtr();
            m_Used --;
        }
    }
    m_EntryList.Clear();
}

int IS_BON::BonDict::Resize( int MiniUsed )
{
    int NewSize = 0;
    for (NewSize = SMALL_TABLE_SIZE; NewSize <= MiniUsed && NewSize > 0; )
    {
        NewSize <<= 1;
    }
    if (NewSize <= 0)
    {
        return -__LINE__;
    }
    
    HashEntry * OldTable = m_Table;
    CBiList OldList;
    OldList.Copy(m_EntryList);
    bool old_is_smalltable = m_Table == m_SmallTable;
    HashEntry * NewTable = (HashEntry *)BonObj::Malloc( NewSize * sizeof(HashEntry));
    if (NewTable == NULL)
    {
        return -__LINE__;
    }
    memset(NewTable, 0, NewSize * sizeof(HashEntry));
    m_Table = NewTable;
    SetSize(NewSize);
    m_Used = 0;
    m_EntryList.Clear();

    //插入到新表
    for (list_node * pNode = OldList.Front(); pNode != OldList.End(); pNode = pNode->next)
    {
        HashEntry * pOldEntry = (HashEntry *)pNode->value;
        uint64_t Hash = pOldEntry->hash;
        
        uint64_t Index = Hash & m_Mask;
        HashEntry * pNewEntry = &m_Table[Index];
        uint64_t Perturb = Hash;
        while (pNewEntry->used)
        {
            Index = (Index << 2) + Index + Perturb + 1;
            pNewEntry = &m_Table[Index & m_Mask];
            Perturb >>= 5;
        }
        
        pNewEntry->hash = Hash;
        memcpy(pNewEntry->key, pOldEntry->key, pOldEntry->key_len);
        pNewEntry->key_len = pOldEntry->key_len;
        pNewEntry->value = pOldEntry->value;
        pOldEntry->value.Reset();
        pNewEntry->used = true;
        pNewEntry->node.value = pNewEntry;
        m_EntryList.PushBack(&pNewEntry->node);
        m_Used ++;
    }

    if (!old_is_smalltable)
    {
        BonObj::Free(OldTable);
    }
    
    return 0;
}
