/*
下载： RCF::FileDownload 需要作为远程方法的参数来实现
服务端默认是不开启传输功能，需要明确激活
*/

#include <iostream>
using namespace std;

#define RCF_FEATURE_FILETRANSFER 1// 开启下载功能  需要boost filesystem库支持
#include <RCF/RCF.hpp>
#include <RCF/FileDownload.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V2(void, PrintAndDownload, const string &, RCF::FileDownload) 
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void PrintAndDownload(const string &s, RCF::FileDownload fileDownload)
	{
		cout << "服务器: " << s << endl;
		fileDownload = RCF::FileDownload("G:/RCF/Client/client.cpp");
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
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		RCF::FileDownload fileDownload(".");
		// 下载文件
		client.PrintAndDownload("wang", fileDownload);
		// 获取下载文件存放路径
		string pathToDownload = fileDownload.getLocalPath();
		// 获取文件
		RCF::FileManifest &downloadManifest = fileDownload.getManifest();
		cout << "下载文件存放路径：" << pathToDownload << endl;
		cout << "文件大小：" << downloadManifest.mFiles.size() << endl;

		cout << "任意键退出..." << endl;
		cin.get();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}
	return 0;
}