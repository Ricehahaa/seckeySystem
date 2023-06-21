#pragma once
#include "TcpSocket.h"
#include "SecKeyShm.h"
#include "TcpServer.h"
#include "RequestCodec.h"
#include "mysqlop.h"
#include <map>
#include<string>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include"registerDB.h"

class ServerInfo
{
public:
	char			serverID[12];	// 服务器端编号
	char			dbUse[24]; 		// 数据库用户名
	char			dbName[24]; 		// 数据库名
	char			dbPasswd[24]; 	// 数据库密码
	char			dbSID[24]; 		// 数据库sid(ip)
	unsigned short 	dPort;  //数据库的端口

	unsigned short 	sPort;			// 服务器绑定的端口
	int				maxnode;		// 共享内存最大网点树 客户端默认1个
	int 			shmkey;			// 共享内存keyid 创建共享内存时使用	 
};
//捕获信号
//void sighandler(int signo);
class ServerOperation
{
public:
	ServerOperation(ServerInfo *info);
	~ServerOperation();

	// 服务器开始工作
	void startWork();
	// 秘钥协商
	int secKeyAgree(RequestMsg* reqmsg, char** outData, int& outLen);
	// 秘钥校验
	int secKeyCheck();
	// 秘钥注销
	int secKeyRevoke(RequestMsg* reqmsg);
	// 秘钥查看
	int secKeyView();
	
	friend void* working(void * arg);
	// 线程回调也可使用静态成员函数
	static void* wrokingHard(void* arg);
	static void catchSignal(int num);

private:
	void getRandString(int len, char* randBuf);

private:
	ServerInfo m_info;
	SecKeyShm* m_shm;
	TcpServer m_server;
	TcpSocket* m_client;
	
	MYSQLOP m_mysql;//记录客服端id，服务端id，密钥及其id的数据库
	
	std::map<pthread_t, TcpSocket*> m_listSocket;
	
	
	registerDB m_regDB;//只记录注册的客服端id
	
	static bool m_stop;
};

void* working(void * arg);

