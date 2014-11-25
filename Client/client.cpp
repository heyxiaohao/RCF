/*
需要添加 RCF/include   boost/include
工程需要引入 RCF.cpp
如果是win32  需要在项目属性定义 WIN32_LEAN_AND_MEAN
*/
#include <iostream>
#include <vector>
using namespace std;

#include <RCF/RCF.hpp>

#include <SF/vector.hpp> 

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
	RCF_METHOD_R1(int, Print, const vector<string> &)
	RCF_METHOD_V2(void, Print, const vector<string> &, int &)
RCF_END(I_HelloWorld)

int main()
{
	RCF::RcfInitDeinit rcfInit;
	cout << "调用服务器端的 Print 函数" << endl;
	try {
		// RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		// RcfClient<I_HelloWorld> client(RCF::TcpEndpoint("127.0.0.1", 50001));
		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint("wangh", 50001)); // 如果服务器监听的是IPv6地址，则可以使用服务器名建立连接
		// RcfClient<I_HelloWorld> client(RCF::UdpEndpoint("192.168.0.37", 50001)); // Udp
		// RcfClient<I_HelloWorld> client(RCF::Win32NamedPipeEndpoint(_T("Pipe")));

		// Http
// 		RcfClient<I_HelloWorld> client(RCF::HttpEndpoint("server1.acme.com", 80));
// 		client.getClientStub().setHttpProxy("proxy.acme.com");
// 		client.getClientStub().setHttpProxyPort(8080);

		// Https
// 		RcfClient<I_HelloWorld> client(RCF::HttpEndpoint("server1.acme.com", 443));
// 		client.getClientStub().setHttpProxy("proxy.acme.com");
// 		client.getClientStub().setHttpProxyPort(8080);

		// 客户端代理
		RCF::ClientStub &clientStub = client.getClientStub();
		// 最重要的设置 连接超时时间和远程调用超时时间
		clientStub.setConnectTimeoutMs(2000);
		clientStub.setRemoteCallTimeoutMs(2000);
		// 如果服务器设置了传输协议，则客户端必须选择一种
		clientStub.setTransportProtocol(RCF::Tp_Ntlm); // 此时客户端使用NTLM协议向服务器证明身份及连接进行加解密
		// clientStub.setTransportProtocol(RCF::Tp_Kerberos);
		// NTLM或者提供确切的证明
		clientStub.setUsername(_T("wangh"));
		clientStub.setPassword(_T("123456"));
		// Kerberos协议
		// clientStub.setKerberosSpn(_T("1234"));
		
		// 远程调用
		client.Print("我是客户端1");
		client.Print("我是客户端2");
		client.Print("我是客户端3");

		vector<string> strToPrint;
		strToPrint.push_back("AAA");
		strToPrint.push_back("BBB");
		strToPrint.push_back("王");
		int howManyChars = client.Print(strToPrint);
		cout << "howManyChars: " << howManyChars << endl;

		int how = 0;
		client.Print(strToPrint, how);
		cout << how << endl;

	}
	catch (RCF::Exception &e)
	{
		cout << "ErrorId: " << e.getErrorId() << endl; // 错误码
		cout << "Error: " << e.getErrorString() << endl; // 错误信息
	}
	

	cin.get();
	return 0;
}