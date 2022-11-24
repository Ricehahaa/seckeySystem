#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "RequestCodec.h"
#include "RespondCodec.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "ClientOperation.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;
int usage();
//如果执行CTRL +C OR +Z直接杀死
void sighandler(int signo)
{
		printf("get success!!!!!!!!\n");
		exit(0);
		kill(getpid(),SIGKILL);
}
int main()
{
	
	struct sigaction act;
	act.sa_handler = sighandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGINT,&act,NULL);
	sigaction(SIGTSTP,&act,NULL);
	
	// 设置随机种子
	// 需提前设置，如果在void ClientOperation::getRandString(int len, char * randBuf)里设置就太晚了，以免用户输入过快导致密钥出现重复
	srand(time(NULL));
	
	ClientInfo info;
	memset(&info, 0x00, sizeof(ClientInfo));
	
	strcpy(info.clientID, "1111");
	strcpy(info.serverID, "0001");
	strcpy(info.serverIP, "127.0.0.1");
	info.serverPort = 8888;
	info.maxNode = 1;
	info.shmKey = 0x12345678;
	
	//sleep(1000);
	ClientOperation client(&info);
	//sleep(1000);
	//enum CmdType{NewOrUpdate=1, Check, Revoke, View};
	int nSel;
	while (1)
	{
		nSel = usage();
		switch(nSel)
		{
		case RequestCodec::NewOrUpdate:
			client.secKeyAgree();
			break;

		case RequestCodec::Check:
			client.secKeyCheck();
			break;
		case RequestCodec::Revoke:
			client.secKeyRevoke();
			break;
		case RequestCodec::View:
			client.secKeyView();
			break;
		case 0:
		     exit(0);

		default:
			break;
		}
	}	
}

int usage()
{
	int nSel = -1;
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     1.密钥协商                                            */");
	printf("\n  /*     2.密钥校验                                          */");
	printf("\n  /*     3.密钥注销                                            */");
	printf("\n  /*     4.密钥查看                                           */");
	printf("\n  /*     0.退出系统                                        */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  选择:");

	scanf("%d", &nSel);
	while (getchar() != '\n');

	return nSel;
}
