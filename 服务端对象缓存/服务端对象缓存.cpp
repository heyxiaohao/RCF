/*
序列化和反序列化是性能瓶颈，使用服务端对象缓存可提升，第一次调用会分配缓存，后续调用不会再分配
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
		return s;
	}
};

int main()
{
	RCF::init();
	try
	{
		RCF::RcfServer server((RCF::TcpEndpoint(50001)));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		// 创建缓冲
		RCF::ObjectPool &cache = RCF::getObjectPool();
		// 为string激活缓冲，语义不要超过10个string对象，在放入缓冲前调用string.clear
		cache.enableCaching<string>(10, boost::bind(&string::clear, _1));

		string s1 = "asdfghjkl;";
		string s2;

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		// 第一次调用会分配缓冲区内存
		s2 = client.Print(s1);
		// 随后同一个连接调用，不会分配缓冲
		for (size_t i = 0; i < 100; ++i)
		{
			s2 = client.Print(s1);
		}

		// 删除缓存
		cache.disableCaching<string>();

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