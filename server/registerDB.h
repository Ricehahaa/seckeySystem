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

	// ��ʼ�������������ݿ�
	bool connectDB(const char* user, const char* passwd, const char* connstr,const char* db_name,unsigned short port);
	// �õ�keyID
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