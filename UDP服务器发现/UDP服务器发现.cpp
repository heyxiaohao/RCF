/*
如果服务器是动态的绑定端口，则多播和广播对于客户端与服务器的连接就很有意义
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

// 广播TCP端口 接口，可以广播服务器地址么？
RCF_BEGIN(I_Broadcast, "I_Broadcast")
	RCF_METHOD_V1(void, serverIsRunningOnPort, int)
RCF_END(I_Broadcast)
// 接受广播数据的实现类
class Broadcast
{
public:
	Broadcast() :m_Port(){}
	void serverIsRunningOnPort(int port)
	{
		m_Port = port;
	}
	int m_Port;
};

// 服务器线程，每秒广播一次服务器位置，如果多播地址被禁用，则也可以使用广播地址发送
void broadcastThread(int port, const string &multicastIp, int multicastPort)
{
	RcfClient<I_Broadcast> client(RCF::UdpEndpoint(multicastIp, multicastPort));
	client.getClientStub().setRemoteCallSemantics(RCF::Oneway);
	// 开始广播
	while (true)
	{
		client.serverIsRunningOnPort(port);
		RCF::sleepMs(1000);
	}
	
}

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const string &s)
	{
		cout << "服务器: " << s << endl;
	}
};

int main()
{
	try{
		RCF::init();

		// 启动动态端口的服务器
		RCF::RcfServer server(RCF::TcpEndpoint(0));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();
		// 获取服务器端口
		int port = server.getIpServerTransport().getPort();
		// 开始广播端口
		RCF::ThreadPtr broadcastThreadPtr(new RCF::Thread(boost::bind(&broadcastThread, port, "232.5.5.5", 50001)));

		// 客户端在开始事务前需要先获取广播消息
		Broadcast broadcast;
		RCF::RcfServer clientSizeBroadcastListener(RCF::UdpEndpoint("0.0.0.0", 50001).listenOnMulticast("232.5.5.5"));
		clientSizeBroadcastListener.bind<I_Broadcast>(broadcast);
		clientSizeBroadcastListener.start();
		// 等待广播
		while (!broadcast.m_Port)
		{
			RCF::sleepMs(1000);
		}

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(broadcast.m_Port));
		client.Print("我是客户端");
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}