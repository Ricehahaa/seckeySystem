#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "SecKeyShm.h"
#include <unistd.h>
using namespace std;

SecKeyShm::SecKeyShm(int key): ShareMemory(key)
{
	
}

SecKeyShm::SecKeyShm(int key, int maxNode): 
	ShareMemory(key, sizeof(int) + maxNode*sizeof(NodeSHMInfo)), 
	m_maxNode(maxNode)
{
	//连接共享内存
	void *p = mapShm();
	
	//cout<<"size::"<<sizeof(int) + maxNode*sizeof(NodeSHMInfo)<<endl;
	//将maxNode写入共享内存的前四个字节
	//sleep(10000);
	//cout<<m_maxNode<<endl;
	//Sleep(10000);
	memcpy(p, &m_maxNode, sizeof(m_maxNode));
	//sleep(10000);
	//初始化结构体元素
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p+sizeof(int));
	memset(pNode, 0x00, sizeof(NodeSHMInfo)*m_maxNode);
	cout<<"创建之时:m_maxNode:"<<maxNode<<endl;
	//断开与共享内存的关联
	unmapShm();
}

SecKeyShm::SecKeyShm(const char* pathName): ShareMemory(pathName)
{
	
}

SecKeyShm::SecKeyShm(const char* pathName, int maxNode):
	ShareMemory(pathName, sizeof(int) + maxNode*sizeof(NodeSHMInfo)), 
	m_maxNode(maxNode)
{
	//连接共享内存
	void *p = mapShm();
	
	
	//将maxNode写入共享内存的前四个字节
	memcpy(p, &m_maxNode, sizeof(int));
	
	//初始化结构体元素
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p+sizeof(int));
	memset(pNode, 0x00, sizeof(NodeSHMInfo)*m_maxNode);
	
	//断开与共享内存的关联
	unmapShm();
}

SecKeyShm::~SecKeyShm()
{
	
}

int SecKeyShm::shmWrite(NodeSHMInfo* pNodeInfo)
{
	int ret = 0;
	
	//连接共享内存
	void * p = mapShm();
	
	//获得共享内存大小--头4个字节表示共享内存大小
	memcpy(&m_maxNode, p, sizeof(int));
	//cout << "m_maxNode=="<< m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p + sizeof(int));
	cout<<"m_maxNode:"<<m_maxNode<<endl;
	//先查找释放已经存在过
	int i = 0;
	for(i=0; i<m_maxNode; i++)
	{
		if(strcmp(pNode[i].clientID, pNodeInfo->clientID)==0 && 
		   strcmp(pNode[i].serverID, pNodeInfo->serverID)==0)
		{
			//将秘钥信息写入到这个位置
			memcpy(&pNode[i], pNodeInfo, sizeof(NodeSHMInfo));
			break;
		}
	}
	//cout << "i==" << i << endl;
	
	//若没有找到原来的, 则找一个空闲位置写入
	NodeSHMInfo tmpNode;
	memset(&tmpNode, 0x00, sizeof(tmpNode));
	if(i==m_maxNode)
	{
		for(i=0; i<m_maxNode; i++)
		{
			if(memcmp(&pNode[i], &tmpNode, sizeof(NodeSHMInfo))==0)
			{
				cout<<"fuwu端共享内存写入成功\n"<<endl;
				cout<<"写入时:"<<pNodeInfo->clientID<<"  "<<pNodeInfo->serverID<<endl;
				memcpy(&pNode[i], pNodeInfo, sizeof(NodeSHMInfo));
				break;
			}
		}
	}
	//cout << "i==" << i << endl;
	
	
	//没有空间可用	
	if(i==m_maxNode)
	{
		cout << "no space to left" << endl;
		ret = -1;
	}
	//断开与共享内存的关联
	unmapShm();
	
	return ret;
}

int SecKeyShm::shmRead(const char* clientID, const char* serverID, NodeSHMInfo* pNodeInfo)
{
	int ret = 0;
	
	//连接共享内存
	void * p = mapShm();
	
	//获得共享内存大小--头4个字节表示共享内存大小
	memcpy(&m_maxNode, p, sizeof(int));
	//cout << "m_maxNode=="<< m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p + sizeof(int));
	
	//cout << "clientID:" << clientID << endl;
	//cout << "serverID:" << serverID << endl;
	
	int i = 0;
	for(i=0; i<m_maxNode; i++)
	{
		if(strcmp(pNode[i].clientID, clientID)==0 && 
		   strcmp(pNode[i].serverID, serverID)==0)
		{
			//将秘钥信息写入到这个位置
			memcpy(pNodeInfo, &pNode[i], sizeof(NodeSHMInfo));
			break;
		}
	}
	//cout << "i==" << i << endl;
	
	//没有空间可用	
	if(i==m_maxNode)
	{
		cout << "not found data" << endl;
		ret = -1;
	}
	//断开与共享内存的关联
	unmapShm();
	
	return ret;
}

//删除共享内存中指定的密钥
int SecKeyShm::shmDelete(const char* clientID, const char* serverID)
{
	int ret = 0;
	
	//连接共享内存
	void * p = mapShm();
	
	//获得共享内存大小--头4个字节表示共享内存大小
	memcpy(&m_maxNode, p, sizeof(int));
	cout<<"m_maxNode:"<<m_maxNode<<endl;
	//cout << "m_maxNode=="<< m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p + sizeof(int));
	
	cout<<"fuwu共享内存删除时\n"<<endl;
	cout<<"删除时:"<<clientID<<"  "<<serverID	<<endl;
	//cout << "clientID:" << clientID << endl;
	//cout << "serverID:" << serverID << endl;
	
	int i = 0;
	for(i=0; i<m_maxNode; i++)
	{
		if(strcmp(pNode[i].clientID, clientID)==0 && 
		   strcmp(pNode[i].serverID, serverID)==0)
		{
			//清空这个位置
			memset(&pNode[i], 0x00, sizeof(NodeSHMInfo));
			printf("success in deleting\n");
			break;
		}
	}
	
	//cout << "i==" << i << endl;
	
	//未找到	
	if(i==m_maxNode)
	{
		cout << "not found data" << endl;
		ret = -1;
	}
	//断开与共享内存的关联
	unmapShm();
	
	return ret;
}


void SecKeyShm::printShm()
{
	//连接共享内存
	void * p = mapShm();
	
	//获得共享内存大小--头4个字节表示共享内存大小
	memcpy(&m_maxNode, p, sizeof(int));
	cout << "m_maxNode=="<< m_maxNode << endl;
	
	NodeSHMInfo *pNode = (NodeSHMInfo *)((char *)p + sizeof(int));
	for(int i=0; i<m_maxNode; i++)
	{
		cout << "--------" << i << "-------" << endl;
		cout << "status:" <<pNode[i].status << endl;
		cout << "seckeyID:" << pNode[i].seckeyID << endl;
		cout << "clientID:" << pNode[i].clientID << endl;
		cout << "serverID:" << pNode[i].serverID << endl;
		cout << "seckey:" << pNode[i].seckey << endl;
		cout << "--------------------" << endl;
	}
	
	//断开与共享内存的关联
	unmapShm();
	
	return;
}
