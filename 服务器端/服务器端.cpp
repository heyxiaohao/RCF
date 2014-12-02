/*
服务器端  RcfServer::setOnCallbackConnectionCreated() 用于控制客户端建立的回调连接
*/

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

RCF_BEGIN(I_IdentifyClient, "I_IdentifyClient")
	RCF_METHOD_V1(void, SetClientName, const string &)
RCF_END(I_IdentifyClient)

// 设置客户端名字，用于识别
class IdentifyClient
{
public:
	void SetClientName(const string &clientName)
	{
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		string &sessionClientName = session.createSessionObject<string>();
		sessionClientName = clientName;
	}
};

typedef RcfClient<I_HelloWorld> helloWorldClient;
typedef boost::shared_ptr<helloWorldClient > HelloWorldPtr;
RCF::Mutex gCallbackClientMutex;
//vector<HelloWorldPtr> gCallbackClients;
map<string, HelloWorldPtr> gCallbackClients;

// 回调连接创建完毕调用
void onCallbackConnectionCreated(RCF::RcfSessionPtr sessionPtr, RCF::ClientTransportAutoPtr transportPtr)
{
	string &clientName = sessionPtr->getSessionObject<string>();
	cout << "clientName" << clientName << endl;

	// 保存
	HelloWorldPtr helloWorldPtr(new RcfClient<I_HelloWorld>(transportPtr));
	RCF::Lock lock(gCallbackClientMutex);
	//gCallbackClients.push_back(helloWorldPtr);
	gCallbackClients[clientName] = helloWorldPtr;
}

int main()
{
	RCF::init();
	try
	{
		RCF::RcfServer server((RCF::TcpEndpoint(50004)));
		server.setOnCallbackConnectionCreated(boost::bind(&onCallbackConnectionCreated, _1, _2));
		IdentifyClient identifyClient;
		server.bind<I_IdentifyClient>(identifyClient);
		server.start();

		// 等待连接，可单独创建等待线程
		while (gCallbackClients.empty())
		{
			RCF::sleepMs(1000);
		}

		// 开始回调，此处可单独创建处理线程
		//vector<HelloWorldPtr> clients;
		map<string, HelloWorldPtr> clients;
		{
			RCF::Lock lock(gCallbackClientMutex);
			clients.swap(gCallbackClients);
		}
		// 调用客户端函数
		for (size_t i = 0; i < clients.size(); ++i)
		{
			HelloWorldPtr client = clients["wangh"];
			//HelloWorldPtr client = clients[i];
			client->Print("hello网");
		}

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