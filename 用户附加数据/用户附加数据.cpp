/*
一般数据都是作为参数传递，但可额外附加数据到调用请求的用户数据域，服务器端设置在 应答 中设置数据域
应用：可作为自定义的验证
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
		cout << "服务器: " << s << endl;

		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		string customRequestData = session.getRequestUserData();
		cout << "请求用户数据: " << customRequestData << endl;

		string customResponseData = "f81d4fae-7dec-11d0-a765-00a0c91e6bf6";
		session.setResponseUserData(customResponseData);
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
		RCF::ClientStub &clientStub = client.getClientStub();
		clientStub.setRequestUserData("e6a9bb54-da25-102b-9a03-2db401e887ec");
		client.Print("用户数据");
		string responseData = clientStub.getResponseUserData();
		cout << "应答用户数据: " << responseData << endl;

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}
	

	return 0;
}