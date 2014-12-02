/*
订阅
RcfServer::createSubscription<>();
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_R1(int, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	int Print(const string &s)
	{
		cout << s << endl;
		return s.size();
	}
};

// 订阅方断开连接回调函数
void onSubscriptionDisconnected(RCF::RcfSession &session)
{
	cout << "断开连接" << endl;

	cout << "任意键退出..." << endl;
	cin.get();
}

int main()
{
	RCF::init();
	try
	{
		RCF::RcfServer subServer((RCF::TcpEndpoint())); // 此处不能填端口号
		subServer.start();
		HelloWorld helloWorld;
		RCF::SubscriptionParms subParms;
		subParms.setPublisherEndpoint(RCF::TcpEndpoint(50001));
		subParms.setOnSubscriptionDisconnect(&onSubscriptionDisconnected);
		// 默认主题名  I_HelloWorld
		RCF::SubscriptionPtr subPtr = subServer.createSubscription<I_HelloWorld>(
			helloWorld, // 指定那个实现类接受调用，在订阅仍在连接时，此对象不能被销毁
			subParms);
		// 手动指定
		subParms.setTopicName("HelloWorld_Topic_1");

		cout << "任意键退出..." << endl;
		cin.get();
		// 关闭
		subPtr->close();
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