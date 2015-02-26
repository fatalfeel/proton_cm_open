#include "PlatformPrecomp.h"
#include "MySQLManager.h"

#define HAVE_INT32
#define HAVE_UINT32
#define byte_defined
#include <my_global.h>
#include <mysql.h>

#define C_MYSQL_PING_TIMER_MS (1000*60*60)*4

MySQLManager::MySQLManager()
{
	m_conn = NULL;
	m_pingTimer = 0;
	m_bLostServerConnection = false;
}

MySQLManager::~MySQLManager()
{
	Kill();
}

void MySQLManager::Kill()
{
	if (m_conn)
	{
		mysql_close(m_conn);
		m_conn = NULL;
	}
}

int MySQLManager::ShowError()
{
	int error = mysql_errno(m_conn);

	LogError("MySQLManager error: %u: %s", error, mysql_error(m_conn));
	return error;
}

bool MySQLManager::DoesTableExist(string tableName, bool bShowErrors)
{
	assert(m_conn);
	MYSQL_RES *result = NULL;

	bool bSuccess = Query("SELECT * FROM "+tableName, bShowErrors);

	if (!bSuccess) return false;

	result = mysql_store_result(m_conn);
	int fields = mysql_num_fields(result);
	mysql_free_result(result);
	return bSuccess;
}

bool MySQLManager::DoResultsExist()
{
	MYSQL_RES *result = NULL;
	result = mysql_store_result(m_conn);

	if (!result)
	{
		return false;
	}
	int rows = (int)mysql_num_rows(result);

	mysql_free_result(result);
	return rows != 0;
}


int MySQLManager::AddSelectResults(vector<VariantDB> &vdb)
{
	MYSQL_RES *result = NULL;
	result = mysql_store_result(m_conn);

	if (!result)
	{
		return 0;
	}

	int num_fields = mysql_num_fields(result);
	int rows = (int)mysql_num_rows(result);

	MYSQL_ROW row;
	MYSQL_FIELD *field;
	
	vector<string> fieldNames;
	vector<enum_field_types> fieldType;
	vector<int> maxLength;

	int curRow = (int)vdb.size();
	vdb.resize(curRow+rows);

	bool bGotFields = false;

	while ((row = mysql_fetch_row(result)))
	{
	
	  VariantDB &db = vdb[curRow++];

	  if (!bGotFields) 
	  {
		  bGotFields = true;

		  while(field = mysql_fetch_field(result)) 
		  {
			  fieldNames.push_back(field->name);
			  fieldType.push_back(field->type);
			  maxLength.push_back(field->max_length);
		  }
	  }
	  for(int i = 0; i < num_fields; i++)
		{
		
			switch(fieldType[i])
			{

			case FIELD_TYPE_DECIMAL:
			case FIELD_TYPE_FLOAT:
				db.GetVar(fieldNames[i])->Set((float)atof(row[i]));
				break;

			case FIELD_TYPE_SHORT:
			case FIELD_TYPE_LONG:
				db.GetVar(fieldNames[i])->Set((int32)atoi(row[i]));
				break;

			case FIELD_TYPE_LONGLONG:
				db.GetVar(fieldNames[i])->Set((int32)atoi(row[i]));
				break;

			case FIELD_TYPE_DATETIME:
				{
						uint	y, m, d, h, mn, s;
						uint nbScanned = sscanf(row[i], "%u-%u-%u %u:%u:%u", &y, &m, &d, &h, &mn, &s);
						assert(nbScanned == 6);
						tm	myTm;
						myTm.tm_year = y-1900;
						myTm.tm_mon = m-1;
						myTm.tm_mday = d;
						myTm.tm_hour = h;
						myTm.tm_min = mn;
						myTm.tm_sec = s;
				
						myTm.tm_isdst = -1; // let the C runtime determine daylight adjustment
						myTm.tm_wday = -1;
						myTm.tm_yday = -1;
						//assert( sizeof(time_t) == 4 && "Uh oh.. define _USE_32BIT_TIME_T somewhere for MSVC");
						uint32 t = (uint32)mktime(&myTm);
						db.GetVar(fieldNames[i])->Set(t);
					}
				break;

			case FIELD_TYPE_DATE:
				{
					//convert the sql style date ('YYYY-MM-DD') into a unix style date with string processing
					uint	y, m, d;
					uint nbScanned = sscanf(row[i], "%u-%u-%u", &y, &m, &d);
					assert(nbScanned == 3);
					tm	myTm;
					myTm.tm_year = y-1900;
					myTm.tm_mon = m-1;
					myTm.tm_mday = d;
					myTm.tm_hour = 0;
					myTm.tm_min = 0;
					myTm.tm_sec = 0;
					
					myTm.tm_isdst = -1; 
					myTm.tm_wday = -1;
					myTm.tm_yday = -1;
					//assert( sizeof(time_t) == 4 && "Uh oh.. define _USE_32BIT_TIME_T somewhere for MSVC");
					uint32 t = (uint32) mktime(&myTm);
					db.GetVar(fieldNames[i])->Set(t);
				}
				break;

			case FIELD_TYPE_TIMESTAMP:
			{
					tm	tm;
					memset( &tm, 0, sizeof( tm ));
					int nCnt = sscanf( row[i],"%4u%2u%2u%2u%2u%2u",
						&tm.tm_year, &tm.tm_mon, &tm.tm_mday,
						&tm.tm_hour, &tm.tm_min, &tm.tm_sec );

					if( nCnt == 6 ) 
					{
						tm.tm_year = tm.tm_year - 1900;
						tm.tm_mon--;
					} else
					{
						LogMsg("Error converting mysql timestamp");
					}

					//assert( sizeof(time_t) == 4 && "Uh oh.. define _USE_32BIT_TIME_T somewhere for MSVC");


					uint32 t = (uint32) mktime(&tm);
					db.GetVar(fieldNames[i])->Set(t);
				}
				break;

			case FIELD_TYPE_VAR_STRING:
				{

					//first we'll get the size of the data in here
					db.GetVar(fieldNames[i])->Set(string()); //we need to register it as a string, the mega hack we do in a
					string &s = db.GetVar(fieldNames[i])->GetString();
					//second won't do it..
					if (maxLength[i] > 0)
					{
						//now put it into the string, keeping things like nulls and such.  (up to you to pull it out right though)
						s.resize(maxLength[i]);

						if (!row[i])
						{
							//it's null?!
#ifdef _DEBUG
							LogMsg("SQL Warning: Something is NULL?!");
#endif
						} else
						{
							memcpy((void*)s.c_str(), &row[i][0], maxLength[i]);

						}
					}
				}
				break;

			case FIELD_TYPE_STRING:
				db.GetVar(fieldNames[i])->Set(string(row[i]));
			break;
			default:;
				assert(!"Unknown mysql type");
				db.GetVar(fieldNames[i])->Set(string(row[i]));

			}

		}
	}
	//printf("\n");

	mysql_free_result(result);

  return rows;
}

