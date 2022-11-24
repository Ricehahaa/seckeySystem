#pragma once
#include <string>
#include<mysql/mysql.h>
#include "SecKeyShm.h"
using namespace std;

class registerDB
{
public:
	registerDB();
	~registerDB();

	// 初始化环境连接数据库
	bool connectDB(const char* user, const char* passwd, const char* connstr,const char* db_name,unsigned short port);
	// 得到keyID
//	int getKeyID();
//	bool updataKeyID(int keyID);
	//bool writeSecKey();	
	bool judgeifExit(const char* client);
	void closeDB();

private:
	MYSQL *mysql;
	string dbname;
	//Environment* m_env;
	//Connection* m_conn;
	
};