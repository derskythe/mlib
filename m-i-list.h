/*
 * M*LIB - Intrusive List module
 *
 * Copyright (c) 2017, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __M_I_LIST_H
#define __M_I_LIST_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "m-core.h"

/* Interface to add to a structure to allow intrusive support.
   name: name of the intrusive list.
   type: name of the type of the structure (aka. struct test_s) - not used currently
*/
#define ILIST_INTERFACE(name, type)             \
  struct ilist_head_s name

/* Define a list of a given type.
   LIST_DEF(name, type [, oplist_of_the_type]) */
#define ILIST_DEF(name, ...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ILISTI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST, M_C(name, _t), M_C(name, _it_t) ), \
   ILISTI_DEF2(name, __VA_ARGS__,                   M_C(name, _t), M_C(name, _it_t) ))

/* Define the oplist of a ilist of type.
   USAGE: LIST_OPLIST(name [, oplist_of_the_type]) */
#define ILIST_OPLIST(...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (ILISTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                    \
   ILISTI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

/* Define the structure to be added in all objects. */
typedef struct ilist_head_s {
  struct ilist_head_s *next;
  struct ilist_head_s *prev;
} ilist_head_t;

/* Define the oplist of an ilist of type */
#define ILISTI_OPLIST(name,oplist)					\
  (INIT(M_C(name, _init)),						\
   CLEAR(M_C(name, _clear)),						\
   TYPE(M_C(name,_t)),							\
   SUBTYPE(M_C(name,_type_t)),						\
   IT_TYPE(M_C(name,_it_t)),						\
   IT_FIRST(M_C(name,_it)),						\
   IT_SET(M_C(name,_it_set)),						\
   IT_LAST(M_C(name,_it_last)),						\
   IT_END(M_C(name,_it_end)),						\
   IT_END_P(M_C(name,_end_p)),						\
   IT_LAST_P(M_C(name,_last_p)),					\
   IT_EQUAL_P(M_C(name,_it_equal_p)),					\
   IT_NEXT(M_C(name,_next)),						\
   IT_PREVIOUS(M_C(name,_previous)),					\
   IT_REF(M_C(name,_ref)),						\
   IT_CREF(M_C(name,_cref)),						\
   IT_REMOVE(M_C(name,_remove)),					\
   M_IF_METHOD(NEW, oplist)(IT_INSERT(M_C(name,_insert)),),		\
   OPLIST(oplist),                                                      \
   PUSH(M_C(name,_push_back)),						\
   POP(M_C(name,_pop_back))						\
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

#define ILISTI_CONTRACT(name, list) do {				\
    assert (list != NULL);						\
    assert(list->name.next->prev == &list->name);			\
    assert(list->name.prev->next == &list->name);			\
  } while (0)