bool MySQLManager::Init(string name, string password)
{
	LogMsg("MySQL client version: %s", mysql_get_client_info());
	Kill();
	m_bLostServerConnection = false;
	//store these so we can re-connect ourselves if needed

	m_conn = mysql_init(NULL);
	
	if (!m_conn)
	{
		ShowError();
		return false;
	}

	//actually connect?

	if (!mysql_real_connect(m_conn, "localhost", name.c_str(), password.c_str(), NULL, 0, NULL, 0))
	{
		ShowError();
		return false;
	}

	return true;
}

bool MySQLManager::Query( string query, bool bShowError )
{
#ifdef _DEBUG
	//LogMsg("Queting %s", query.c_str() );
#endif
	if (mysql_query(m_conn, query.c_str()))
	{
		if (bShowError)
		{
			int error = ShowError();
			if (error == 2006) //this should be CR_SERVER_GONE_ERROR, but I can't find the define..
			{
				//let our user know something is desparately wrong.  mysql service probably died, requiring a restart
				m_bLostServerConnection = true;
			}
			
		}
		return false;
	}

	return true;
}


int MySQLManager::GetLastAutoIncrementInsertID()
{
	return (int)mysql_insert_id(m_conn);
}

void MySQLManager::Update()
{

	if (m_conn && m_pingTimer < GetSystemTimeTick() || GetSystemTimeTick() > m_pingTimer+C_MYSQL_PING_TIMER_MS )
	{
		//keep the DB connection alive, if there were no accesses it can time-out
		//LogMsg("Ping! pingtimer is %u, system is %u.  Internval is %d", m_pingTimer, GetSystemTimeTick(), C_MYSQL_PING_TIMER_MS);
		DoesTableExist("BogusTable", false);
		m_pingTimer = GetSystemTimeTick()+uint32(C_MYSQL_PING_TIMER_MS);
	}
}

std::string MySQLManager::EscapeString( const string &input )
{
	char *pBuffer = new char[input.length()*2+1];

	mysql_real_escape_string(m_conn, pBuffer, input.c_str(), (unsigned long)input.size());

	string ret = pBuffer;
	SAFE_DELETE_ARRAY(pBuffer);

	return ret;
}

int MySQLManager::GetTableRecordCount( string tableName )
{
	string sql = "SELECT COUNT(*) FROM "+tableName;
	if (!Query(sql, true))
	{
		LogError("(Couldn't GetTableRecordCount)");
		return 0;
	}
	vector<VariantDB> vdb;
	AddSelectResults(vdb);
	return vdb[0].GetVar("COUNT(*)")->GetINT32();
}