/*
下载： RCF::FileDownload 需要作为远程方法的参数来实现
服务端默认是不开启传输功能，需要明确激活
如果指定为目录，则目录下所有文件都将传输
*/

#include <iostream>
using namespace std;

//#define RCF_USE_BOOST_FILESYSTEM // 开启下载功能需要在属性 预处理器中设置  
// 需要首先编译filesystem库 boost filesystem库支持
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
		fileDownload = RCF::FileDownload("E:/works/RCF/RCF.sdf");
	}
};

void onFileTransferProgress(const RCF::FileTransferProgress &progress)
{
	// 使用boost库的进度显示

	//cout << "文件大小： " << progress.mBytesTotalToTransfer << endl;
	//cout << "已经下载的大小: " << progress.mBytesTransferredSoFar << endl;
	//cout << "服务器带宽限制: " << progress.mServerLimitBps << endl;
}

// 服务器端监视下载信息，可抛出异常中断传输
void onServerFileDown(RCF::RcfSession &session, const RCF::FileDownloadInfo &downloadInfo)
{
	// 哪个文件在传输
	const RCF::FileManifest &fileManifest = downloadInfo.mManifest;
	// 传输进度
	boost::uint32_t currentFile = downloadInfo.mCurrentFile; // 当前第一个文件
	boost::uint64_t currentFilePos = downloadInfo.mCurrentPos; // 已上传的大小
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
		server.setOnFileDownloadProgress(&onServerFileDown);
		server.start();

		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));
		RCF::FileDownload fileDownload(".");
		// 下载文件，如果下载过程中断开连接，则可以再次调用以继续下载，
		client.getClientStub().setAutoReconnect(true);
		// 可设置下载进度回调显示 
		client.getClientStub().setFileProgressCallback(&onFileTransferProgress);
		// 可对文件传输进行加密
		client.getClientStub().setTransportProtocol(RCF::Tp_Ntlm);
		client.PrintAndDownload("wang", fileDownload);
		// 获取下载文件存放路径
		string pathToDownload = fileDownload.getLocalPath();
		// 获取文件
		RCF::FileManifest &downloadManifest = fileDownload.getManifest();
		cout << "下载文件存放路径：" << pathToDownload << endl;
		cout << "文件个数：" << downloadManifest.mFiles.size() << endl;
		cout << "第一个文件大小：" << downloadManifest.mFiles[0].mFileSize << endl;

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