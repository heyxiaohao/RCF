/*
内建Json-RPC服务器，Javascript可访问C++服务器功能，支持HTTP,HTTPS
RCF使用JSON Spirit库读取和写入JSON，因此需要下载和编译 JSON Spirit库，并在 属性 预处理器 定义宏 RCF_FEATURE_JSON
配置JSON-RPC 端点，I_ServerTransport::setRpcProtocol()
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/JsonRpc.hpp>
#include <json_spirit.h>
#ifdef _DEBUG
#pragma comment(lib, "json_spirit_libD.lib")
#else
#pragma comment(lib, "json_spirit_lib.lib")
#endif

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

	void JsonPrint(const RCF::JsonRpcRequest &request, RCF::JsonRpcResponse &response)
	{
		int charsPrinted = 0;
		const json_spirit::Array &params = request.getJsonParams();
		for (size_t i = 0; i < params.size(); ++i)
		{
			const string &s = params[i].get_str();
			cout << "服务器:" << s << endl;
			charsPrinted += s.size();
		}

		json_spirit::mObject &responseObj = response.getJsonResponse();
		responseObj["result"] = charsPrinted;
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
		server.bindJsonRpc(boost::bind(&HelloWorld::JsonPrint, &helloWorld, _1, _2), "JsonPrint");
		server.addEndpoint(RCF::HttpEndpoint(80)).setRpcProtocol(RCF::Rp_JsonRpc);
		server.start();

		// RCF 客户端可以调用Print 在端口50001
		// Json-RPC客户端可以调用JsonPrint通过HTTP在80端口

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