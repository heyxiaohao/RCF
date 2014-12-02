/*
SF 序列化指针指向的对象的系数与 序列化指针相同，此时则指针指向的是不同的对象
如果希望指针指向同一个对象，则需要使用指针跟踪功能
此项功能有较大的性能损失，因为需要跟踪所有指针
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include <SF/shared_ptr_boost.hpp>
#include <SF/vector.hpp>

typedef vector<boost::shared_ptr<int> > TwoPointers;
RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_R1(TwoPointers, Print, const TwoPointers &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	TwoPointers Print(TwoPointers &x)
	{
		// 设置指针跟踪
		RCF::getCurrentRcfSession().setEnableSfPointerTracking(true);
		cout << "调用期间： " << x[0] << ":" << x[1] << endl;
		return x;
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
		// 设置指针跟踪，服务端在收到后仍然需要设置
		client.getClientStub().setEnableSfPointerTracking(true);
		boost::shared_ptr<int> spn1(new int(1));
		boost::shared_ptr<int> spn2(spn1);
		cout << "创建初期： " << spn1 << ":" << spn2 << endl;
		TwoPointers tw;
		tw.push_back(spn1);
		tw.push_back(spn2);
		cout << "放入容器： " << tw[0] << ":" << tw[1] << endl;
		TwoPointers ret = client.Print(tw);
		cout << "调用过后： " << ret[0] << ":" << ret[1] << endl;

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}