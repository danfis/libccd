/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libccddbl.
 *
 *  Distributed under the OSI-approved BSD License (the "License");
 *  see accompanying file BDS-LICENSE for details or see
 *  <http://www.opensource.org/licenses/bsd-license.php>.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the License for more information.
 */

#ifndef __CCDDBL_LIST_H__
#define __CCDDBL_LIST_H__

#include <string.h>
#include <ccddbl/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _ccddbl_list_t {
    struct _ccddbl_list_t *next, *prev;
};
typedef struct _ccddbl_list_t ccddbl_list_t;



/**
 * Get the struct for this entry.
 * @ptr:	the &ccddbl_list_t pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define ccddblListEntry(ptr, type, member) \
    ccddbl_container_of(ptr, type, member)

/**
 * Iterates over list.
 */
#define ccddblListForEach(list, item) \
        for (item = (list)->next; \
             _ccddbl_prefetch((item)->next), item != (list); \
             item = (item)->next)

/**
 * Iterates over list safe against remove of list entry
 */
#define ccddblListForEachSafe(list, item, tmp) \
	    for (item = (list)->next, tmp = (item)->next; \
             item != (list); \
		     item = tmp, tmp = (item)->next)

/**
 * Iterates over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define ccddblListForEachEntry(head, pos, postype, member)                 \
	for (pos = ccddblListEntry((head)->next, postype, member);	\
	     _ccddbl_prefetch(pos->member.next), &pos->member != (head); 	\
	     pos = ccddblListEntry(pos->member.next, postype, member))

/**
 * Iterates over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define ccddblListForEachEntrySafe(head, pos, postype, n, ntype, member)         \
    for (pos = ccddblListEntry((head)->next, postype, member),             \
		 n = ccddblListEntry(pos->member.next, postype, member);	\
	     &pos->member != (head); 					\
	     pos = n, n = ccddblListEntry(n->member.next, ntype, member))


/**
 * Initialize list.
 */
_ccddbl_inline void ccddblListInit(ccddbl_list_t *l);

_ccddbl_inline ccddbl_list_t *ccddblListNext(ccddbl_list_t *l);
_ccddbl_inline ccddbl_list_t *ccddblListPrev(ccddbl_list_t *l);

/**
 * Returns true if list is empty.
 */
_ccddbl_inline int ccddblListEmpty(const ccddbl_list_t *head);

/**
 * Appends item to end of the list l.
 */
_ccddbl_inline void ccddblListAppend(ccddbl_list_t *l, ccddbl_list_t *item);

/**
 * Removes item from list.
 */
_ccddbl_inline void ccddblListDel(ccddbl_list_t *item);



///
/// INLINES:
///

_ccddbl_inline void ccddblListInit(ccddbl_list_t *l)
{
    l->next = l;
    l->prev = l;
}

_ccddbl_inline ccddbl_list_t *ccddblListNext(ccddbl_list_t *l)
{
    return l->next;
}

_ccddbl_inline ccddbl_list_t *ccddblListPrev(ccddbl_list_t *l)
{
    return l->prev;
}

_ccddbl_inline int ccddblListEmpty(const ccddbl_list_t *head)
{
    return head->next == head;
}

_ccddbl_inline void ccddblListAppend(ccddbl_list_t *l, ccddbl_list_t *new_item)
{
    new_item->prev = l->prev;
    new_item->next = l;
    l->prev->next = new_item;
    l->prev = new_item;
}

_ccddbl_inline void ccddblListDel(ccddbl_list_t *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
    item->next = item;
    item->prev = item;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_LIST_H__ */
