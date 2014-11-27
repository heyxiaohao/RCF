/*
RCF的 SF序列化框架可以单独使用，如序列化到文件
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include <SF/OBinaryStream.hpp>
#include <SF/IBinaryStream.hpp>
#include <SF/vector.hpp>
#include <SF/string.hpp>

class MyType
{
public:
	string name;
	vector<int> num;
	void serialize(SF::Archive &ar)
	{
		ar &name &num;
	}
};

int main()
{
	string filename = "data.txt";

	MyType myType;
	myType.name = "王浩";
	myType.num.push_back(112);
	myType.num.push_back(101);

	ofstream fout(filename.c_str(), ios::binary);
	SF::OBinaryStream os(fout);
	os << myType;
	fout.close();

	ifstream fin(filename.c_str(), ios::binary);
	SF::IBinaryStream is(fin);
	MyType m;
	is >> m;
	cout << m.name << endl;
	cout << m.num[0] << endl;

	return 0;
}