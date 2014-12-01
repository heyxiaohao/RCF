/*
默认客户端与服务器绑定时，使用本地系统配置  IPv4或IPv6  端口随机
可通过I_IpServerTransport::setLocalIp() 改变

应用在什么地方？？？
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/IpClientTransport.hpp>

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
		server.addEndpoint(RCF::TcpEndpoint("0.0.0.0", 50001));
		server.addEndpoint(RCF::TcpEndpoint("::0", 50001));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint("127.0.0.1", 50001));
		// 设置
		RCF::IpClientTransport &ipClientTransport = client.getClientStub().getIpTransport();
		ipClientTransport.setLocalIp(RCF::IpAddress("127.0.1.1", 10100));
		client.getClientStub().connect();
		// 获取，不设置则无法取得
		RCF::IpAddress &localIp = ipClientTransport.getAssignedLocalIp();
		cout << "localinterface: " << localIp.getIp() << endl;
		cout << "port: " << localIp.getPort() << endl;

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