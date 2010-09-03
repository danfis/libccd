/***
 * libgjk
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libgjk.
 *
 *  libgjk is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  libgjk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GJK_LIST_H__
#define __GJK_LIST_H__

#include <string.h>
#include <gjk/compiler.h>


struct _gjk_list_t {
    struct _gjk_list_t *next, *prev;
};
typedef struct _gjk_list_t gjk_list_t;



/**
 * Get the struct for this entry.
 * @ptr:	the &gjk_list_t pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define gjkListEntry(ptr, type, member) \
    gjk_container_of(ptr, type, member)

/**
 * Iterates over list.
 */
#define gjkListForEach(list, item) \
        for (item = (list)->next; \
             _gjk_prefetch((item)->next), item != (list); \
             item = (item)->next)

/**
 * Iterates over list safe against remove of list entry
 */
#define gjkListForEachSafe(list, item, tmp) \
	    for (item = (list)->next, tmp = (item)->next; \
             item != (list); \
		     item = tmp, tmp = (item)->next)

/**
 * Iterates over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define gjkListForEachEntry(head, pos, member)				\
	for (pos = gjkListEntry((head)->next, typeof(*pos), member);	\
	     _gjk_prefetch(pos->member.next), &pos->member != (head); 	\
	     pos = gjkListEntry(pos->member.next, typeof(*pos), member))

/**
 * Iterates over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define gjkListForEachEntrySafe(head, pos, n, member)			\
	for (pos = gjkListEntry((head)->next, typeof(*pos), member),	\
		 n = gjkListEntry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = gjkListEntry(n->member.next, typeof(*n), member))


/**
 * Initialize list.
 */
_gjk_inline void gjkListInit(gjk_list_t *l);

_gjk_inline gjk_list_t *gjkListNext(gjk_list_t *l);
_gjk_inline gjk_list_t *gjkListPrev(gjk_list_t *l);

/**
 * Returns true if list is empty.
 */
_gjk_inline int gjkListEmpty(const gjk_list_t *head);

/**
 * Appends item to end of the list l.
 */
_gjk_inline void gjkListAppend(gjk_list_t *l, gjk_list_t *item);

/**
 * Removes item from list.
 */
_gjk_inline void gjkListDel(gjk_list_t *item);



///
/// INLINES:
///

_gjk_inline void gjkListInit(gjk_list_t *l)
{
    l->next = l;
    l->prev = l;
}

_gjk_inline gjk_list_t *gjkListNext(gjk_list_t *l)
{
    return l->next;
}

_gjk_inline gjk_list_t *gjkListPrev(gjk_list_t *l)
{
    return l->prev;
}

_gjk_inline int gjkListEmpty(const gjk_list_t *head)
{
    return head->next == head;
}

_gjk_inline void gjkListAppend(gjk_list_t *l, gjk_list_t *new)
{
    new->prev = l->prev;
    new->next = l;
    l->prev->next = new;
    l->prev = new;
}

_gjk_inline void gjkListDel(gjk_list_t *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
    item->next = item;
    item->prev = item;
}

#endif /* __GJK_LIST_H__ */
