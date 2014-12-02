

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
		cout << "服务器: " << s << endl;
		RCF::sleepMs(10000);
		//throw runtime_error("异步调用错误");
		return s.size();
	}
};

// 代替在主线程中中断等待异步调用结束
typedef boost::shared_ptr<RcfClient<I_HelloWorld> > HelloWorldPtr;
void onPrintCompleted(RCF::Future<int> fRet, HelloWorldPtr clientPtr)
{
	std::auto_ptr<RCF::Exception> ePtr = clientPtr->getClientStub().getAsyncException();
	if (ePtr.get())
	{
		// 有错误
		cout << ePtr->getErrorString() << endl;
	}
	else
	{
		cout << __FUNCTION__ << *fRet << endl;
	}
}

int main()
{
	RCF::init();

	RCF::RcfServer server(RCF::TcpEndpoint(50001));
	HelloWorld helloWorld;
	server.bind<I_HelloWorld>(helloWorld);
	server.start();

	//RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
	RCF::Future<int> fRet; // 异步调用返回值
	// 启动异步调用
//	fRet = client.Print("往后");

	// 回调模式异步调用
	HelloWorldPtr clientPtr(new RcfClient<I_HelloWorld>(RCF::TcpEndpoint(50001)));
	//fRet = clientPtr->Print(RCF::AsyncTwoway(boost::bind(&onPrintCompleted, fRet, clientPtr)), "往后");
	fRet = clientPtr->Print("hehe");
	// 等待调用完成
	fRet.wait();
// 	while (!fRet.ready())
// 	{
// 		RCF::sleepMs(10);
// 		cout << "等..." << endl;
// 	}
	// 检查错误
// 	std::auto_ptr<RCF::Exception> ePtr = client.getClientStub().getAsyncException();
// 	if (ePtr.get())
// 	{
// 		// 有错误
// 		cout << ePtr->getErrorString() << endl;
// 	}
// 	else
// 	{
// 		cout << *fRet << endl;
// 	}


	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}