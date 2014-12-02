/*
上传： RCF::FileUpload 需要作为远程方法的参数来实现
服务端默认是不开启传输功能，需要明确激活
如果指定为目录，则目录下所有文件都将传输
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

void onFileTransferProgress(const RCF::FileTransferProgress &progress)
{
	// 使用boost库的进度显示

//	cout << "文件大小： " << progress.mBytesTotalToTransfer << endl;
//	cout << "已经上传的大小: " << progress.mBytesTransferredSoFar << endl;
	//cout << "服务器带宽限制: " << progress.mServerLimitBps << endl;
}

// 服务器端监视上传信息，可抛出异常中断传输
void onServerFileUpload(RCF::RcfSession &session, const RCF::FileUploadInfo &uploadInfo)
{
	// 哪个文件在传输
	const RCF::FileManifest &fileManifest = uploadInfo.mManifest;
	// 上传进度
	boost::uint32_t currentFile = uploadInfo.mCurrentFile; // 当前第一个文件
	boost::uint64_t currentFilePos = uploadInfo.mCurrentPos; // 已上传的大小
	cout << currentFile << ":" << currentFilePos << endl;
	// 抛出异常，中断传输
}

int main()
{
	RCF::init();
	try
	{
		RCF::RcfServer server((RCF::TcpEndpoint(50001)));
		HelloWorld helloWorld;
		server.bind<I_HelloWorld>(helloWorld);
		// 默认上传的文件保存至与应用同级目录，可指定
		server.setFileUploadDirectory("C://RCF-Uploads");
		server.setOnFileUploadProgress(&onServerFileUpload);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		RCF::FileUpload fileUpload("E:/works/RCF/RCF.sdf");
		// 上传文件，上传期间如果连接断开，则可以再次调用以继续传输，可开启自动重连
		// 可设置上传进度回调显示 
		client.getClientStub().setFileProgressCallback(&onFileTransferProgress);
		client.getClientStub().setAutoReconnect(true);
		// 可对文件传输进行加密
		client.getClientStub().setTransportProtocol(RCF::Tp_Ntlm);
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