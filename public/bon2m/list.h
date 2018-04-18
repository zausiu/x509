/*
 * =====================================================================================
 *
 *       Filename:  list.h
 *
 *    Description:  list
 *
 *        Version:  1.0
 *        Created:  08/10/2012 03:17:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  cosineyu (gk), cosineyu@tencent.com
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  LIST_INC
#define  LIST_INC


namespace IS_BON
{


struct list_node
{
    struct list_node * prev;
    struct list_node * next;
    void * value;
};

class CBiList
{
public:
    CBiList() { m_head.prev = &m_head; m_head.next = &m_head; m_iSize = 0; }
    ~CBiList() {};
    inline struct list_node * Front() const { return m_head.next; }
    inline struct list_node * Back() const { return m_head.prev; }
    inline struct list_node * End() { return &m_head; }
    int PushBack(struct list_node * pNode);
    int Insert(struct list_node * Pos, struct list_node * pNode);
    int Remove(struct list_node * pNode);
    inline int Size() const { return m_iSize; }
    inline void Clear() { m_head.prev = &m_head; m_head.next = &m_head; m_iSize = 0; }
    int Copy(CBiList & Other);
private:
    struct list_node m_head;
    int m_iSize;
};
}

#endif   /* ----- #ifndef LIST_INC  ----- */
