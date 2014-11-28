/*
udp还可以共享绑定的地址，因此多个server可以监听同一个端口，在多播模式是默认开启的，非多播模式可以设置开启
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
		cout << "服务器HelloWorld: " << s << endl;
	}
};

class HelloWorld2
{
public:
	void Print(const string &s)
	{
		cout << "服务器HelloWorld2: " << s << endl;
	}
};

int main()
{
	try{
		RCF::init();
		// 地址共享
		RCF::RcfServer server(RCF::UdpEndpoint("0.0.0.0", 50001).enableSharedAddressBinding());
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RCF::RcfServer server2(RCF::UdpEndpoint("0.0.0.0", 50001).enableSharedAddressBinding());
		HelloWorld2 helloWorld2;
		server2.bind<I_HelloWorld>(helloWorld2);		
		server2.start();
		

		// 此时广播则两个服务器都会收到消息，非广播则第一个启动的服务器才收到消息（如果是多个服务器
		RcfClient<I_HelloWorld> client(RCF::UdpEndpoint("255.255.255.255", 50001)); // 广播
		client.getClientStub().setRemoteCallSemantics(RCF::Oneway);
		client.Print(/*RCF::Oneway, */"我是客户端1"); // UDP需要指定Oneway 否则会等待回应
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}