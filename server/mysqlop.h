#pragma once
#include <string>
#include<mysql/mysql.h>
//#include"mysql.h"
#include "SecKeyShm.h"
using namespace std;

class MYSQLOP
{
public:
	MYSQLOP();
	~MYSQLOP();

	// ��ʼ�������������ݿ�
	bool connectDB(const char* user, const char* passwd, const char* connstr,const char* db_name,unsigned short port);
	
	//�жϿͷ��˺Ͷ�Ӧ�ķ������Ƿ�����Կ(��������)
	bool judgeifExit(const char* client);
	bool judgeifExit(const char* client,const char *server);
	bool writeSecKey(NodeSHMInfo *pNode);
	bool deleteSeckey(const char* client,const char *server);
	void closeDB();

private:
	MYSQL *mysql;
	string dbname;
	//Environment* m_env;
	//Connection* m_conn;
	
};

