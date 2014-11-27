/*
用于配置每个客户端允许传入服务器的最大消息字节数
如果不设置，则请求可能是任意的字节数，造成客户端分配的内存空间混乱
默认的是 1Mb	I_ServerTransport::setMaxIncomingMessageLength() 改变其大小
			I_ClientTransport::setMaxIncomingMessageLength()
客户端只从一个传输层接受数据，相比服务器较干净
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_R1(string, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	string Print(const string &s)
	{
		cout << "Print服务器: " << s << endl;
		return s;
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
		// 服务器设置最大输入数据长度
		server.getServerTransport().setMaxIncomingMessageLength(5 * 1024 * 1024); // 5Mb
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		// 客户端
		client.getClientStub().getTransport().setMaxIncomingMessageLength(5 * 1024 * 1024);
		string s = client.Print("我是客户端");
		cout << "客户端: " << s << endl;

		// 获取前一个调用的请求和响应的数据大小
		RCF::ClientTransport &transport = client.getClientStub().getTransport();
		size_t requestSize = transport.getLastRequestSize();
		size_t responseSize = transport.getLastResponseSize();
		cout << "requestSize: " << requestSize << endl;
		cout << "responseSize: " << responseSize << endl;
		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}

	return 0;
}