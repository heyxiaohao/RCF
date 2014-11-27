/*
如果使用自定义的类型如：
class Point3D
{
public:
	double mX;
	double mY;
	double mZ;
};
RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_R1(void, Print, const Point3D &)
RCF_END(I_HelloWorld)
编译时报错：
d:\rcf-2.0.1.101\include\sf\serializer.hpp(283): error C2039: “serialize”: 不是“Point3D”的成员
表示自定义的类型需要提供序列化函数  serialize()，或者作为成员函数 用于序列化和反序列化
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

// 方式1 成员函数
class Point3D
{
public:
	double mX;
	double mY;
	double mZ;

// 	void serialize(SF::Archive &ar)
// 	{
// 		ar &mX &mY &mZ; // 指定那些成员需要被序列化
// 	}
};

// 方式2 外部函数（需要与Point3D具有相同的命名空间 或 都无
//void serialize(SF::Archive &ar, Point3D &point3D)
//{
//	ar &point3D.mX &point3D.mY &point3D.mZ;
//}

// 方式3 在 SF 命名空间中
namespace SF
{
	// 序列化和反序列化都在此函数进行
	void serialize(SF::Archive &ar, Point3D &point3D)
	{
		ar &point3D.mX &point3D.mY &point3D.mZ;
		if (ar.isWrite()) // 表示在序列化
		{
			cout << "序列化" << endl;
		}
		else if (ar.isRead()) // 表示在反序列化
		{
			cout << "反序列化" << endl;
		}
	}
}

// 方式4 使用ProtocolBuffers方式

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const Point3D &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const Point3D &point3D)
	{
		cout << "Print服务器: " << point3D.mX << endl;
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
		
		Point3D point3D = { 1, 2, 3 };
		client.Print(point3D);

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}