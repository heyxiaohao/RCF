/*
上传： RCF::FileUpload 需要作为远程方法的参数来实现
服务端默认是不开启传输功能，需要明确激活
*/

#include <iostream>
using namespace std;

//#define RCF_USE_BOOST_FILESYSTEM // 开启下载功能需要在属性 预处理器中设置  
// 需要首先编译filesystem库 boost filesystem库支持
#include <RCF/RCF.hpp>
#include <RCF/FileUpload.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V2(void, PrintAndUpload, const string &, RCF::FileUpload)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void PrintAndUpload(const string &s, RCF::FileUpload fileUpload)
	{
		cout << "服务器: " << s << endl;
		string pathToUpload = fileUpload.getLocalPath();
		RCF::FileManifest &uploadManifest = fileUpload.getManifest();
		cout << "上传路径:" << pathToUpload << endl;
		cout << "文件个数:" << uploadManifest.mFiles.size() << endl;
		cout << "第一个文件大小: " << uploadManifest.mFiles[0].mFileSize << endl;
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
		RCF::FileUpload fileUpload("E:/works/RCF/RCF.sdf");
		// 上传文件
		client.PrintAndUpload("wang", fileUpload);

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