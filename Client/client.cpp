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
		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));

		// 客户端代理
		RCF::ClientStub &clientStub = client.getClientStub();
		// 最重要的设置 连接超时时间和远程调用超时时间
		clientStub.setConnectTimeoutMs(2000);
		clientStub.setRemoteCallTimeoutMs(2000);
		
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