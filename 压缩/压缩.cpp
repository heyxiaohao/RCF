/*
需要在预处理器中定义 RCF_USE_ZLIB
如使用静态库 则需定义 RCF_ZLIB_STATIC(测试结果不可用）
ClientStub::setEnableCompression()
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
		// 获取当前连接的会话
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		// 发送和收到的字节数
		boost::uint64_t totalBytesReceived = session.getTotalBytesReceived();
		cout << totalBytesReceived << endl;
		boost::uint64_t totalBytesSent = session.getTotalBytesSent();
		cout << totalBytesSent << endl;
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
		client.getClientStub().setTransportProtocol(RCF::Tp_Ntlm);
		client.Print("未压缩");
		client.getClientStub().setEnableCompression(true);
		client.Print("压缩");

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}