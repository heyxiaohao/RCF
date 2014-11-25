
/*
	即：捕获调用句柄，统一处理
	当请求到达，使用线程池的一个线程获取句柄，另一个线程处理异步调用
	RCF::RemoteCallContext<> 捕获远程调用服务端上下文，并队列后做后续处理
	RemoteCallContext::parameters() 访问每一个调用的参数
	RemoteCallContext::commit() 发送调用回应
*/

#include <iostream>
#include <deque>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/RemoteCallContext.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
RCF_METHOD_R1(int, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	typedef RCF::RemoteCallContext<int, const std::string &> PrintCall;

	int Print(const string &s)
	{
		
	}
private:
	RCF::Mutex mPrintCallsMutex;
	deque<PrintCall> mPrintCalls;
	RCF::ThreadPtr mPrintThreadPtr;
	volatile bool mStopFlag;
};

int main()
{
	RCF::init();
	HelloWorld helloWorld;
	RCF::RcfServer server(RCF::TcpEndpoint(50001));
	server.bind<I_HelloWorld>(helloWorld);
	server.start();

	RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
	int ret = client.Print("完好");
	cout << ret << endl;

	return 0;
}