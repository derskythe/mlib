// Header for coverage purpose
// Expand here all code that shall not be measured in coverage.
// Since the functions are not declared as static inline, their coverage is measured
// Also include helper container that should not be measured in coverage.
#if SHARED_PTR_HEADER == 1

SHARED_PTR_DEF_EXTERN_AS(shared_int, SharedInt, int, M_BASIC_OPLIST)
SHARED_PTR_DEF_EXTERN_AS(shared_double, SharedDouble, double, M_OPEXTEND(M_BASIC_OPLIST, TYPE(double)))
SHARED_PTR_DEF_EXTERN(shared_array, array_t)
SHARED_PTR_DEF_EXTERN(shared_array_str, array_str_t, ARRAY_OPLIST(array_str, M_STRING_OPLIST))

#elif SHARED_PTR_HEADER == 2

// TEST WITH ARRAY
ARRAY_DEF(array, int)
// Provide a FULL_P operator
static inline bool array_full_p(const array_t a) { return array_size(a) >= 10; }
#define M_OPL_array_t() M_OPEXTEND( ARRAY_OPLIST(array, M_BASIC_OPLIST), SUBTYPE(int), FULL_P(array_full_p) )
SHARED_PTR_DECL(shared_array, M_OPL_array_t() )

#elif SHARED_PTR_HEADER == 3

ARRAY_DEF(array_str, string_t)
SHARED_PTR_DECL(shared_array_str, ARRAY_OPLIST(array_str, M_STRING_OPLIST))

#elif SHARED_PTR_HEADER == 4

// TEST WITH LIST
LIST_DEF(list, float)
#define M_OPL_list_t() LIST_OPLIST(list, M_BASIC_OPLIST)
SHARED_WEAK_PTR_DECL(shared_list, M_OPL_list_t())
SHARED_WEAK_PTR_DEF_EXTERN(shared_list, list_t)

LIST_DUAL_PUSH_DEF(list2, int)
SHARED_WEAK_PTR_DEF(shared_list2, list2_t, LIST_OPLIST(list2))


// TEST WITH DICT
DICT_DEF2(dict, float, float)
#define M_OPL_dict_t() DICT_OPLIST(dict, M_BASIC_OPLIST, M_BASIC_OPLIST)
SHARED_WEAK_PTR_DECL_AS(shared_dict, shared_dict_t, M_OPL_dict_t())
SHARED_WEAK_PTR_DEF_EXTERN_AS(shared_dict, shared_dict_t, dict_t)

DICT_DEF2(string_pool, string_t, string_t)
#define M_OPL_string_pool_t() DICT_OPLIST(string_pool, STRING_OPLIST, STRING_OPLIST)
SHARED_PTR_DEF(string_pool_ts, string_pool_t, M_OPL_string_pool_t() )

DICT_SET_DEF(dict2, int)
SHARED_PTR_DEF(shared_dict2, dict2_t, DICT_SET_OPLIST(dict2))

#else
#error "SHARED_PTR_HEADER must be 1 or 2"
#endif
#undef SHARED_PTR_HEADER
