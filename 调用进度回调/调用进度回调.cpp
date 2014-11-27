/*
报告Twoway调用的进度的回调，应用：展示进度条，重绘窗口，或取消调用
如果调用超时，则会触发回调
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const string &)
RCF_END(I_HelloWorld)

void onRemoteCallProgress()
{
	cout << "进度回调" << endl;
}

int main()
{
	RCF::init();
	cout << "调用服务器端的 Print 函数" << endl;
	try {
		RcfClient<I_HelloWorld> client(RCF::TcpEndpoint(50001));

		// 获取客户端代理
		RCF::ClientStub &clientStub = client.getClientStub();
		// 设置进度的回调
		clientStub.setRemoteCallProgressCallback(boost::bind(&onRemoteCallProgress), 500);// 每500ms回调一次

		client.Print("我是客户端1");
	}
	catch (RCF::Exception &e)
	{
		cout << "ErrorId: " << e.getErrorId() << endl; // 错误码
		cout << "Error: " << e.getErrorString() << endl; // 错误信息
	}

	cin.get();
	return 0;
}