#ifndef _GJK_COMPILER_H_
#define _GJK_COMPILER_H_

/**
 * Using this macros you can specify is it's likely or unlikely that branch
 * will be used.
 * Comes from linux header file ./include/compiler.h
 */
#define _gjk_likely(x) __builtin_expect(!!(x), 1)
#define _gjk_unlikely(x) __builtin_expect(!!(x), 0)


/** GCC Extensions: **/

/**
 * Marks function as deprecated results in warning is used anywhere in
 * source code.
 */
#define _gjk_deprecated __attribute__((deprecated))

/**
 * Causes declaration of function to be emitted as weak rather than global
 * symbol.
 */
#define _gjk_weak __attribute__((weak))

/**
 * Marks function which won't return. (They always call on end exit() or
 * abort() for example).
 */
#define _gjk_noreturn __attribute__((noreturn))

/**
 * Marks inline function.
 */
#define _gjk_inline static inline __attribute__((always_inline))

/**
 * Marks function as inline and to be inlined according to c89 standard
 * rather than c99.
 */
#define _gjk_gnu_inline inline __attribute__((always_inline, gnu_inline))

/**
 * __const tells the compiler that a particular function has no state
 * (that is, it uses the arguments passed in to generate a result to
 * return).
 */
#define _gjk_const __attribute__((__const__))


/**
 * Sets up minimal alignment of variable.
 */
#define _gjk_align(len) __attribute__((aligned(len)))


/**
 * __prefetch(x)  - prefetches the cacheline at "x" for read
 * __prefetchw(x) - prefetches the cacheline at "x" for write
 */
#define _gjk_prefetch(x) __builtin_prefetch(x)
#define _gjk_prefetchw(x) __builtin_prefetch(x,1)

#endif /* __GJK_COMPILER_H_ */

