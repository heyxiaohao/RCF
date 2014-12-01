/*
配置允许或限制的客户端IP连接
IPv6的限制规则怎么设置？？？
*/

#include <iostream>
#include <vector>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const string &s)
	{
		cout << "服务器: " << s << endl;
	}
};

int main()
{
	try{
		RCF::init();


		RCF::RcfServer server(RCF::TcpEndpoint(50001));
		RCF::IpServerTransport &ipTransport = server.getIpServerTransport();
		// 设置允许的客户端IP
		vector<RCF::IpRule> ipAllowRules;
		// 匹配
		ipAllowRules.push_back(RCF::IpRule(RCF::IpAddress("11.22.33.0"), 24)); // 24表示24位匹配
		ipAllowRules.push_back(RCF::IpRule(RCF::IpAddress("127.0.0.0"), 24));
		ipAllowRules.push_back(RCF::IpRule(RCF::IpAddress("192.168.0.0"), 16));
		ipTransport.setAllowIps(ipAllowRules);

		// 设置拒绝的客户端IP
		vector<RCF::IpRule> ipDenyRules;
		ipDenyRules.push_back(RCF::IpRule(RCF::IpAddress("11.0.0.0"), 8));
		ipDenyRules.push_back(RCF::IpRule(RCF::IpAddress("12.22.0.0"), 16));
		ipTransport.setDenyIps(ipDenyRules);

		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		client.Print("我是客户端1");
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}