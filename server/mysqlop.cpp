#include "mysqlop.h"
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace std;

MYSQLOP::MYSQLOP()
{
	
}


MYSQLOP::~MYSQLOP()
{
	this->closeDB();
}

bool MYSQLOP::connectDB(const char* user, const char* passwd, const char* connstr,const char* db_name,unsigned short port)
{
	// 1. 初始化环境
	
	mysql=mysql_init(NULL);
	//sleep(10000);
	if(mysql==NULL)
	{
			cout<<"Init Errpr:"<<mysql_error(mysql);
			return false;
	}
	
	//m_env = Environment::createEnvironment(Environment::DEFAULT);
	// 1. 初始化环境
	/*cout<<connstr<<endl;
	cout<<user<<endl;
	cout<<passwd<<endl;
	cout<<db_name<<endl;
	cout<<port<<endl;*/
	mysql = mysql_real_connect(mysql, connstr, user,passwd,db_name,port,NULL,0);
//	mysql = mysql_real_connect(mysql, "192.168.56.99", "root","123456","mydb1",3306,NULL,(unsigned long)0);
	if (mysql == NULL)
	{
		cout<<"connect Error:"<<mysql_error(mysql);
		return false;
	}
	dbname=db_name;
	//strcpy(dbname, db_name);
	cout << "creat success" << endl;
	return true;
}

bool MYSQLOP::judgeifExit(const char* client)
{
	//组织待查询的sql语言
	char sql[1024]={0};
	sprintf(sql,"select clientid from SECKEYINFO where clientid = %s",client);
	int ret = mysql_query(this->mysql,sql);
	//cout<<"ret:"<<ret<<endl;
	//cout<<"row:"<<mysql_field_count(mysql);
	
	if(ret!=0)
	{
		printf("mysql_query_error ,[%s]\n",mysql_error(mysql));
	}
	MYSQL_RES *results = mysql_store_result(mysql);
	if(results==NULL)
	{
		printf("mysql_store_result error,[%s]\n",mysql_error(mysql));
		return false;
	}
	MYSQL_ROW row;
	//读取每一行，如果没有一行就不会进入while。
	while((row=mysql_fetch_row(results)))
	{
			return true;
	}
	
	return false;
}


// 将生成的秘钥写入数据库
// 更新秘钥编号
bool MYSQLOP::writeSecKey(NodeSHMInfo *pNode)
{
	 	// 组织待插入的sql语句
	char sql[1024] = { 0 };
	
	if(!this->judgeifExit(pNode->clientID,pNode->serverID	))
	{
			sprintf(sql, "Insert Into SECKEYINFO(clientid, serverid, keyid, createtime, state, seckey) \
					values ('%s', '%s', %d, curdate() , %d, '%s') ", 
		pNode->clientID, pNode->serverID, pNode->seckeyID, 
		 0, pNode->seckey);
	}
	else
	{
		sprintf(sql, "update SECKEYINFO set clientid ='%s',serverid='%s',keyid=%d,createtime=curdate(),state=%d,seckey='%s' ", 
		pNode->clientID, pNode->serverID, pNode->seckeyID, 
		 0, pNode->seckey);
	}
		
	cout << "sql: " << sql << endl;
	int ret =mysql_query(mysql,sql);
	if(ret!=0)
	{
		printf("mysql_query error,[%s]\n",mysql_error(mysql));
		return false;
	}

	return true;
}
bool MYSQLOP::judgeifExit(const char* client,const char *server)
{
	//组织待查询的sql语言
	char sql[1024]={0};
	sprintf(sql,"select clientid,	serverid from SECKEYINFO where clientid = %s and serverid = %s",client,server);
	int ret = mysql_query(this->mysql,sql);
	//cout<<"ret:"<<ret<<endl;
	//cout<<"row:"<<mysql_field_count(mysql);
	
	if(ret!=0)
	{
		printf("mysql_query_error ,[%s]\n",mysql_error(mysql));
	}
	MYSQL_RES *results = mysql_store_result(mysql);
	if(results==NULL)
	{
		printf("mysql_store_result error,[%s]\n",mysql_error(mysql));
		return false;
	}
	MYSQL_ROW row;
	//读取每一行，如果没有一行就不会进入while。
	while((row=mysql_fetch_row(results)))
	{
			return true;
	}
	
	return false;
}
void MYSQLOP::closeDB()
{
		mysql_close(mysql);
}


bool MYSQLOP::deleteSeckey(const char* client,const char *server)
{
	cout<<"shanchushujuku"<<endl;
	if(!judgeifExit(client,server))
	{
		printf("false no such data\n");
		return false;
	}
	//组织删除的sql语言
	char sql[1024]={0};
	
	sprintf(sql,"delete from SECKEYINFO where clientid = %s and serverid =%s",client,server);
	int ret = mysql_query(this->mysql,sql);
	//cout<<"ret:"<<ret<<endl;
	//cout<<"row:"<<mysql_field_count(mysql);
	
	if(ret!=0)
	{
		printf("mysql_query_error ,[%s]\n",mysql_error(mysql));
		return false;
	}
	return true;
}





