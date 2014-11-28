/*
使用win32命名管道允许容易的验证客户端的身份，通过 RCF::Win32NamedPipeImpersonator类 它使用Win32API ImpersonateNamedPipeClient()模拟客户端
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/Win32NamedPipeEndpoint.hpp>
#include <RCF/Win32NamedPipeServerTransport.hpp>
#include <RCF/Win32Username.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_R1(string, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	string Print(const string &s)
	{
		cout << "服务器: " << s << endl;
		// 模拟客户端
		RCF::Win32NamedPipeImpersonator impersonator;
		wcout << "客户端的名字: " << RCF::getMyUserName() << endl;
		return s;
	}
};

int main()
{
	try{
		RCF::init();
		RCF::RcfServer server(RCF::Win32NamedPipeEndpoint(_T("PiPe")));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();


		RcfClient<I_HelloWorld> client(RCF::Win32NamedPipeEndpoint(_T("PiPe")));
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