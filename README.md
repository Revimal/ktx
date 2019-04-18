![ktx logo](logo.png)

[![Beerware License](https://img.shields.io/badge/license-Beerware-green.svg)](https://wikipedia.org/wiki/Beerware)
[![Release: 1.0.1](https://img.shields.io/badge/release-v1.0.1-blue.svg)](https://github.com/revimal/ktx/releases/tag/v1.0.1)

## What is the Kernel Test eXpressions?
Kernel Test eXpressions(KTX) is the most simple testing framework for kernel projects.
It's compatible with C99 standards and supports all versions of Linux kernel.
Inspired by [Catch2](https://github.com/catchorg/Catch2).

## How to use it
### Simple Example
#### module.c
```C
#include <linux/kernel.h>
#include <linux/module.h>
#include "ktx.h"

KTX_DEFINE(testcase_simple)
{
	static const int test_01 = 10;
	static const void *test_02 = NULL;
	static const char test_03 = 'a';
	static const char *test_04 = "Hello, there";

	KTX_CHECK(testcase_simple, test_01, 10);
	KTX_CHECK(testcase_simple, test_02, 10);
	KTX_CHECK(testcase_simple, test_03, 'a');
	KTX_CHECK(testcase_simple, test_04, NULL);
}

static int __init testmod_init(void)
{
	KTX_LOGGER("Init Module\n");
	KTX_RUN(testcase_simple);
	return 0;
}

static void __exit testmod_exit(void)
{
	unsigned long fails = KTX_RESULT(testcase_simple);

	KTX_LOGGER("Fail Counts: %lu\n", fails);
	KTX_REPORT(testcase_simple);
	KTX_LOGGER("Exit Module\n");
	return;
}

module_init(testmod_init);
module_exit(testmod_exit);
```
#### dmesg result
```
$ sudo insmod testmod.ko
$ sudo rmmod testmod
$ sudo dmesg
[ 4455.007871] Init Module
[ 4455.007872] testcase_simple: PASS
[ 4455.007872] 	Evaluation: 10 == 10
[ 4455.007872] 	Expression: test_01 == 10
[ 4455.007872] testcase_simple: FAILED
[ 4455.007873] 	Evaluation: 0 == 10
[ 4455.007873] 	Expression: test_02 == 10
[ 4455.007873] testcase_simple: PASS
[ 4455.007873] 	Evaluation: 97 == 97
[ 4455.007874] 	Expression: test_03 == 'a'
[ 4455.007874] testcase_simple: FAILED
[ 4455.007874] 	Evaluation: 18446744072657121427 == 0
[ 4455.007874] 	Expression: test_04 == ((void *)0)
[ 4456.745734] Fail Counts: 2
[ 4456.745734] [*] TESTCASE Report
[ 4456.745735]  | TC Name   : testcase_simple
[ 4456.745735]  | Testnum   : 4
[ 4456.745735]  | Result    : FAILED
[ 4456.745735]  | Tested    : 4
[ 4456.745735]  | Passed    : 2
[ 4456.745736]  | Failed    : 2
[ 4456.745736] [*]
[ 4456.745736] Exit Module
```

### Modular Example
#### testcase.h
```C
#ifndef _TESTCASE_H
#define _TESTCASE_H
#include "ktx.h"

KTX_DECLARE(testcase_modular);

#endif
```
#### testcase.c
```C
#include "testcase.h"

KTX_DEFINE(testcase_modular)
{
	static const int test_01 = 1;
	static const long test_02 = 2;
	static const unsigned int test_03 = 3;
	static const unsigned long test_04 = 4;

	KTX_CHECK(testcase_modular, test_01, 1);
	KTX_CHECK(testcase_modular, test_02, 2);
	KTX_REQUIRE(testcase_modular, test_03, -3);
	/* Cannot reach here */
	KTX_CHECK(testcase_modular, test_04, 4);
}
```
#### module.c
```C
#include <linux/kernel.h>
#include <linux/module.h>
#include "testcase.h"

static int __init testmod_init(void)
{
	KTX_LOGGER("Init Module\n");
	KTX_RUN(testcase_modular);
	return 0;
}

static void __exit testmod_exit(void)
{
	unsigned long fails = KTX_RESULT(testcase_modular);

	KTX_LOGGER("Fail Counts: %lu\n", fails);
	KTX_REPORT(testcase_modular);
	KTX_LOGGER("Exit Module\n");
	return;
}

module_init(testmod_init);
module_exit(testmod_exit);
```
#### dmesg result
```
$ sudo insmod testmod.ko
$ sudo rmmod testmod
$ sudo dmesg
[ 4767.529209] Init Module
[ 4767.529211] testcase_modular: PASS
[ 4767.529214] 	Evaluation: 1 == 1
[ 4767.529215] 	Expression: test_01 == 1
[ 4767.529216] testcase_modular: PASS
[ 4767.529218] 	Evaluation: 2 == 2
[ 4767.529219] 	Expression: test_02 == 2
[ 4767.529221] testcase_modular: FAILED
[ 4767.529223] 	Evaluation: 3 == 18446744073709551613
[ 4767.529224] 	Expression: test_03 == -3
[ 4769.120650] Fail Counts: 1
[ 4769.120651] [*] TESTCASE Report
[ 4769.120651]  | TC Name   : testcase_modular
[ 4769.120651]  | Testnum   : 4
[ 4769.120651]  | Result    : FAILED
[ 4769.120652]  | Tested    : 3
[ 4769.120652]  | Passed    : 2
[ 4769.120652]  | Failed    : 1
[ 4769.120652] [*]
[ 4769.120652] Exit Module
```

## More informations
* Issues can be reported on [Github Issue Tracker](https://github.com/Revimal/ktx/issues)
* Kernel Test eXpressions(KTX) is released under the [Beerware License](https://github.com/Revimal/ktx/blob/master/LICENSE)
