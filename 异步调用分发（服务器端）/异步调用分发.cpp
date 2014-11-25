
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
		// 上锁，超过作用域自定释放
		RCF::Lock lock(mPrintCallsMutex);
		// 将调用请求放入队列
		mPrintCalls.push_back(PrintCall(RCF::getCurrentRcfSession()));
		return 0;
	}

	HelloWorld()
	{
		mStopFlag = false;
		// 创建处理线程
		mPrintThreadPtr.reset(new RCF::Thread(boost::bind(&HelloWorld::processPrintCalls, this)));
	}
	~HelloWorld()
	{
		// 停止异步处理
		mStopFlag = true;
		// 等待线程结束
		mPrintThreadPtr->join();
	}

private:
	RCF::Mutex mPrintCallsMutex;
	deque<PrintCall> mPrintCalls;
	RCF::ThreadPtr mPrintThreadPtr;
	volatile bool mStopFlag;
	
	void processPrintCalls()
	{
		// 每秒处理所有加入的请求
		while (!mStopFlag)
		{
			RCF::sleepMs(1000);
			// 获取所有请求
			deque<PrintCall> printCalls;
			{ // 方便锁自动释放
				RCF::Lock lock(mPrintCallsMutex);
				printCalls.swap(mPrintCalls);
			}
			// 处理
			for (size_t i = 0; i < printCalls.size(); ++i)
			{
				PrintCall &printCall = printCalls[i];
				// Print函数接受一个参数
				const string &stringToPrint = printCall.parameters().a1.get();
				// 打印传入的参数
				cout << "服务器: " << stringToPrint << endl;
				// 设置返回值
				printCall.parameters().r.set(stringToPrint.size());
				// 提交函数返回值
				printCall.commit();
			}
		}
	}
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

	cout << "任意键退出..." << endl;
	cin.get();
 
	return 0;
}