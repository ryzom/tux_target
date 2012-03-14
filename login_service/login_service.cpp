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


//
// Includes
//

// 44449 is the server connection for welcome services
// 44448 is the server connection for web
// 44447 is the server connection for clients

#include <nel/misc/types_nl.h>

#include <math.h>
#include <stdio.h>
#include <ctype.h>

#include <map>
#include <vector>

#include <nel/misc/log.h>
#include <nel/misc/path.h>
#include <nel/misc/debug.h>
#include <nel/misc/command.h>
#include <nel/misc/displayer.h>
#include <nel/misc/config_file.h>
#include <nel/misc/window_displayer.h>

#include <nel/net/service.h>
#include <nel/net/login_cookie.h>

#include "login_service.h"
#include "connection_ws.h"
#include "connection_client.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;


//
// Variables
//

// store specific user informations
NLMISC::CFileDisplayer Fd ("login_service.stat");
NLMISC::CStdDisplayer Sd;
NLMISC::CLog *Output = 0;

string DatabaseName, DatabaseHost, DatabaseLogin, DatabasePassword;

MYSQL *DatabaseConnection = 0;




CMysqlResult::CMysqlResult() 
{ 
	//nlinfo("CMysqlResult::construct 0x%p",this);
	_result = NULL;
}
/*
CMysqlResult::CMysqlResult(const CMysqlResult &mysqlResult) 
{ 
	nlwarning("CMysqlResult::copy1 not implemented");
	if(_result!=NULL)
	{
		//nlinfo("mysql_free_result3 0x%p 0x%p",this,_result);
		mysql_free_result(_result);
	}
	_result=mysqlResult._result; 
}
*/

CMysqlResult::CMysqlResult(MYSQL_RES *result) 
{ 
	//nlinfo("CMysqlResult::construct2  0x%p 0x%p->0x%p",this,_result,result);
	if(_result!=NULL)
	{
		//nlinfo("mysql_free_result2 0x%p 0x%p",this,_result);
		mysql_free_result(_result);
	}
	_result = result; 
};


CMysqlResult::~CMysqlResult()
{
	//nlinfo("mysql_free_result1 0x%p 0x%p",this,_result);
	mysql_free_result(_result);
}

/*
CMysqlResult& CMysqlResult::operator=(CMysqlResult &mysqlResult)
{
	nlwarning("CMysqlResult::copy2 not implemented");
	if(_result!=NULL)
	{
		//nlinfo("mysql_free_result4 0x%p 0x%p",this,_result);
		mysql_free_result(_result);
	}
	_result=mysqlResult._result; 
	return *this;		
}
*/

CMysqlResult& CMysqlResult::operator=(MYSQL_RES *result)
{
	if(_result!=NULL)
	{
		//nlinfo("mysql_free_result3 0x%p 0x%p",this,_result);
		mysql_free_result(_result);
	}
	_result = result;
	return *this;
}


//
// Functions
//

string sqlQuery(const string &query, sint32 &nbRow, MYSQL_ROW &firstRow, CMysqlResult &result)
{
	nlassert(DatabaseConnection);
	nlinfo("sqlQuery: '%s'", query.c_str());
	sint ret = mysql_query(DatabaseConnection, query.c_str());
	if(ret != 0)
	{
		nlwarning("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(DatabaseConnection));
		return toString("mysql_query('%s') failed: '%s'", query.c_str(),  mysql_error(DatabaseConnection));
	}

	if(query.find("select") == 0)
	{
		// store result on select query
		result = mysql_store_result(DatabaseConnection);
		if(result == 0)
		{
			nlwarning("mysql_store_result() failed from query '%s': %s", query.c_str (),  mysql_error(DatabaseConnection));
			return toString("mysql_store_result() failed from query '%s': %s", query.c_str (),  mysql_error(DatabaseConnection));
		}

		nbRow = (sint32)mysql_num_rows(result);
		
		if(nbRow > 0)
		{
			firstRow = mysql_fetch_row(result);
			if(firstRow == 0)
			{
				nlwarning("mysql_fetch_row (%s) failed: %s", query.c_str (),  mysql_error(DatabaseConnection));
				return toString("mysql_fetch_row (%s) failed: %s", query.c_str (),  mysql_error(DatabaseConnection));
			}
		}
		else
		{
			firstRow = 0;
		}
	}

	return "";
}

string resetDatabase()
{
	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;

	// Reset all shards database
	string reason = sqlQuery("update shard set NbPlayers=0, State='Offline', InternalId=0", nbrow, row, result);
	if(!reason.empty()) return reason;
	
	// Reset all user database
	reason = sqlQuery("update user set ShardId=-1, State='Offline', Cookie=''", nbrow, row, result);
	if(!reason.empty()) return reason;

	return "";
}

void cbDatabaseVar(CConfigFile::CVar &var)
{
	DatabaseName = IService::getInstance()->ConfigFile.getVar("DatabaseName").asString ();
	DatabaseHost = IService::getInstance()->ConfigFile.getVar("DatabaseHost").asString ();
	DatabaseLogin = IService::getInstance()->ConfigFile.getVar("DatabaseLogin").asString ();
	DatabasePassword = IService::getInstance()->ConfigFile.getVar("DatabasePassword").asString ();

	if(DatabaseConnection)
	{
		mysql_close(DatabaseConnection);
		DatabaseConnection = 0;
	}
	MYSQL *db = mysql_init(0);
	if(db == 0)
	{
		nlwarning("mysql_init() failed");
		return;
	}

	DatabaseConnection = mysql_real_connect(db, DatabaseHost.c_str(), DatabaseLogin.c_str(), DatabasePassword.c_str(), DatabaseName.c_str(),0,0,0);
	if (DatabaseConnection == 0 || DatabaseConnection != db)
	{
		mysql_close(db);
		DatabaseConnection = 0;
		nlerror("mysql_real_connect() failed to '%s' with login '%s' and database name '%s'", DatabaseHost.c_str(), DatabaseLogin.c_str(), DatabaseName.c_str());
		return;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// SERVICE IMPLEMENTATION /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class CLoginService : public IService
{

public:

	// Init the service, load the universal time.
	void init()
	{
		if(NLMISC::CFile::fileExists("log000.log"))
			NLMISC::CFile::deleteFile("log000.log");
		
		if(NLMISC::CFile::fileExists("login_service000.stat"))
			NLMISC::CFile::deleteFile("login_service000.stat");
				
		Output = new CLog;
		Output->addDisplayer (&Fd);
		if (WindowDisplayer != 0) Output->addDisplayer (WindowDisplayer);

		connectionWSInit();

		// Initialize the database access

		ConfigFile.setCallback("ForceDatabaseReconnection", cbDatabaseVar);
		cbDatabaseVar (ConfigFile.getVar ("ForceDatabaseReconnection"));

		// Init the client server connections

		connectionClientInit();

		string reason = resetDatabase();
		if(!reason.empty()) nlerror(reason.c_str());

		Output->displayNL("Login Service initialised");
	}

	bool update()
	{
		connectionClientUpdate();
		connectionWSUpdate();

		return true;
	}

	// release the service, save the universal time
	void release()
	{
		string reason = resetDatabase();
		
		connectionWSRelease();
		
		Output->displayNL("Login Service released");
	}
};

// Service instantiation
NLNET_SERVICE_MAIN (CLoginService, "LS", "login_service", 49999, EmptyCallbackArray, "", "");


//
// Variables
//



//
// Commands
//
