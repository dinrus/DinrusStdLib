#ifndef SQL_H
#define SQL_H

#include <drx/Core/Core.h>
#include <drxtypes.h>

namespace drx {

#ifdef flagNOAPPSQL
	#define NOAPPSQL
	#define APPSQLCURSOR
	#define APPSQLSESSION
#else
	#define APPSQLCURSOR = SQL
	#define APPSQLSESSION = SQL.GetSession()
#endif

#include "Sqlexp.h"
#include "Sqls.h"
#include "SqlSchema.h"

}

#endif
