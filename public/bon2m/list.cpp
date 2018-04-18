/*
 * =====================================================================================
 *
 *       Filename:  list.cpp
 *
 *    Description:  bi list;
 *
 *        Version:  1.0
 *        Created:  08/29/2012 04:01:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  cosineyu (gk), cosineyu@tencent.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "list.h"

namespace IS_BON
{
int CBiList::Insert(struct list_node * Pos, struct list_node * pNode)
{
    if(pNode == &m_head)
    {
        return -1;
    }
    Pos->prev->next = pNode;
    pNode->prev = Pos->prev;
    pNode->next = Pos;
    Pos->prev = pNode;
    m_iSize += 1;
    return 0;
}

int CBiList::PushBack(struct list_node * pNode)
{
    return Insert(End(), pNode);
}

int CBiList::Remove(struct list_node * pNode)
{
    pNode->prev->next = pNode->next;
    pNode->next->prev = pNode->prev;
    pNode->next = 0;
    pNode->prev = 0;
    m_iSize -= 1;
    return 0;
}

int CBiList::Copy( CBiList & Other )
{
    m_head = Other.m_head;
    m_head.next->prev = &m_head;
    m_head.prev->next = &m_head;
    m_iSize = Other.m_iSize;
    return 0;
}
}
