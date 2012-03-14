/* Copyright, 2010 Tux Target
 * Copyright, 2003 Melting Pot
 *
 * This file is part of Tux Target.
 * Tux Target is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Tux Target is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Tux Target; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include <mysql.h>

#include <nel/misc/types_nl.h>

#include <nel/misc/log.h>
#include <nel/misc/debug.h>
#include <nel/misc/displayer.h>
#include <nel/misc/config_file.h>

#include <nel/net/service.h>


//
// Variables
//

extern NLMISC::CFileDisplayer	Fd;
extern NLMISC::CLog				*Output;

extern MYSQL *DatabaseConnection;

class CMysqlResult
{
public:
	CMysqlResult();
	CMysqlResult(MYSQL_RES *result);
	~CMysqlResult();

	//CMysqlResult& operator=(CMysqlResult &mysqlResult);
	CMysqlResult& operator=(MYSQL_RES *result);
	
	operator MYSQL_RES*()
	{ 
		//nlinfo("CMysqlResult : get result 0x%p 0x%p",this,_result);
		return _result; 
	};
private:
	//we don't want user to do a copy
	CMysqlResult(const CMysqlResult &mysqlResult){};
	MYSQL_RES *_result;
};

//
// Functions
//

std::string sqlQuery(const std::string &query, sint32 &nbRow, MYSQL_ROW &firstRow, CMysqlResult &result);

#endif // LOGIN_SERVICE_H

/* End of login_service.h */
