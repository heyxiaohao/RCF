/*
客户端，需要有效连接，需要创建RcfServer，并调用RcfServer::createCallbackConnection()
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
		cout << s << endl;
	}
};

int main()
{
	RCF::init();
	try
	{
		RCF::RcfServer callbackServer((RCF::TcpEndpoint())); // 为什么需要空的 还要加两层括号
		HelloWorld helloWorld;
		callbackServer.bind<I_HelloWorld>(helloWorld);
		callbackServer.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		// 创建回调
		RCF::createCallbackConnection(client, callbackServer);
		cout << "任意键退出..." << endl;
		cin.get();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}
	return 0;
}