#define ILISTI_DEF2(name, type, oplist, list_t, list_it_t)              \
									\
  typedef struct M_C(name, _s) {					\
    struct ilist_head_s name;                                           \
  } list_t[1];                                                          \
                                                                        \
  typedef type M_C(name, _type_t);					\
                                                                        \
  typedef struct M_C(name, _it_s) {					\
    struct ilist_head_s *head;                                          \
    struct ilist_head_s *previous;                                      \
    struct ilist_head_s *current;                                       \
    struct ilist_head_s *next;                                          \
  } list_it_t[1];                                                       \
  									\
  static inline void M_C(name, _init)(list_t list)			\
  {                                                                     \
    assert (list != NULL);                                              \
    list->name.next = &list->name;                                      \
    list->name.prev = &list->name;                                      \
    ILISTI_CONTRACT(name, list);					\
  }                                                                     \
                                                                        \
  static inline void M_C(name, _clean)(list_t list)			\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    for(struct ilist_head_s *it = list->name.next, *next ;		\
        it != &list->name; it = next) {					\
      type *obj = M_TYPE_FROM_FIELD(type, it,				\
				    struct ilist_head_s, name);		\
      /* Read next now before the object is destroyed */		\
      next = it->next;							\
      assert (next != NULL);						\
      M_GET_CLEAR oplist (*obj);					\
      M_IF_METHOD(DEL, oplist)(M_GET_DEL oplist (obj), (void) 0);	\
    }									\
    list->name.next = &list->name;                                      \
    list->name.prev = &list->name;                                      \
    ILISTI_CONTRACT(name, list);					\
  }                                                                     \
                                                                        \
  static inline void M_C(name, _clear)(list_t list)			\
  {                                                                     \
    M_C(name, _clean)(list);						\
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _empty_p)(const list_t list)		\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    return list->name.next == &list->name;                              \
  }                                                                     \
                                                                        \
  static inline size_t M_C(name, _size)(const list_t list)		\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    size_t s = 0;                                                       \
    for(const struct ilist_head_s *it = list->name.next ;               \
        it != &list->name; it = it->next)                               \
      s++;                                                              \
    return s;                                                           \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _push_back)(list_t list,	type *obj)	\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert (obj != NULL);						\
    struct ilist_head_s *prev = list->name.prev;                        \
    list->name.prev = &obj->name;                                       \
    obj->name.prev = prev;                                              \
    obj->name.next = &list->name;                                       \
    prev->next = &obj->name;                                            \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _push_front)(list_t list, type *obj)	\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert (obj != NULL);						\
    struct ilist_head_s *next = list->name.next;                        \
    list->name.next = &obj->name;                                       \
    obj->name.next = next;                                              \
    obj->name.prev = &list->name;                                       \
    next->prev = &obj->name;                                            \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _push_after)(type *obj_it, type *obj_ins) \
  {                                                                     \
    assert (obj_it != NULL && obj_ins != NULL);                         \
    struct ilist_head_s *next = obj_it->name.next;                      \
    obj_it->name.next = &obj_ins->name;                                 \
    obj_ins->name.next = next;                                          \
    obj_ins->name.prev = &obj_it->name;                                 \
    next->prev = &obj_ins->name;                                        \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _init_field)(type *obj)		        \
  {                                                                     \
    assert (obj != NULL);                                               \
    obj->name.next = NULL;                                              \
    obj->name.prev = NULL;                                              \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _unlink)(type *obj)			\
  {                                                                     \
    assert (obj != NULL);                                               \
    struct ilist_head_s *next = obj->name.next;                         \
    struct ilist_head_s *prev = obj->name.prev;                         \
    next->prev = prev;                                                  \
    prev->next = next;                                                  \
    /* Note: not really needed, but safer */                            \
    obj->name.next = NULL;                                              \
    obj->name.prev = NULL;                                              \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C(name, _back)(const list_t list)					\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert(!M_C(name, _empty_p)(list));					\
    return M_TYPE_FROM_FIELD(type, list->name.prev,                     \
                             struct ilist_head_s, name);                \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _front)(const list_t list)					\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert(!M_C(name, _empty_p)(list));					\
    return M_TYPE_FROM_FIELD(type, list->name.next,                     \
                             struct ilist_head_s, name);                \
  }                                                                     \
                                                                        \
  static inline type *							\
  M_C(name, _next_obj)(const list_t list, const type *obj)		\
  {									\
    ILISTI_CONTRACT(name, list);					\
    assert (obj != NULL);						\
    return obj->name.next == &list->name ? NULL :			\
      M_TYPE_FROM_FIELD(type, obj->name.next,				\
			struct ilist_head_s, name);			\
  }									\
									\
  static inline type *							\
  M_C(name, _previous_obj)(const list_t list, const type *obj)		\
  {									\
    ILISTI_CONTRACT(name, list);					\
    assert (obj != NULL);						\
    return obj->name.prev == &list->name ? NULL :			\
      M_TYPE_FROM_FIELD(type, obj->name.prev,				\
			struct ilist_head_s, name);			\
  }									\
									\
  static inline void                                                    \
  M_C(name, _it)(list_it_t it, list_t list)				\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert (it != NULL);						\
    it->head = &list->name;                                             \
    it->current = list->name.next;                                      \
    it->next = list->name.next->next;                                   \
    it->previous = &list->name;                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_set)(list_it_t it, const list_it_t cit)			\
  {                                                                     \
    assert (it != NULL && cit != NULL);                                 \
    it->head = cit->head;                                               \
    it->current = cit->current;                                         \
    it->next = cit->next;                                               \
    it->previous = cit->previous;                                       \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_last)(list_it_t it, list_t list)			\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert (it != NULL);						\
    it->head = &list->name;                                             \
    it->current = list->name.prev;                                      \
    it->next = &list->name;                                             \
    it->previous = list->name.prev->prev;                               \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_end)(list_it_t it, list_t list)				\
  {                                                                     \
    assert (it != NULL && list != NULL);                                \
    it->head = &list->name;                                             \
    it->current = &list->name;                                          \
    it->next = list->name.next;                                         \
    it->previous = list->name.prev;                                     \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _end_p)(const list_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == it->head;                                     \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _last_p)(const list_it_t it)				\
  {                                                                     \
    assert (it != NULL);                                                \
    return it->next == it->head || it->current == it->head;		\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _next)(list_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    /* Note: Can't set it->previous to it->current.                     \
       it->current may have been unlinked from the list */              \
    it->current  = it->next;                                            \
    assert (it->current != NULL);                                       \
    it->next     = it->current->next;                                   \
    it->previous = it->current->prev;                                   \
    assert (it->next != NULL && it->previous != NULL);                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _previous)(list_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    /* Note: Can't set it->next to it->current.                         \
       it->current may have been unlinked from the list */              \
    it->current  = it->previous;                                        \
    assert (it->current != NULL);                                       \
    it->next     = it->current->next;                                   \
    it->previous = it->current->prev;                                   \
    assert (it->next != NULL && it->previous != NULL);                  \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _it_equal_p)(const list_it_t it1, const list_it_t it2 )	\
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    /* No need to check for next & previous */                          \
    return it1->head == it2->head && it1->current == it2->current;      \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _ref)(const list_it_t it)					\
  {                                                                     \
    assert (it != NULL && it->current != NULL);                         \
    /* check if 'it' was not deleted */                                 \
    assert (it->current->next == it->next);                             \
    assert (it->current->prev == it->previous);                         \
    assert (!M_C(name, _end_p)(it));					\
    return M_TYPE_FROM_FIELD(type, it->current,                         \
			     struct ilist_head_s, name);		\
  }                                                                     \
  									\
  static inline const type *                                            \
  M_C(name, _cref)(const list_it_t it)					\
  {                                                                     \
    type *ptr = M_C(name, _ref)(it);					\
    return M_CONST_CAST(type, ptr);                                     \
  }                                                                     \
  									\
  static inline void							\
  M_C(name, _remove)(list_t list, list_it_t it)				\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    (void)list;	/* list param is not used */				\
    type *obj = M_TYPE_FROM_FIELD(type, it->current,			\
				  struct ilist_head_s, name);		\
    M_C(name, _unlink)(obj);						\
    M_GET_CLEAR oplist (obj);						\
    M_IF_METHOD(DEL, oplist)(M_GET_DEL oplist (obj), (void) 0);		\
    M_C(name, _next)(it);						\
  }									\
									\
  M_IF_METHOD(NEW, oplist)(						\
  static inline void							\
  M_C(name, _insert)(list_t list, list_it_t it, type x)			\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    type *p = M_GET_NEW oplist (type);					\
    if (M_UNLIKELY (p == NULL)) {					\
      M_MEMORY_FULL (sizeof (type));					\
      return ;								\
    }									\
    M_GET_INIT_SET oplist (*p, x);					\
    type *obj = M_C(name, _ref)(it);					\
    M_C(name, _push_after)(obj, p);					\
    (void) list;							\
    ILISTI_CONTRACT(name, list);					\
  }									\
  , /* NEW not defined */)						\
									\
  static inline type *                                                  \
  M_C(name, _pop_back)(list_t list)					\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert (!M_C(name, _empty_p)(list));				\
    type *obj = M_C(name, _back)(list);					\
    list->name.prev = list->name.prev->prev;                            \
    list->name.prev->next = &list->name;                                \
    return obj;                                                         \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _pop_front)(list_t list)					\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					\
    assert (!M_C(name, _empty_p)(list));				\
    type *obj = M_C(name, _front)(list);				\
    list->name.next = list->name.next->next;                            \
    list->name.next->prev = &list->name;                                \
    return obj;                                                         \
  }                                                                     \
									\
  static inline void                                                    \
  M_C(name, _splice)(list_t list1, list_t list2)			\
  {                                                                     \
    ILISTI_CONTRACT(name, list1);					\
    ILISTI_CONTRACT(name, list2);					\
    struct ilist_head_s *midle1 = list1->name.prev;			\
    struct ilist_head_s *midle2 = list2->name.next;			\
    midle1->next = midle2;						\
    midle2->prev = midle1;						\
    list1->name.prev = list2->name.prev;				\
    list2->name.prev->next = &list1->name;				\
    list2->name.next = &list2->name;					\
    list2->name.prev = &list2->name;					\
    ILISTI_CONTRACT(name, list1);					\
    ILISTI_CONTRACT(name, list2);					\
  }									\
									\
  static inline void                                                    \
  M_C(name, _splice_back)(list_t nv, list_t ov, list_it_t it)		\
  {                                                                     \
    ILISTI_CONTRACT(name, nv);						\
    ILISTI_CONTRACT(name, ov);						\
    assert (it != NULL);						\
    (void) ov;								\
    type *obj = M_C(name, _ref)(it);					\
    M_C(name, _unlink)(obj);						\
    M_C(name, _push_back)(nv, obj);					\
    ILISTI_CONTRACT(name, nv);						\
    ILISTI_CONTRACT(name, ov);						\
  }									\
									\
  static inline void							\
  M_C(name, _swap)(list_t d, list_t e)					\
  {									\
    ILISTI_CONTRACT(name, d);						\
    ILISTI_CONTRACT(name, e);						\
    M_SWAP(struct ilist_head_s *, d->name.next, e->name.next);		\
    M_SWAP(struct ilist_head_s *, d->name.prev, e->name.prev);		\
    ILISTI_CONTRACT(name, d);						\
    ILISTI_CONTRACT(name, e);						\
  }									\
  
#endif
