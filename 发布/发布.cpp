/*
发布
*/

#include <iostream>
using namespace std;

#include <RCF/RCF.hpp>

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
		RCF::RcfServer pubServer(RCF::TcpEndpoint(50001));
		pubServer.start();
		typedef boost::shared_ptr<RCF::Publisher<I_HelloWorld> > HelloWorldPubPtr;
		HelloWorldPubPtr pubPtr = pubServer.createPublisher<I_HelloWorld>();

		while (true)
		{
			RCF::sleepMs(1000);
			pubPtr->publish().Print("HelloWorld");
		}

		pubPtr->close();
	}
	catch (RCF::Exception &e)
	{
		cout << e.getErrorString() << endl;
	}

	cout << "任意键退出..." << endl;
	cin.get();



	return 0;
}