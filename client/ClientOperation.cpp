#include "ClientOperation.h"
#include "RequestCodec.h"
#include <string.h>
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include "CodecFactory.h"
#include "RequestFactory.h"
#include "RespondFactory.h"

using namespace std;


ClientOperation::ClientOperation(ClientInfo * info)
{
	memcpy(&m_info, info, sizeof(ClientInfo));

	//创建共享内存
	m_shm = new SecKeyShm(m_info.shmKey, m_info.maxNode);
	
}

ClientOperation::~ClientOperation()
{
	//close(m_socket);
}


int ClientOperation::secKeyAgree()
{
	//准备请求数据 
	RequestMsg req;
	memset(&req, 0x00, sizeof(RequestMsg));
	req.cmdType = RequestCodec::NewOrUpdate;
	strcpy(req.clientId, m_info.clientID);
	strcpy(req.serverId, m_info.serverID);
	getRandString(sizeof(req.r1), req.r1);
	//使用hmac函数生成哈希值----消息认证码
	char key[64];
	unsigned int len;
	unsigned char md[SHA256_DIGEST_LENGTH];
	memset(key, 0x00, sizeof(key));
	sprintf(key, "@%s+%s@", req.serverId, req.clientId);
	HMAC(EVP_sha256(), key, strlen(key), (unsigned char *)req.r1, strlen(req.r1), md, &len);
	cout<<"md:"<<md<<endl;
	cout<<SHA256_DIGEST_LENGTH<<endl;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&req.authCode[2 * i], "%02x", md[i]);
	}
	cout << "key:" << key << endl;
	cout << "r1:" << req.r1 << endl;
	cout << "authCode:" << req.authCode << endl;


	//将要发送的数据进行编码
	int dataLen;
	char *outData = NULL;
	
	CodecFactory *factory = new RequestFactory(&req);

	Codec *pCodec = factory->createCodec();
	pCodec->msgEncode(&outData, dataLen);
	printf("sile?????\n");
	//delete factory;
	//delete pCodec;

	//连接服务端
	m_socket.connectToHost(m_info.serverIP, m_info.serverPort);

	//发送请求数据给服务端
	m_socket.sendMsg(outData, dataLen);

	//等待接收服务端的应答
	char *inData;
	m_socket.recvMsg(&inData, dataLen);

	//解码
	factory = new RespondFactory();
	pCodec = factory->createCodec();
	RespondMsg *pMsg = (RespondMsg *)pCodec->msgDecode(inData, dataLen);

	//判断服务端是否成功
	if (pMsg->rv == -1)
	{
		cout << "秘钥协商失败" << endl;
		return -1;
	}
	else
	{
		cout << "秘钥协商成功" << endl;
	}

	//将服务端的r2和客户端的r1拼接生成秘钥
	char buf[1024];
	unsigned char md1[SHA_DIGEST_LENGTH];
	memset(md1, 0x00, sizeof(md1));
	char seckey[SHA_DIGEST_LENGTH*2+1];
	memset(buf, 0x00, sizeof(buf));
	memset(seckey, 0x00, sizeof(seckey));
	sprintf(buf, "%s%s", req.r1, pMsg->r2);
	cout<<"r1:"<<req.r1<<endl;
	cout<<"r2:"<<pMsg->r2<<endl;
	SHA1((unsigned char *)buf, strlen((char *)buf), md1);
	for(int i=0; i<SHA_DIGEST_LENGTH; i++)
	{ 
		sprintf(&seckey[i*2], "%02x", md1[i]);
	}
	cout << "秘钥: " << seckey << endl;

	//给秘钥结构体赋值
	NodeSHMInfo node;
	memset(&node, 0x00, sizeof(NodeSHMInfo));
	node.status = 0;
	strcpy(node.seckey, seckey);
	strcpy(node.clientID, m_info.clientID);
	strcpy(node.serverID, m_info.serverID);
	node.seckeyID = pMsg->seckeyid;

	//将秘钥信息写入共享内存
	m_shm->shmWrite(&node);

	//关闭网络连接
	m_socket.disConnect();

	//释放资源
	delete factory;
	delete pCodec;

	return 0;
}

// char randBuf[64]; , 参数 64, randBuf
void ClientOperation::getRandString(int len, char * randBuf)
{
	int flag = -1;

	// 随机字符串: A-Z, a-z, 0-9, 特殊字符(!@#$%^&*()_+=)
	char chars[] = "!@#$%^&*()_+=";
	for (int i = 0; i < len-1; ++i)
	{
		flag = rand() % 4;
		switch (flag)
		{
		case 0:
			randBuf[i] = rand() % 26 + 'A';
			break;
		case 1:
			randBuf[i] = rand() % 26 + 'a';
			break;
		case 2:
			randBuf[i] = rand() % 10 + '0';
			break;
		case 3:
			randBuf[i] = chars[rand() % strlen(chars)];
			break;
		default:
			break;
		}
	}
	randBuf[len - 1] = '\0';
}

//view查看本地共享内存的密钥
int ClientOperation::secKeyView()
{
	//int SecKeyShm::shmRead(const char * clientID, const char * serverID, NodeSHMInfo * pNodeInfo);
	NodeSHMInfo node;
	memset(&node, 0x00, sizeof(NodeSHMInfo));
	int ret=m_shm->shmRead(m_info.clientID,m_info.serverID,&node);
	if(ret==-1)
	{
		printf("do not have such seckey\n");
		return false;
	}
	printf("clientID:%s\n",node.clientID);
	printf("serverID:%s\n",node.serverID);
	printf("seckey:%s\n",node.seckey);
	return true;
}




//删除本地共享内存密钥和对应服务器上的密钥
int ClientOperation::secKeyRevoke() 
{
	//准备请求数据 
	RequestMsg req;
	memset(&req, 0x00, sizeof(RequestMsg));
	req.cmdType = RequestCodec::Revoke;
	strcpy(req.clientId, m_info.clientID);
	strcpy(req.serverId, m_info.serverID);
	
	cout<<"req.clientId:"<<req.clientId<<endl;
	cout<<"req.serverId:"<<req.serverId<<endl;
	
	//将要发送的数据进行编码
	int dataLen;
	char *outData = NULL;
	CodecFactory *factory = new RequestFactory(&req);
	Codec *pCodec = factory->createCodec();
	pCodec->msgEncode(&outData, dataLen);
	delete factory;
	delete pCodec;

	//连接服务端
	m_socket.connectToHost(m_info.serverIP, m_info.serverPort);

	//发送删除数据给服务端
	m_socket.sendMsg(outData, dataLen);
	
	
	//等待接收服务端的应答
	char *inData;
	m_socket.recvMsg(&inData, dataLen);

	//解码
	factory = new RespondFactory();
	pCodec = factory->createCodec();
	RespondMsg *pMsg = (RespondMsg *)pCodec->msgDecode(inData, dataLen);
	
	//释放资源
	delete factory;
	delete pCodec;

	//判断服务端是否成功
	if (pMsg->rv == -1)
	{
		cout << "删除服务器密钥失败" << endl;
		return -1;
	}
	else
	{
		cout << "删除服务器密钥成功" << endl;
	}
	
	m_socket.disConnect();
	
	//清除本地共享内存的密钥
	int ret=m_shm->shmDelete(m_info.clientID,m_info.serverID);
	
	if(ret)
	{
		return true;
	}
		
	return false;
}