/* stdbool.h: ISO/IEC 9899:1999 (C99), section 7.16 */

/* Copyright (C) ARM Ltd., 2002
 * All rights reserved
 * RCS $Revision: 177844 $
 * Checkin $Date: 2012-11-21 11:51:12 +0000 (Wed, 21 Nov 2012) $
 * Revising $Author: drodgman $
 */

#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined 1

#if defined(__C51__)
typedef  bit	bool;
#else
typedef  unsigned char	bool;
#endif

#define false	0
#define true	1

#endif /* __bool_true_false_are_defined */

