/*
RCF::PfxCertificate类  读取证书  .pfx / .p12文件
或者从windows证书管理区读取
RCF::StoreCertificate类
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/Win32Certificate.hpp>

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
		RCF::RcfServer server(RCF::TcpEndpoint(50001));
		// 读取证书文件
		RCF::CertificatePtr serverCertPtr(new RCF::PfxCertificate(string("c:/server.p12"), _T("password"), _T("name")));
		server.setCertificate(serverCertPtr);
		// 从证书管理区读取
		RCF::CertificatePtr serverCerPtr1(new RCF::StoreCertificate(RCF::Cl_LocalMachine, RCF::Cs_My, _T("name")));


		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		client.Print("我是客户端"); 
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}