/*
实现类(servant object)用于实现 RCF 定义的接口，每一个实现类都有一个绑定名字指示它
默认的绑定名字是运行时标识（也标识I_HelloWorld接口）
也可以在调用bind函数时明确的指定绑定名字，此时客户端就必须使用此自定义的绑定名字进行连接
*/


#include <iostream>

using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld") // (编译时接口标识, 运行时接口标识)
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const string &s)
	{
		cout << "Print服务器: " << s << endl;
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
		server.bind<I_HelloWorld>(helloWorld, "CustomBindingName");
		server.start();

		close();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001), "CustomBindingName");
		RCF::ClientStub &clientStub = client.getClientStub();
		
		client.Print("client");

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}