/*
 *  include/scst_debug.h
 *
 *  Copyright (C) 2004 - 2011 Vladislav Bolkhovitin <vst@vlnb.net>
 *  Copyright (C) 2004 - 2005 Leonid Stoljar
 *  Copyright (C) 2007 - 2010 ID7 Ltd.
 *  Copyright (C) 2010 - 2011 SCST Ltd.
 *
 *  Contains macros for execution tracing and error reporting
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation, version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 */

#ifndef __SCST_DEBUG_H
#define __SCST_DEBUG_H

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 32)
#include <linux/autoconf.h>	/* for CONFIG_* */
#else
#include <generated/autoconf.h>	/* for CONFIG_* */
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 19)
#include <linux/bug.h>		/* for WARN_ON_ONCE */
#endif

#if !defined(INSIDE_KERNEL_TREE)
#ifdef CONFIG_SCST_DEBUG

#define sBUG() do {						\
	printk(KERN_CRIT "BUG at %s:%d\n",			\
	       __FILE__, __LINE__);				\
	local_irq_enable();					\
	while (in_softirq())					\
		local_bh_enable();				\
	BUG();							\
} while (0)

#define sBUG_ON(p) do {						\
	if (unlikely(p)) {					\
		printk(KERN_CRIT "BUG at %s:%d (%s)\n",		\
		       __FILE__, __LINE__, #p);			\
		local_irq_enable();				\
		while (in_softirq())				\
			local_bh_enable();			\
		BUG();						\
	}							\
} while (0)

#else

#define sBUG() BUG()
#define sBUG_ON(p) BUG_ON(p)

#endif
#endif

#ifdef CONFIG_SCST_EXTRACHECKS
#define EXTRACHECKS_BUG_ON(a)		sBUG_ON(a)
#define EXTRACHECKS_WARN_ON(a)		WARN_ON(a)
#define EXTRACHECKS_WARN_ON_ONCE(a)	WARN_ON_ONCE(a)
#else
#define EXTRACHECKS_BUG_ON(a)		do { } while (0)
#define EXTRACHECKS_WARN_ON(a)		do { } while (0)
#define EXTRACHECKS_WARN_ON_ONCE(a)	do { } while (0)
#endif

#define TRACE_NULL           0x00000000
#define TRACE_DEBUG          0x00000001
#define TRACE_FUNCTION       0x00000002
#define TRACE_LINE           0x00000004
#define TRACE_PID            0x00000008
#ifndef GENERATING_UPSTREAM_PATCH
#define TRACE_ENTRYEXIT      0x00000010
#endif
#define TRACE_BUFF           0x00000020
#define TRACE_MEMORY         0x00000040
#define TRACE_SG_OP          0x00000080
#define TRACE_OUT_OF_MEM     0x00000100
#define TRACE_MINOR          0x00000200 /* less important events */
#define TRACE_MGMT           0x00000400
#define TRACE_MGMT_DEBUG     0x00000800
#define TRACE_SCSI           0x00001000
#define TRACE_SPECIAL        0x00002000 /* filtering debug, etc */
#define TRACE_FLOW_CONTROL   0x00004000 /* flow control in action */
#define TRACE_PRES           0x00008000
#define TRACE_ALL            0xffffffff
/* Flags 0xXXXX0000 are local for users */

#define TRACE_MINOR_AND_MGMT_DBG	(TRACE_MINOR|TRACE_MGMT_DEBUG)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24) && !defined(RHEL_MAJOR)
#define KERN_CONT       ""
#endif

/*
 * Note: in the next two printk() statements the KERN_CONT macro is only
 * present to suppress a checkpatch warning (KERN_CONT is defined as "").
 */
