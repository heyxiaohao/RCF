/*
底层与服务器的连接或取消连接一般是自动的，也可以手动
client.getClientStub().connect();
client.getClientStub().disconnect();
可以将一个客户端的底层协议移动到另一个客户端
ClientStub::releaseTransport()    ClientStub::setTransport();
应用：在不同的客户端实例使用同一个底层传输协议
*/


#include <iostream>

using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
	RCF_METHOD_V1(void, AnotherPrint, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const string &s)
	{
		cout << "Print服务器: " << s << endl;
	}
	void AnotherPrint(const string &s)
	{
		cout << "AnotherPrint服务器: " << s << endl;
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

		RCF::RcfServer server(RCF::TcpEndpoint(50001));
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		RCF::ClientStub &clientStub = client.getClientStub();
		RcfClient<I_HelloWorld> client2(client.getClientStub().releaseTransport());
		client2.AnotherPrint("client2");
		clientStub.setTransport(client2.getClientStub().releaseTransport());
		client.Print("client");

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}