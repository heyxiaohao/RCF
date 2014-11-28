/*
RCF 目前支持 NTML Kerberos Negotiate 和 SSL 前三种只能在windows平台，SSL可用于所有平台
支持 zlib 压缩
客户端设置：ClientStub::setTransportProtocol();
(可选）服务器设置：RcfServer::setSupportedTransportProtocols(); // 服务器设置时可以设置一个组
服务器获取：RcfSession::getTransportPtotocol();
*/

/*
NTML是windows早期版本的标准安全协议，win2000及以后都默认使用Kerberos
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
		cout << "服务器: " << s << endl;
		// 可以取回客户端使用的协议
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		RCF::TransportProtocol protocol = session.getTransportProtocol();
		if (protocol == RCF::Tp_Ntlm)
		{
			RCF::tstring clientUserName = session.getClientUsername();
			wcout << "客户端的名字: " << clientUserName << endl;
			// 模拟客户端 需要在RCF.hpp 添加 SspiFilter.hpp
			RCF::SspiImpersonator impersonator(session);
			// ...
		}
		return s;
	}
};

int main()
{
	try{
		RCF::init();
		RCF::RcfServer server(RCF::TcpEndpoint(50002));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();


		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50002));
		client.getClientStub().setTransportProtocol(RCF::Tp_Ntlm);
		string s = client.Print("我是客户端1");
		cout << "客户端: " << s << endl;
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}