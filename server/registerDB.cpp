#include "mysqlop.h"
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include"registerDB.h"
using namespace std;

registerDB::registerDB()
{
	
}


registerDB::~registerDB()
{
	this->closeDB();
}
void registerDB::closeDB()
{
		mysql_close(mysql);
}

bool registerDB::connectDB(const char* user, const char* passwd, const char* connstr,const char* db_name,unsigned short port)
{
	// 1. ��ʼ������
	
	mysql=mysql_init(NULL);
	//sleep(10000);
	if(mysql==NULL)
	{
			cout<<"Init Errpr:"<<mysql_error(mysql);
			return false;
	}
	
	//m_env = Environment::createEnvironment(Environment::DEFAULT);
	// 1. ��ʼ������
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
	this->dbname=db_name;
	cout << "creat success" << endl;
	return true;
}

bool registerDB::judgeifExit(const char* client)
{
	//��֯����ѯ��sql����
	char sql[1024]={0};
	sprintf(sql,"select clientid from registerDB where clientid = '%s' ",client);
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
	//��ȡÿһ�У����û��һ�оͲ������while��
	while((row=mysql_fetch_row(results)))
	{
			return true;
	}
	
	return false;
}
/*
int MYSQLOP::getKeyID()
{
	// ��ѯ���ݿ�
	string sql = "select ikeysn from SECKEYINFO for update";
	Statement* stat = m_conn->createStatement(sql);
	// ִ�����ݲ�ѯ
	ResultSet* resSet = stat->executeQuery();
	// ִ�����ݲ�ѯ
	int keyID = -1;
	if (resSet->next())
	{
		keyID = resSet->getInt(1);
	}
	m_conn->terminateStatement(stat);

	return keyID;
}*/
/*
//��ԿID�ڲ����ʱ����Զ�����, Ҳ�����ֶ�����
bool MYSQLOP::updataKeyID(int keyID)
{
	// �������ݿ�
	string sql = "update SECMNG.KEYSN set ikeysn = " + to_string(keyID);
	Statement* stat = m_conn->createStatement(sql);
	// �����Զ��ύ
	stat->setAutoCommit(true);	
	// ִ�����ݲ�ѯ
	int ret = stat->executeUpdate();
	m_conn->terminateStatement(stat);
	if (ret == 0)
	{
		return false;
	}
	return true;
}*/


