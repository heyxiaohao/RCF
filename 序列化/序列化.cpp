/*
远程调用需要被序列化为二进制数据用于网络传输，接收端需要反系列化为C++对象
RCF 在 SF 中提供序列化框架，使用此框架可以自动序列化C++的数据类型，以及自定义类型
基础类型由RCF自动序列化，其他类型需要引入相关的头文件
一般，如果C++的类型定义为 <xyz> 或 Boost的类型 <boost/xyz.hpp>，则相对应的序列化定义 <SF/xyz.hpp>
C++的enmu类型自动序列化为int   C++11的enum class 类型 需要定义宏

*/


#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

// 自动序列化为int
enum Suit
{
	Heart = 1,
	Diamond = 2,
	Club = 2
};

// C++11 enum class
enum class Colors : int8_t
{
	Red = 1,
	Green = 2,
	Blue = 3
};
SF_SERIALIZE_ENUM_CLASS(Colors, int8_t) // 

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
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
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));

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