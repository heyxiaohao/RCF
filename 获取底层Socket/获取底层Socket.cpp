/*
获取底层Socket  用于自定义socket配置
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/TcpAsioServerTransport.hpp>
#include <RCF/TcpClientTransport.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const string &s)
	{
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		cout << "IPv4/IPv6: " << session.getClientAddress().string() << endl;

		// 服务器端获取连接的socket
		RCF::SessionState &sessionState = RCF::getCurrentRcfSession().getSessionState();
		RCF::TcpAsioSessionState &tcpSessionState = dynamic_cast<RCF::TcpAsioSessionState &>(sessionState);
		cout << "服务器端socket: " << tcpSessionState.getNativeHandle() << endl;

		cout << "服务器: " << s << endl;
	}
};

int main()
{
	try{
		RCF::init();
	
		RCF::RcfServer server;
		server.addEndpoint(RCF::TcpEndpoint("0.0.0.0", 50001));
		server.addEndpoint(RCF::TcpEndpoint("::0", 50001));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		//client.getClientStub().connect(); // 如果不调用connect() 实际连接会推迟到第一次接口调用
		
		client.Print("我是客户端1");

		// 客户端获取连接的socket
		RCF::TcpClientTransport &transport = dynamic_cast<RCF::TcpClientTransport &>(client.getClientStub().getTransport());
		cout << "客户端Socket: " << transport.getNativeHandle() << endl;
		
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}