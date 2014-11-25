/*
需要添加 RCF/include   boost/include
工程需要引入 RCF.cpp
如果是win32  需要在项目属性定义 WIN32_LEAN_AND_MEAN
*/


#include <iostream>
#include <vector>
using namespace std;

#include <RCF/RCF.hpp>
#include <SF/vector.hpp> // vector的序列化实现类
/************************************************************************/
// 接口定义
/************************************************************************/
RCF_BEGIN(I_HelloWorld, "I_HelloWorld") // (编译时接口标识, 运行时接口标识)
	RCF_METHOD_V1(void, Print, const string &) // 定义一个参数，返回值为void的接口
	RCF_METHOD_R1(int, Print, const vector<string> &)// 定义一个参数，返回值为string的接口
	RCF_METHOD_V2(void, Print, const vector<string> &, int &)
RCF_END(I_HelloWorld)
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// 会话信息 
/************************************************************************/
class HelloWorldSession
{
public:
	HelloWorldSession(){ m_CallCount = 0; cout << "Created HelloWorldSession object." << endl; }
	~HelloWorldSession() { cout << "Destroyed HelloWorldSession object." << endl; }
	size_t m_CallCount;
};
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// 接口实现 
/************************************************************************/
class HelloWorld
{
public:
	void Print(const string &s)
	{
		RCF::RcfSession &session = RCF::getCurrentRcfSession();
		HelloWorldSession &hwSession = session.getSessionObject<HelloWorldSession>(true);
		++hwSession.m_CallCount;

		//throw runtime_error("Print() can't be called");
		cout << "服务器: " << s << endl;
		cout << "总共调用次数: " << hwSession.m_CallCount << endl;
	}
	int Print(const vector<string> &v)
	{
		int howManyChars = 0;
		for (size_t i = 0; i < v.size(); ++i)
		{
			cout << "服务器: " << v[i] << endl;
			howManyChars += v[i].size();
		}
		return howManyChars;
	}
	void Print(const vector<string> &v, int &howManyChars)
	{
		howManyChars = 0;
		for (size_t i = 0; i < v.size(); ++i)
		{
			cout << "服务器: " << v[i] << endl;
			howManyChars += v[i].size();
		}
	}
};
//////////////////////////////////////////////////////////////////////////

int main()
{
	RCF::RcfInitDeinit rcfInit;
	HelloWorld helloWorld;
	RCF::RcfServer server(RCF::TcpEndpoint(50001));
	server.bind<I_HelloWorld>(helloWorld);  // 服务器绑定到接口实现对象
	server.start();

	cout << "任意键退出..." << endl;
	cin.get();

	return 0;
}