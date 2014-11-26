/*
简称 protobuf   google的开源数据交换格式   https://code.google.com/p/protobuf/
创建 .proto文件，protoc编译此文件（下载文件编译出protoc, 和库文件libprotobuf.lib 注意需要Debug和Release版本对应)
// person.proto
message Person
{
	required int32 id = 1;
	required string name = 2;
	optional string email = 3;
}

message PbEmpty
{
	optional string log = 1;
}

protoc Person.proto --cpp_out=.   (生成.cc 和 .h文件）

需要在属性 预处理器加入 RCF_USE_PROTOBUF
vs2013如出现 error C4996: 'std::_Copy_impl' 错误  需要在C/C++ 命令行 - 其他命令添加 -D_SCL_SECURE_NO_WARNINGS
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>
#include "person.pb.h"
#ifdef _DEBUG
#pragma comment(lib, "libprotobufD.lib")
#else
#pragma comment(lib, "libprotobuf.lib")
#endif

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_V1(void, Print, const Person &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	void Print(const Person &person)
	{
		cout << "服务器: " << endl;
		cout << person.name() << endl;
		cout << person.email() << endl;
		cout << person.id() << endl;
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
		
		Person person;
		person.set_name("abd"); // 如果是中文，需要转换为UTF-8
		person.set_id(110);
		person.set_email("hey-xiaohao@163.com");
		client.Print(person);

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

