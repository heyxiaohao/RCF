/*
服务器端分发 客户端的远程调用 响应

需要添加 RCF/include   boost/include
工程需要引入 RCF.cpp
如果是win32  需要在项目属性定义 WIN32_LEAN_AND_MEAN
*/


#include <iostream>
#include <vector>
using namespace std;

#include <RCF/RCF.hpp>
#include <SF/vector.hpp> // vector的序列化实现类
/************************************************************************/
// 接口定义
/************************************************************************/
RCF_BEGIN(I_HelloWorld, "I_HelloWorld") // (编译时接口标识, 运行时接口标识)
	RCF_METHOD_V1(void, Print, const string &) // 定义一个参数，返回值为void的接口
	RCF_METHOD_R1(int, Print, const vector<string> &)// 定义一个参数，返回值为string的接口
	RCF_METHOD_V2(void, Print, const vector<string> &, int &)
RCF_END(I_HelloWorld)
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// 会话信息 
/************************************************************************/
class HelloWorldSession
{
public:
	HelloWorldSession(){ m_CallCount = 0; cout << "Created HelloWorldSession object." << endl; }
	~HelloWorldSession() { cout << "Destroyed HelloWorldSession object." << endl; }
	size_t m_CallCount;
};
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// 接口实现 
/************************************************************************/
class HelloWorld
{
public:
	void Print(const string &s)
	{
		// 每一个客户端都有自己的session，生命期为底层协议的生命期
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		// 可以取回客户端使用的协议
		RCF::TransportProtocol protocol = session.getTransportProtocol();
		if (protocol == RCF::Tp_Ntlm || protocol == RCF::Tp_Kerberos)
		{
			RCF::tstring clientUserName = session.getClientUsername();
			// 模拟客户端 需要在RCF.hpp 添加 SspiFilter.hpp
			RCF::SspiImpersonator impersonator(session);
			// ...
		}

		HelloWorldSession &hwSession = session.getSessionObject<HelloWorldSession>(true);
		++hwSession.m_CallCount;

		//throw runtime_error("Print() can't be called");
		cout << "服务器: " << s << endl;
		cout << "总共调用次数: " << hwSession.m_CallCount << endl;
	}
	int Print(const vector<string> &v)
	{
		int howManyChars = 0;
		for (size_t i = 0; i < v.size(); ++i)
		{
			cout << "服务器: " << v[i] << endl;
			howManyChars += v[i].size();
		}
		return howManyChars;
	}
	void Print(const vector<string> &v, int &howManyChars)
	{
		howManyChars = 0;
		for (size_t i = 0; i < v.size(); ++i)
		{
			cout << "服务器: " << v[i] << endl;
			howManyChars += v[i].size();
		}
	}
};
//////////////////////////////////////////////////////////////////////////

int main()
{
	RCF::RcfInitDeinit rcfInit;
	HelloWorld helloWorld;
	// RCF::RcfServer server(RCF::TcpEndpoint(50001)); // 如果不提供地址，则默认使用127.0.0.1
	// RCF::RcfServer server(RCF::TcpEndpoint("127.0.0.1", 50001));
	// RCF::RcfServer server(RCF::TcpEndpoint("0.0.0.0", 50001)); // 监听所有地址，IPv4   RCF::RcfServer server(RCF::TcpEndpoint("::0", 50001)); // IPv6
	// RCF::RcfServer server(RCF::UdpEndpoint("0.0.0.0", 50001)); // Udp  不能使用 twoway （request/response）
	// RCF::RcfServer server(RCF::Win32NamedPipeEndpoint(_T("Pipe"))); // 需要在头文件RCF.hpp 在windows平台使用windows命名管道， 在Unix平台使用本地套接字 

	RCF::RcfServer server;
	// 同时指定
	server.addEndpoint(RCF::TcpEndpoint("0.0.0.0", 50001));
//	server.addEndpoint(RCF::TcpEndpoint("::0", 50001)); // 在有些系统，此方式可同时监听IPv4和IPv6
	// 或三种模式
	server.addEndpoint(RCF::TcpEndpoint("::0", 50001));
	server.addEndpoint(RCF::UdpEndpoint("::0", 50002));
	server.addEndpoint(RCF::Win32NamedPipeEndpoint(_T("Pipe")));

	// 可选的设置传输协议，对应的客户端需要设置其中一种协议
// 	vector<RCF::TransportProtocol> protocols;
// 	protocols.push_back(RCF::Tp_Ntlm);
// 	protocols.push_back(RCF::Tp_Kerberos);
// 	server.setSupportedTransportProtocols(protocols);

	// RCF::RcfServer server(RCF::HttpEndpoint("0.0.0.0", 80)); // http
	
	// Https  需要在RCF.hpp 中添加 Win32Certificate.hpp
	// RCF::RcfServer server(RCF::HttpsEndpoint("0.0.0.0", 443));
	server.bind<I_HelloWorld>(helloWorld);  // 服务器绑定到接口实现对象
	// RCF::CertificatePtr serverCertPtr(new RCF::PfxCertificate("path/to/certificate.p12", _T("password"), _T("CertificateName")));
	// server.setCertificate(serverCertPtr);
	
	server.start();

	cout << "任意键退出..." << endl;
	cin.get();

	server.stop(); // server在超出作用域则自动stop
	return 0;
}

/************************************************************************/
// RCF支持 NTLM Kerberos SSL 传输协议，前两者仅支持 windows, SSL支持所有平台
/************************************************************************/