/*
RcfServer 将有1个或多个服务器传输层，但客户端只能有一个
为了获取服务器的传输层，需要捕获addEndpoint()的返回值
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const string &s)
	{
		cout << "Print服务器: " << s << endl;
	}
};

void close()
{
	cout << "任意键退出..." << endl;
	cin.get();
}

int main()
{
	try
	{
		RCF::init();
		HelloWorld helloWorld;

		RCF::RcfServer server;
		// 捕获服务端的传输层
		RCF::ServerTransport &tcpTransport = server.addEndpoint(RCF::TcpEndpoint(50001));
		RCF::ServerTransport &udpTransport = server.addEndpoint(RCF::UdpEndpoint(50001));
		// 如果服务端只有一个传输层，则可以使用getServerTransport()
		// RCF::ServerTransport &transport = server.getServerTransport();
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		// 客户端获取传输层
		RCF::ClientTransport &clientTransport = client.getClientStub().getTransport();
		client.Print("我是客户端");

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}