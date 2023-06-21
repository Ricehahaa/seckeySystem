#include <cstdio>
#include "ServerOperation.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>




using namespace std;
//如果执行CTRL +C OR +Z直接杀死

void sighandler(int signo)
{
		printf("get success!!!!!!!!\n");
		exit(0);
		kill(getpid(),SIGKILL);
}

void createDaemon()
{
	pid_t pid=fork();
	if(pid<0||pid>0)
	{
			exit(0);	
	}
	setsid();
	chdir("/home/wuningfeng/projects/server");
	umask(0000);
	int fd=open("/dev/null",O_RDWR);
	dup2(fd,STDIN_FILENO);
	dup2(fd,STDOUT_FILENO);
	dup2(fd,STDERR_FILENO);
}
int main()
{
	//创建守护进程
	createDaemon();
	
	struct sigaction act;
	act.sa_handler = sighandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGINT,&act,NULL);
	sigaction(SIGTSTP,&act,NULL);
	
	// 设置随机种子
	// 需提前设置，如果在void ClientOperation::getRandString(int len, char * randBuf)里设置就太晚了，以免用户输入过快导致密钥出现重复
	srand(time(NULL));
	
	ServerInfo info;
	strcpy(info.serverID, "0001");
	info.maxnode = 10;
	info.shmkey = 0x12340000;
	strcpy(info.dbUse, "root");
	strcpy(info.dbPasswd, "123456");
	strcpy(info.dbSID, "192.168.56.99");
	strcpy(info.dbName, "mydb1");
	info.sPort = 8888;
	info.dPort=3306;

	ServerOperation server(&info);
		//sleep(100000);
	server.startWork();

    return 0;
}