#define PRINT(log_flag, format, args...)  \
		printk(log_flag format "\n", ## args)
#define PRINTN(log_flag, format, args...) \
		printk(log_flag format, ## args)

#ifdef LOG_PREFIX
#define __LOG_PREFIX	LOG_PREFIX
#else
#define __LOG_PREFIX	NULL
#endif

#if defined(CONFIG_SCST_DEBUG) || defined(CONFIG_SCST_TRACING)

#ifndef CONFIG_SCST_DEBUG
#define ___unlikely(a)		(a)
#else
#define ___unlikely(a)		unlikely(a)
#endif

/*
 * We don't print prefix for debug traces to not put additional pressure
 * on the logging system in case of a lot of logging.
 */

int debug_print_prefix(unsigned long trace_flag,
	const char *prefix, const char *func, int line);
void debug_print_buffer(const void *data, int len);
const char *debug_transport_id_to_initiator_name(const uint8_t *transport_id);

#define TRACING_MINOR() (trace_flag & TRACE_MINOR)

#define TRACE(trace, format, args...)					\
do {									\
	if (___unlikely(trace_flag & (trace))) {			\
		debug_print_prefix(trace_flag, __LOG_PREFIX,		\
				       __func__, __LINE__);		\
		PRINT(KERN_CONT, format, args);				\
	}								\
} while (0)

#ifdef CONFIG_SCST_DEBUG

#define PRINT_BUFFER(message, buff, len)                            \
do {                                                                \
	PRINT(KERN_INFO, "%s:%s:", __func__, message);		    \
	debug_print_buffer(buff, len);				    \
} while (0)

#else

#define PRINT_BUFFER(message, buff, len)                            \
do {                                                                \
	PRINT(KERN_INFO, "%s:", message);			    \
	debug_print_buffer(buff, len);				    \
} while (0)

#endif

#define PRINT_BUFF_FLAG(flag, message, buff, len)			\
do {									\
	if (___unlikely(trace_flag & (flag))) {				\
		debug_print_prefix(trace_flag, NULL, __func__, __LINE__);\
		PRINT(KERN_CONT, "%s:", message);			\
		debug_print_buffer(buff, len);				\
	}								\
} while (0)

#else  /* CONFIG_SCST_DEBUG || CONFIG_SCST_TRACING */

#define TRACING_MINOR() (false)

#define TRACE(trace, args...) do {} while (0)
#define PRINT_BUFFER(message, buff, len) do {} while (0)
#define PRINT_BUFF_FLAG(flag, message, buff, len) do {} while (0)

#endif /* CONFIG_SCST_DEBUG || CONFIG_SCST_TRACING */

#ifdef CONFIG_SCST_DEBUG

#define TRACE_DBG_FLAG(trace, format, args...)				\
do {									\
	if (trace_flag & (trace)) {					\
		debug_print_prefix(trace_flag, NULL, __func__, __LINE__);\
		PRINT(KERN_CONT, format, args);				\
	}								\
} while (0)

#define TRACE_MEM(args...)		TRACE_DBG_FLAG(TRACE_MEMORY, args)
#define TRACE_SG(args...)		TRACE_DBG_FLAG(TRACE_SG_OP, args)
#define TRACE_DBG(args...)		TRACE_DBG_FLAG(TRACE_DEBUG, args)
#define TRACE_DBG_SPECIAL(args...)	TRACE_DBG_FLAG(TRACE_DEBUG|TRACE_SPECIAL, args)
#define TRACE_MGMT_DBG(args...)		TRACE_DBG_FLAG(TRACE_MGMT_DEBUG, args)
#define TRACE_MGMT_DBG_SPECIAL(args...)	\
		TRACE_DBG_FLAG(TRACE_MGMT_DEBUG|TRACE_SPECIAL, args)
#define TRACE_PR(args...)		TRACE_DBG_FLAG(TRACE_PRES, args)

#define TRACE_BUFFER(message, buff, len)				\
do {									\
	if (trace_flag & TRACE_BUFF) {					\
		debug_print_prefix(trace_flag, NULL, __func__, __LINE__);\
		PRINT(KERN_CONT, "%s:", message);			\
		debug_print_buffer(buff, len);				\
	}								\
} while (0)

#define TRACE_BUFF_FLAG(flag, message, buff, len)			\
do {									\
	if (trace_flag & (flag)) {					\
		debug_print_prefix(trace_flag, NULL, __func__, __LINE__);\
		PRINT(KERN_CONT, "%s:", message);			\
		debug_print_buffer(buff, len);				\
	}								\
} while (0)

#define PRINT_LOG_FLAG(log_flag, format, args...)			\
do {									\
	debug_print_prefix(trace_flag, __LOG_PREFIX, __func__, __LINE__);\
	PRINT(KERN_CONT, format, args);					\
} while (0)

#define PRINT_WARNING(format, args...)					\
do {									\
	debug_print_prefix(trace_flag, __LOG_PREFIX, __func__, __LINE__);\
	PRINT(KERN_CONT, "***WARNING***: " format, args);		\
} while (0)

#define PRINT_ERROR(format, args...)					\
do {									\
	debug_print_prefix(trace_flag, __LOG_PREFIX, __func__, __LINE__);\
	PRINT(KERN_CONT, "***ERROR***: " format, args);			\
} while (0)

#define PRINT_CRIT_ERROR(format, args...)				\
do {									\
	debug_print_prefix(trace_flag, __LOG_PREFIX, __func__, __LINE__);\
	PRINT(KERN_CONT, "***CRITICAL ERROR***: " format, args);	\
} while (0)

#define PRINT_INFO(format, args...)					\
do {									\
	debug_print_prefix(trace_flag, __LOG_PREFIX, __func__, __LINE__);\
	PRINT(KERN_CONT, format, args);					\
} while (0)

#ifndef GENERATING_UPSTREAM_PATCH
#define TRACE_ENTRY()							\
do {									\
	if (trace_flag & TRACE_ENTRYEXIT) {				\
		if (trace_flag & TRACE_PID) {				\
			PRINT(KERN_INFO, "[%d]: ENTRY %s", current->pid, \
				__func__);				\
		}							\
		else {							\
			PRINT(KERN_INFO, "ENTRY %s", __func__);	\
		}							\
	}								\
} while (0)

#define TRACE_EXIT()							\
do {									\
	if (trace_flag & TRACE_ENTRYEXIT) {				\
		if (trace_flag & TRACE_PID) {				\
			PRINT(KERN_INFO, "[%d]: EXIT %s", current->pid,	\
				__func__);				\
		}							\
		else {							\
			PRINT(KERN_INFO, "EXIT %s", __func__);		\
		}							\
	}								\
} while (0)

#define TRACE_EXIT_RES(res)						\
do {									\
	if (trace_flag & TRACE_ENTRYEXIT) {				\
		if (trace_flag & TRACE_PID) {				\
			PRINT(KERN_INFO, "[%d]: EXIT %s: %ld", current->pid, \
			      __func__, (long)(res));			\
		}							\
		else {							\
			PRINT(KERN_INFO, "EXIT %s: %ld",		\
				__func__, (long)(res));			\
		}							\
	}                                                               \
} while (0)

#define TRACE_EXIT_HRES(res)						\
do {									\
	if (trace_flag & TRACE_ENTRYEXIT) {				\
		if (trace_flag & TRACE_PID) {				\
			PRINT(KERN_INFO, "[%d]: EXIT %s: 0x%lx", current->pid, \
			      __func__, (long)(res));			\
		}							\
		else {							\
			PRINT(KERN_INFO, "EXIT %s: %lx",		\
					__func__, (long)(res));		\
		}							\
	}                                                               \
} while (0)
#endif

#else  /* CONFIG_SCST_DEBUG */

#define TRACE_MEM(format, args...) do {} while (0)
#define TRACE_SG(format, args...) do {} while (0)
#define TRACE_DBG(format, args...) do {} while (0)
#define TRACE_DBG_FLAG(format, args...) do {} while (0)
#define TRACE_DBG_SPECIAL(format, args...) do {} while (0)
#define TRACE_MGMT_DBG(format, args...) do {} while (0)
#define TRACE_MGMT_DBG_SPECIAL(format, args...) do {} while (0)
#define TRACE_PR(format, args...) do {} while (0)
#define TRACE_BUFFER(message, buff, len) do {} while (0)
#define TRACE_BUFF_FLAG(flag, message, buff, len) do {} while (0)

#ifndef GENERATING_UPSTREAM_PATCH
#define TRACE_ENTRY() do {} while (0)
#define TRACE_EXIT() do {} while (0)
#define TRACE_EXIT_RES(res) do {} while (0)
#define TRACE_EXIT_HRES(res) do {} while (0)
#endif

#ifdef LOG_PREFIX

#define PRINT_INFO(format, args...)				\
do {								\
	PRINT(KERN_INFO, "%s: " format, LOG_PREFIX, args);	\
} while (0)

#define PRINT_WARNING(format, args...)          \
do {                                            \
	PRINT(KERN_INFO, "%s: ***WARNING***: "	\
	      format, LOG_PREFIX, args);	\
} while (0)

#define PRINT_ERROR(format, args...)            \
do {                                            \
	PRINT(KERN_INFO, "%s: ***ERROR***: "	\
	      format, LOG_PREFIX, args);	\
} while (0)

#define PRINT_CRIT_ERROR(format, args...)       \
do {                                            \
	PRINT(KERN_INFO, "%s: ***CRITICAL ERROR***: "	\
		format, LOG_PREFIX, args);		\
} while (0)

#else

#define PRINT_INFO(format, args...)		\
do {                                            \
	PRINT(KERN_INFO, format, args);		\
} while (0)

#define PRINT_WARNING(format, args...)          \
do {                                            \
	PRINT(KERN_INFO, "***WARNING***: "	\
		format, args);			\
} while (0)

#define PRINT_ERROR(format, args...)		\
do {                                            \
	PRINT(KERN_ERR, "***ERROR***: "		\
		format, args);			\
} while (0)

#define PRINT_CRIT_ERROR(format, args...)		\
do {							\
	PRINT(KERN_CRIT, "***CRITICAL ERROR***: "	\
		format, args);				\
} while (0)

#endif /* LOG_PREFIX */

#endif /* CONFIG_SCST_DEBUG */

#if defined(CONFIG_SCST_DEBUG) && defined(CONFIG_DEBUG_SLAB)
#define SCST_SLAB_FLAGS (SLAB_RED_ZONE | SLAB_POISON)
#else
#define SCST_SLAB_FLAGS 0L
#endif

#endif /* __SCST_DEBUG_H */
