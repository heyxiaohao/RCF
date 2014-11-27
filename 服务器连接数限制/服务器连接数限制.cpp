/*
限制并发的连接数，对UDP无效，因为UDP没有连接的概念
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

void onConnectCreated()
{
	static int count = 0;
	cout << ++count << endl;
}

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
		// 设置连接数限制
		server.getServerTransport().setConnectionLimit(5);
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}

	return 0;
}