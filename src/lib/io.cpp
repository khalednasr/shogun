#include "lib/io.h"
#include "lib/common.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

FILE* CIO::target=stdout;

CIO::CIO()
{
}

void CIO::message(EMessageType prio, const CHAR *fmt, ... )
{
	check_target();
	print_message_prio(prio, target);
    va_list list;
    va_start(list,fmt);
    vfprintf(target,fmt,list);
    va_end(list);
    fflush(target);
}

void CIO::buffered_message(EMessageType prio, const CHAR *fmt, ... )
{
	check_target();
	print_message_prio(prio, target);
    va_list list;
    va_start(list,fmt);
    vfprintf(target,fmt,list);
    va_end(list);
}

CHAR* CIO::skip_spaces(CHAR* str)
{
	INT i=0;

	if (str)
	{
		for (i=0; isspace(str[i]); i++);

		return &str[i];
	}
	else 
		return str;
}

void CIO::set_target(FILE* t)
{
	target=t;
}

void CIO::check_target()
{
	if (!target)
		target=stdout;
}

void CIO::print_message_prio(EMessageType prio, FILE* target)
{
	switch (prio)
	{
		case M_DEBUG:
			fprintf(target, "[DEBUG] ");
			break;
		case M_INFO:
			fprintf(target, "[INFO] ");
			break;
		case M_NOTICE:
			fprintf(target, "[NOTICE] ");
			break;
		case M_WARN:
			fprintf(target, "[WARN] ");
			break;
		case M_ERROR:
			fprintf(target, "[ERROR] ");
			break;
		case M_CRITICAL:
			fprintf(target, "[CRITICAL] ");
			break;
		case M_ALERT:
			fprintf(target, "[ALERT] ");
			break;
		case M_EMERGENCY:
			fprintf(target, "[EMERGENCY] ");
			break;
		case M_MESSAGEONLY:
			break;
		default:
			break;
	}
}
