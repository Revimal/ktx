/**
 * @file ktx.h
 * @brief Kernel Test eXpressions
 *
 * A simple, header-only, test framework for kernel projects.
 * Inspired by 'Catch2'.
 *
 * @author Hyeonho Seo (Revimal)
 * @bug No Known Bugs
 */

#ifndef _KTX_H
#define _KTX_H

#include <linux/printk.h>

/**
 * @def KTX_EQUAL
 * 	Same as 0
 */
#define KTX_EQUAL 0
/**
 * @def KTX_EQUAL
 * 	Same as 1
 */
#define KTX_NOTEQ 1

/**
 * @def KTX_LOGGER(...)
 * 	Same as printk(KERN_INFO ...)
 */
#define KTX_LOGGER(...) \
	printk(KERN_INFO __VA_ARGS__)

struct ktx_internal_testcase
{
	const char *name;
	unsigned long test_cnt;
	unsigned long eval_cnt;
	unsigned long fail_cnt;
	int skip_next;
};
typedef struct ktx_internal_testcase ktx_tc_t;

static inline int ktx_internal_assert(ktx_tc_t *tc, int eval, const char *expr, unsigned long value1, unsigned long value2)
{
	int ret = KTX_EQUAL;
	tc->test_cnt++;
	if (!(tc->skip_next))
	{
		KTX_LOGGER("%s: %s\n", tc->name, !!(eval) ? "PASS" : "FAILED");
		KTX_LOGGER("\tEvaluation: %lu == %lu\n", value1, value2);
		KTX_LOGGER("\tExpression: %s\n", expr);
		if (!(eval))
		{
			tc->fail_cnt++;
			ret = KTX_NOTEQ;
		}
		tc->eval_cnt++;
	}
	return ret;
}

static inline void ktx_internal_report(ktx_tc_t *tc)
{
	KTX_LOGGER("[*] TESTCASE Report\n");
	KTX_LOGGER(" | TC Name   : %s\n", tc->name);
	KTX_LOGGER(" | Testnum   : %lu\n", tc->test_cnt);
	KTX_LOGGER(" | Result    : %s\n", !!(tc->fail_cnt) ? "FAILED" : "PASS");
	KTX_LOGGER(" | Tested    : %lu\n", tc->eval_cnt);
	KTX_LOGGER(" | Passed    : %lu\n", tc->eval_cnt - tc->fail_cnt);
	KTX_LOGGER(" | Failed    : %lu\n", tc->fail_cnt);
	KTX_LOGGER("[*]\n");
}

#define KTX_INTERNAL_STRINGIFY( expr )                 #expr
#define KTX_INTERNAL_TESTEQ_EXPR( expr1, expr2 )       expr1 == expr2
#define KTX_INTERNAL_STRC(tc) \
	ktx_tc_t ktx_strc_ ## tc
#define KTX_INTERNAL_INIT(tc) \
	KTX_INTERNAL_STRC(tc) = { \
		.name = KTX_INTERNAL_STRINGIFY(tc), \
		.test_cnt = 0, \
		.eval_cnt = 0, \
		.fail_cnt = 0, \
		.skip_next = 0 \
	}
#define KTX_INTERNAL_ACCESS(tc) \
	(&( ktx_strc_ ## tc ))
#define KTX_INTERNAL_FUNC(tc) \
	void ktx_func_ ## tc (void)
#define KTX_INTERNAL_ASSERT(tc, expr, value1, value2) \
	ktx_internal_assert((tc), KTX_INTERNAL_TESTEQ_EXPR(value1, value2), \
		KTX_INTERNAL_STRINGIFY(expr), (value1), (value2))

/**
 * @def KTX_DECLARE(tc)
 * 	Macro function to declare test-cases.
 * 	Place in header files.
 */
#define KTX_DECLARE(tc) \
	extern KTX_INTERNAL_STRC(tc); \
	extern KTX_INTERNAL_FUNC(tc)
/**
 * @def KTX_DEFINE(tc)
 * 	Macro function to define test-cases.
 * 	Place in a source file.
 */
#define KTX_DEFINE(tc) \
	KTX_INTERNAL_INIT(tc); \
	KTX_INTERNAL_FUNC(tc)
/**
 * @def KTX_CHECK(tc)
 * 	Macro function to write check-expressions.
 * 	If two expressions are the same, will return KTX_EQUAL.
 * 	If not, will return KTX_NOTEQ.
 */
#define KTX_CHECK(tc, expr1, expr2) \
	KTX_INTERNAL_ASSERT(KTX_INTERNAL_ACCESS(tc), \
		KTX_INTERNAL_TESTEQ_EXPR(expr1, expr2), \
		(unsigned long)(expr1), (unsigned long)(expr2))

/**
 * @def KTX_REQUIRE(tc)
 * 	Macro function to write check-expressions.
 * 	If two expressions are not the same, the test-case will return immediately.
 */

#define KTX_REQUIRE(tc, expr1, expr2) \
	if (KTX_CHECK(tc, expr1, expr2) == KTX_NOTEQ) \
	{ \
		KTX_INTERNAL_ACCESS(tc)->skip_next = 1; \
	}
/**
 * @def KTX_RUN(tc)
 * 	Macro function to run test-cases.
 */
#define KTX_RUN(tc) \
	ktx_func_ ## tc ()

/**
 * @def KTX_RUN_RESULT(tc)
 * 	Macro function to get results.
 * 	Will return fail counts.
 */
#define KTX_RESULT(tc) \
	(KTX_INTERNAL_ACCESS(tc)->fail_cnt)

/**
 * @def KTX_REPORT(tc)
 * 	Macro function to print statistics.
 */
#define KTX_REPORT(tc) \
	ktx_internal_report(KTX_INTERNAL_ACCESS(tc))

#endif
