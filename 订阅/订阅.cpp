/*
¶©ÔÄ
*/

#include <iostream>
#include <deque>
using namespace std;

#include <RCF/RCF.hpp>
#include <RCF/RemoteCallContext.hpp>

RCF_BEGIN(I_HelloWorld, "I_HelloWorld")
RCF_METHOD_R1(int, Print, const string &)
RCF_END(I_HelloWorld)

class HelloWorld
{
public:
	int Print(const string &s)
	{
		cout << s << endl;
		return s.size();
	}
};

int main()
{
	RCF::init();
	try
	{
		RCF::RcfServer subServer((RCF::TcpEndpoint()));
		subServer.start();
		HelloWorld helloWorld;
		RCF::SubscriptionPtr subPtr = subServer.createSubscription<I_HelloWorld>(helloWorld, RCF::TcpEndpoint(50001));

		cout << "ÈÎÒâ¼üÍË³ö..." << endl;
		cin.get();

		subPtr->close();

	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	return 0;
}