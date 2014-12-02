/*
访问控制使用用户自定义回调函数，用于决定客户端是否能访问实现类
每一个客户端尝试调用方法是有服务器回调，如果被允许，则可以保存验证状态以备后续调用
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

class LoginInfo
{
public:

	void serialize(SF::Archive &ar)
	{

	}
};

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Login, const LoginInfo &)
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorldAuthenticationState
{
public:
	HelloWorldAuthenticationState()
		: mAuthenticated(false)
	{

	}

	bool mAuthenticated;
	RCF::tstring mClientUsername;
	LoginInfo mLoginInfo;
};


class HelloWorld
{
public:
	void Login(const LoginInfo &loginInfo)
	{
		cout << "Login" << endl;
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		// 如果不存在则创建
		HelloWorldAuthenticationState &authState = session.getSessionObject<HelloWorldAuthenticationState>(true);
		if (!authState.mAuthenticated)
		{
			RCF::TransportProtocol tp = session.getTransportProtocol();
			if (tp == RCF::Tp_Ntlm || tp == RCF::Tp_Kerberos)
			{
				authState.mAuthenticated = true;
				authState.mClientUsername = session.getClientUsername();
				authState.mLoginInfo = loginInfo;
			}
		}
	}

	void Print(const string &s)
	{
		cout << "Print服务器: " << s << endl;
		// 获取当前连接的会话
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		// 发送和收到的字节数
		boost::uint64_t totalBytesReceived = session.getTotalBytesReceived();
		cout << totalBytesReceived << endl;
		boost::uint64_t totalBytesSent = session.getTotalBytesSent();
		cout << totalBytesSent << endl;
	}
};

bool onHelloWorldAccess(int dispatchId)
{
	cout << "DispatchId: " << dispatchId << endl;
	if (dispatchId == 0)
	{
		return true;
	}
	else
	{
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		HelloWorldAuthenticationState &authState = session.getSessionObject<HelloWorldAuthenticationState>(true);
		return authState.mAuthenticated;
	}
}

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
		// 获取绑定对象
		RCF::ServerBindingPtr bindPtr = server.bind<I_HelloWorld>(helloWorld);
		bindPtr->setAccessControl(boost::bind(&onHelloWorldAccess, _1));
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		client.getClientStub().setTransportProtocol(RCF::Tp_Ntlm);
		client.Login(LoginInfo());
		client.Print("压缩");

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}