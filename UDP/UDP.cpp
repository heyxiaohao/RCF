/*
TCP: RCF::TcpEndpoint()
UDP: RCF::UdpEndpoint()
另外UDP还提供额为的功能，处理多播和广播
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
		cout << "服务器: " << s << endl;
	}
};

int main()
{
	try{
		RCF::init();
		// 服务器仍然需要制定本地网用于监听
		// RCF::RcfServer server(RCF::UdpEndpoint("0.0.0.0", 50001).listenOnMulticast("232.5.5.5"));
		RCF::RcfServer server(RCF::UdpEndpoint("0.0.0.0", 50001));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		//RcfClient<I_HelloWorld> client(RCF::UdpEndpoint("232.5.5.5", 50001));
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