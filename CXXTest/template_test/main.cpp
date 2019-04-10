#include <vector>
#include <iostream>
#include <memory>
#include "make_unique_define.h"

using namespace std;

template<class C, class Oper>
void for_all(C& c, Oper op)
{
	for (auto& x : c)
	{
		op(*x);
	}
}

class Test
{
public:
	Test(int val) { m_val = val; }
	~Test(){}
public:
	void Print() { std::cout << m_val << " "; }
private:
	int m_val;
};



int main()
{
	std::vector<unique_ptr<Test> > v;
	
	unique_ptr<Test> t1(new Test(1));
	unique_ptr<Test> t2(new Test(3));
	unique_ptr<Test> t3(new Test(5));
	Test t(123);
	unique_ptr<Test> t4(&t);
	unique_ptr<Test> t5 = make_unique<Test>(-34.50);

	v.push_back(std::move(t1));
	v.push_back(std::move(t2));
	v.push_back(std::move(t3));
	v.push_back(std::move(t4));
	v.push_back(std::move(t5));

	for_all(v, [](Test& t) { t.Print(); });
	t.Print();

	return 0;
}

