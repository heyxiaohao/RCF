/*
RcfSession 可以获取当前客户端连接的信息
*/

/*
底层与服务器的连接或取消连接一般是自动的，也可以手动
client.getClientStub().connect();
client.getClientStub().disconnect();
可以将一个客户端的底层协议移动到另一个客户端
ClientStub::releaseTransport()    ClientStub::setTransport();
应用：在不同的客户端实例使用同一个底层传输协议
*/


#include <iostream>

using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class MySessionObj
{
public:
	MySessionObj(){ count++; }
	static int count;
};
int MySessionObj::count = 0;

class HelloWorld
{
public:
	void Print(const string &s)
	{
		cout << "Print服务器: " << s << endl;
		// 获取当前连接的会话
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		// 传输协议
		RCF::TransportType transportType = session.getTransportType();
		// 客户端地址:端口
		const RCF::RemoteAddress &clientAddr = session.getClientAddress();
		cout << clientAddr.string() << endl;
		// 请求
		RCF::RemoteCallRequest request = session.getRemoteCallRequest();
		// 自定义的请求附加数据
		string customRequestData = session.getRequestUserData();
		// 创建，取得，删除会话对象
		session.createSessionObject<MySessionObj>();
		MySessionObj *pObj = session.querySessionObject<MySessionObj>();
		MySessionObj &obj = session.getSessionObject<MySessionObj>(); // 如果不存在则会抛出异常，可指定构造函数 为true
		cout << obj.count << endl;
		session.deleteSessionObject<MySessionObj>();
		// 客户端的连接成功的时间点（ms)
		time_t connectedAt = session.getConnectedAtTime();
		cout << connectedAt << endl;
		// 客户端连接的时长(S)，从客户端调用接口开始计算
		//RCF::sleepMs(2500);
		size_t connectedDurationS = session.getConnectionDuration();
		cout << connectedDurationS << endl;
		// 客户端的调用次数
		size_t callCount = session.getRemoteCallCount();
		cout << callCount << endl;
		// 发送和收到的字节数
		boost::uint64_t totalBytesReceived = session.getTotalBytesReceived();
		cout << totalBytesReceived << endl;
		boost::uint64_t totalBytesSent = session.getTotalBytesSent();
		cout << totalBytesSent << endl;
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