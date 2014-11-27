/*
客户端，用于传输层测试
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

int main()
{
	RCF::init();
	try
	{
		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		client.Print("客户端");
		cout << "任意键退出..." << endl;
		cin.get();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		cout << "任意键退出..." << endl;
		cin.get();
	}
	return 0;
}