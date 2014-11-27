/*
可以定义相当复杂的容器，而不需要编写序列化
*/


#include <iostream>
using namespace std;

#include <boost/tuple/tuple.hpp>

#include <RCF/RCF.hpp>

// 包含序列化框架
#include <SF/map.hpp>
#include <SF/list.hpp>
#include <SF/tuple.hpp>
#include <SF/vector.hpp>

typedef map<string, list<pair<int, string> > >	MyMap;
typedef string Name;
typedef string Street;
typedef unsigned int Zip;
typedef string Suburb;
typedef string State;
typedef string Country;
typedef boost::tuple<Name, Street, Zip, Suburb, State, Country> Address;
typedef vector<Address> Addresses;

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
	RCF_METHOD_R1(MyMap, Echo, const MyMap &)
	RCF_METHOD_R1(Addresses, Echo, const Addresses &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	MyMap Echo(const MyMap &myMap)
	{
		cout << "服务器：" << myMap.size() << endl;
		return myMap;
	}
	Addresses Echo(const Addresses &addresses)
	{
		cout << "服务器：" << addresses.size() << endl;
		return addresses;
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

		MyMap myMap;
		myMap["1-3"].push_back(make_pair(1, "one"));
		myMap["1-3"].push_back(make_pair(2, "two"));
		myMap["1-3"].push_back(make_pair(3, "three"));
		myMap["4-6"].push_back(make_pair(4, "four"));
		myMap["4-6"].push_back(make_pair(5, "five"));
		myMap["4-6"].push_back(make_pair(6, "six"));
		MyMap m = client.Echo(myMap);
		
		Addresses addresses;
		addresses.push_back(Address("", "", 123, "", "", ""));
		addresses.push_back(Address("", "", 456, "", "", ""));
		addresses.push_back(Address("", "", 789, "", "", ""));
		Addresses a = client.Echo(addresses);

		close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
		close();
	}


	return 0;
}