/*
激活IPv6需要定义宏 RCF_USE_IPv6
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
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		cout << "IPv4/IPv6: " << session.getClientAddress().string() << endl;
		cout << "服务器: " << s << endl;
	}
};

int main()
{
	try{
		RCF::init();

		RCF::RcfServer server;
		//server.addEndpoint(RCF::TcpEndpoint("127.0.0.1", 50001)); // IPv4
		//server.addEndpoint(RCF::TcpEndpoint("::1", 50001)); // IPv6
		// 监听两个模式
		server.addEndpoint(RCF::TcpEndpoint("0.0.0.0", 50001));
		server.addEndpoint(RCF::TcpEndpoint("::0", 50001));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		//RcfClient<I_HelloWorld> client(RCF::TcpEndpoint("127.0.0.1", 50001));
		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint("::1", 50001));
		// RCF使用POSIX getaddrinfo()获取IP地址，如果使用域名形式的字符串，则可能是IPv4或IPv6，取决于系统设置
		// RcfClient<I_HelloWorld> client(RCF::TcpEndpoint("machin.domain", 50001));
		// 也可以强制客户端使用哪种协议
//		RCF::IpAddressV4 addr_4("machin.domin", 50001);
//		RcfClient<I_HelloWorld> client((RCF::TcpEndpoint(addr_4))); // 为什么要多加一层括号？？
// 		RCF::IpAddressV6 addr_6("machin.domin", 50001);
// 		RcfClient<I_HelloWorld> client((RCF::TcpEndpoint(addr_6)));
		client.Print("我是客户端1");
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}