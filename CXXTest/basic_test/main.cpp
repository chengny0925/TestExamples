#include <iostream>

using namespace std;

class ClassA {
public:
	virtual ~ClassA() {};
	virtual void FunctionA() {};
};
class ClassB {
public:
	virtual void FunctionB() {};
};
class ClassC :public ClassA, public ClassB {
public:
};

void test_1()
{

	ClassC Object;
	ClassA* pA = &Object;
	ClassB* pB = &Object;
	ClassC* pC = &Object;
	int n = 235;
	int* pn = &n;

	int pa = (int)pA;
	int pb = (int)pB;
	int pc = (int)pC;

	if (pa == pc && pa < pb)
	{
		cout << "yes" << endl;
	}
}

//#pragma pack(push) 
//#pragma pack(4)
struct One {
	double d;
	char c;
	int i;
};
struct Two {
	char c;
	double d;
	int i;
};

struct node
{
	char a;
	int b;
	//double d;
};
struct pa
{
	char a;
	node b;
};

//#pragma pack(pop)

/// 8字节对齐，结构体大小为16、24
/// 4字节对齐，结构体大小为16、16
/// 默认情况下与8字节对齐相同

void test_2()
{
	int size_one = sizeof(One);
	int size_two = sizeof(Two);
	One one;
	Two two;

	cout << "size of struct One is " << size_one << endl;
	cout << "size of struct Two is " << size_two << endl;

	cout << "size of struct node is " << sizeof(node) << endl;
	cout << "size of struct pa is " << sizeof(pa) << endl;
}

void test_3()
{
	cout << "输入i下单，q退出" << endl;
	char input;

	while (input = getchar())
	{
		switch (input)
		{
		case 'i':
		{
			cout << "请输入 合约、买卖方向、价格、数量" << endl;
			char code[16];
			int direction;
			double price;
			int volume;
			cin >> code >> direction >> price >> volume;

			cout << "code:" << code << endl;
			cout << "direction:" << direction << endl;
			cout << "price:" << price << endl;
			cout << "volume:" << volume << endl;
			break;
		}
		//   case 'c':
		//{
		//	trader.ReqOrderCancel();
		//	break;
		//}
		case 'q':
			return ;
		}

		if (input != '\n')
		{
			continue;
		}
		cout << "输入i下单，q退出" << endl;
		cin.clear();
	}






}

void test_4()
{
	int a[5] = { 1, 2, 3, 4, 5 };
	int *ptr = (int *)(&a + 1);
	printf("%d,%d", *(a + 1), *(ptr - 1));
}

#include <iomanip>
void test_5()
{
	signed char a = 0xe0;
	unsigned int b = a;
	unsigned char c = a;
	printf("%x, %x, %x\n", a,b,c);

	cout << "hex a:" << hex << a << endl;
	cout << "hex b:" << hex << b << endl;
	cout << "hex c:" << hex << c << endl;

	int as = 34;
	cout << "hex as:" << hex << as << endl;
	cout << "dec as:" << dec << as << endl;
	cout << "oct as:" << oct << as << endl;
	cout << "base 8 as:" << setbase(8) << as << endl;
}

void Foo(char str[100])
{
	int s4 = sizeof(str);
	cout << "s4=" << s4 << endl;
}

void test_6()
{
	int* pint = 0;
	pint += 6;
	cout << pint << endl;

	int t = 3; 
	int x = 2, y = 2; 
	t = x++ || ++y;

	cout << "t=" << t << ",x=" << x << ",y=" << y << endl;

	char str[] = "http://www.xxxxx.com";
	char *p = str;
	int n = 10;
	int s1 = sizeof(str);
	int s2 = sizeof(p);
	int s3 = sizeof(n);
	cout << "s1=" << s1 << endl;
	cout << "s2=" << s2 << endl;
	cout << "s3=" << s3 << endl;
	Foo(str);
	int len = strlen(str);

	void *p2 = malloc(100);
	int s5 = sizeof(p2);
	cout << "s5=" << s5 << endl;

	const int i = 0;
	int *j = (int *)&i;
	*j = 1;
	printf("%d, %d\n", i, *j);
	if ((*j) == i)
	{
		cout << "yes" << endl;
	}
	else
	{
		cout << "no" << endl;
	}
}

class parent {
public:
	virtual void output();
};
void parent::output() {
	printf("parent!");
}
class son : public parent {
public:
	son(int a):m_a(a)
	{
	}
	virtual void output();

private:
	int& m_a;
};
void son::output() {
	printf("son!");
}

void test_7()
{
	son s(4);
	s.output();
	//memset(&s, 0, sizeof(s));
	parent& p = s;
	p.output();
}


void test_8()
{
	struct Node {
		int size;
		char data[0];
	};

	Node no;
	Node* prt1 = &no;
	int* ptr2 = &no.size;
	char* ptr3 = no.data;
	int s1 = sizeof(Node);
	cout << "sizeof(Node)=" << s1 << endl;

	char str[10];
	char *p = str; p++; *p = '0';
}

//判断大小端
void test_9()
{
	typedef union {
		int i;
		char c;
	}my_union;

	int size = sizeof(my_union);
	my_union u;
	u.i = 1;
	if (u.i == u.c)
	{
		cout << "little endian" << endl;
	}
	else
	{
		cout << "big endian" << endl;
	}

	int i = 0x12345678;
	char *c = (char*)&i;
	bool b = ((c[0] == 0x78) && (c[1] == 0x56) && (c[2] == 0x34) && (c[3] == 0x12));
	if (b)
	{
		cout << "little endian" << endl;
	}
	else
	{
		cout << "big endian" << endl;
	}

	{
		/*
		1. long long占8字节。
		2. 小端表示，低字节在低位，最低4字节为1，接下来四字节为高位部分的0，再接下来4字节为第二个数低位的2。
		3. printf的控制符相当于分配好待打印容器大小，这里"%d %d %d"就分配了12字节，分别装入三个8字节元素，只装入一半。
		*/
		long long a = 1, b = 2, c = 3;
		printf("%d %d %d %d %d\n", a, b, c);
		cout << a << " " << b  << " " << c << endl;
	}
}

void test_10()
{
	//当一个类A中没有声明任何成员变量与成员函数,这时sizeof(A)的值是?  1
	//带虚函数的类长度就增加了4，这个4其实就是个指针，指向虚函数表的指针
	class my
	{

	};
	class Two
	{
	public:
		Two() {};
		~Two() {};
		//void Foo() { printf("Foo\n"); }
		virtual void Foo() { printf("Foo\n"); }
	};

	int size = sizeof(my);
	cout << "sizeof(my)=" << size << endl;
	cout << "sizeof(Two)=" << sizeof(Two) << endl;
}

void fun(int *pl, int *p2, int *s) {
	s = (int*)calloc(1, sizeof(int));
	*s = *pl + *p2;
	free(s);
}
void test_11()
{
	int a[2] = { 1, 2 }, b[2] = { 40, 50 }, *q = a;
	fun(a, b, q);
	printf("%d\n", *q);
}

void test_12()
{
	//考察结构体对齐及虚继承和虚函数继承的区别。
	//	1. A的大小包括本身的虚函数指针及定义的变量。
	//	2. B的大小包括本身的虚函数指针和继承自A的变量b和c。
	//	3. 如果是虚继承，则B的大小会增加4字节，增加的内容为指向虚继承的指针。
	class A {
	public:
		int b;
		char c;
		virtual void print() {
			cout << "this is father’s fuction! " << endl;
		}
	};
	class B : A {
	public:
		virtual void print() {
			cout << "this is children’s fuction! " << endl;
		}
	};
	class C : public A {
	public:
		virtual void print_2() {
			//cout << "this is children’s fuction! " << endl;
		}
	};
	class D :virtual public A {
	public:
		virtual void print_2() {
			//cout << "this is children’s fuction! " << endl;
		}
	};
	class E :virtual public A {
	public:
		virtual void print() {
			cout << "this is children’s fuction! " << endl;
		}
	};

	cout << sizeof(A) << " " << sizeof(B) << " " << sizeof(C) << " " << sizeof(D) << " " << sizeof(E) << endl;
}

void test_13()
{
	char str[10];
	cin >> str; 
	//当从键盘输入"I love this game"时，str中的字符串是: I
	//cin遇空格，结束输入
	cout << str << endl;
}

void f(char** p) {
	*p += 2;
}
void test_14()
{
	char *a[] = { "123", "abc", "456" }, **p;
	p = a;
	printf("%s\r\n", *(p + 2));
	printf("%s\r\n", *p + 2);
	f(p);
	printf("%s\r\n", *p);
}

double add(int a, int b) { return a + b; }
int add(int a, int b, int c) { return a + b + c; }
int add(double a, double b) { return a + b; }
//返回值类型在C++中并不作为重载标记
//double add(double a, double b) { return a + b; }

std::string& test_str() {
	std::string str = "test";
	return str;
}

void test_15()
{
	std::string& str_ref = test_str();
	//引用返回的是局部变量本身，而不是复制一份再返回，所以结果难以预料
	//std::cout << str_ref.c_str() << std::endl;
}

class B {
public:
	B() {
		cout << "default constructor" << " " << endl;
	}
	~B() {
		cout << "destructed" << " " << endl;
	}
	B(int i) :data(i) {
		cout << "constructed by parameter" << data << " " << endl;
	}
private:
	int data;
};
B Play(B b) {
	return b;
}
void test_16()
{
	B temp = Play(5);
}

void test_17()
{
	int i = 10, j = 10, k = 3;
	k *= i + j;
	cout << "k = " << k << endl;

	struct st_task {
		uint16_t id;
		uint32_t value;
		uint64_t timestamp;
	};
	st_task task = {};
	uint64_t a = 0x00010001;
	memcpy(&task, &a, sizeof(uint64_t));
	printf("%11u, %11u, %11u\n", task.id, task.value, task.timestamp);
	cout << "sizeof(st_task) " << sizeof(st_task) << endl;
}

void test_18()
{
	char arr[] = { 4, 3, 9, 9, 2, 0, 1, 5 };
	char *str = arr;
	char *a[] = { "123", "abc", "456" }, **p;
	p = a;

	cout << "sizeof(arr) " << sizeof(arr) << endl;
	cout << "sizeof(str) " << sizeof(str) << endl;
	cout << "strlen(str) " << strlen(str) << endl;
	cout << "sizeof(a) " << sizeof(a) << endl;
	cout << "sizeof(p) " << sizeof(p) << endl;
}

void test_19()
{
	unsigned int a = 6;
	int b = -20;
	//强制类型转换: int与unsigned相加，int -> unsigned int。
	(a + b > 6) ? printf(">6") : printf("<=6");
}

void test_20()
{
	//考察运算符优先级。
	//	1. p先和那个运算符结合就是什么。
	//	2. char *p[16]：p是一个包含16个元素的Char型指针数组，[]优于*，p[]先结合，是数组。
	//	3. char(*p)[16]：p是一个指针，指向一个包含16个元素的char数组，由于()出现，先和*结合，是指针。
	char(*p)[16];
	cout << "sizeof(p):" << sizeof(p) << endl;
	cout << "sizeof(*p):" << sizeof(*p) << endl;

	char *t[16];
	cout << "sizeof(t):" << sizeof(t) << endl;
}

void test_21()
{
	/*考察虚继承的继承顺序。
		1. class B, class C为虚继承。
		2. 虚继承（class 派生类 : virtual 继承方式 基类名）：从不同的路径继承过来的同名数据成员在内存中就只有一个拷贝。
		3. 继承顺序：
		执行基类构造函数，多个基类的构造函数按照被继承的顺序构造。
		执行成员对象的构造函数，多个成员对象的构造函数按照声明的顺序构造。
		执行派生类自己的构造函数。
		4. 要执行D构造函数必须先执行参数列表，欲构造B, C必须先构造A，其B, C虚继承A，所以只要执行一次构造函数。*/
	class A {
	public:
		A(char *s) {
			cout << s << endl;
		}
		~A() {}
	};
	class B :virtual public A {
	public:
		B(char *s1, char*s2) :A(s1) {
			cout << s2 << endl;
		}
	};
	class C :virtual public A {
	public:
		C(char *s1, char*s2) :A(s1) {
			cout << s2 << endl;
		}
	};
	class D :public B, public C {
	public:
		D(char *s1, char *s2, char *s3, char *s4) :B(s1, s2), C(s1, s3), A(s1) {
			cout << s4 << endl;
		}
	};

	D *p = new D("class A", "class B", "class C", "class D");
	delete p;
}

void test_22()
{
	char a[] = "xyz", b[] = { 'x', 'y', 'z' };
	if (strlen(a) > strlen(b))
		printf("a > b\n");
	else
		printf("a <= b\n");
}

void test_23()
{
	char *p1 = "hello";
	char *p2 = "world";
	char *p3 = "a piece of cake";
	char *str[] = { p1, p2, p3 };
	printf("%c", *(str[0] + 1));
	char* test = *str + 1;
	char* temp = (char*)(str + 1);
	printf("%c", *(str + 1));
}


void test_24()
{
	//2. delete子类对象是一定会调用父类的析构函数的先调用子类的析构函数然后调用父类的析构函数。
	// delete 子类初始化的父类指针 不一定会调用子类析构函数，需要父类是虚析构函数
	class Base
	{
	public:
		Base(){}
		/*virtual*/ ~Base(){ cout << "Base destructed" << endl; }
	};

	class Derived : public Base
	{
	public:
		Derived():Base(){}
		~Derived() { cout << "Derived destructed" << endl; }
	};

	Derived* p = new Derived();
	delete p;
	Base* t = new Derived();
	delete t;
}

//#define NUMA 10000000
//#define NUMB 1000
//int a[NUMA], b[NUMB];
//void pa() {
//	int i, j;
//	for (i = 0; i < NUMB; ++i)
//		for (j = 0; j < NUMA; ++j)
//			++a[j];
//}
//void pb() {
//	int i, j;
//	for (i = 0; i < NUMA; ++i)
//		for (j = 0; j < NUMB; ++j)
//			++b[j];
//}
//
//答案:
//pb比pa快
//
//解答 :
//1. 二维数组操作时，因为缓存的原因，外层放小循环，内层放大循环效率高。
//2. 个人觉得这题是因为在给大数组赋值时会发生缺页，而小数组赋值不会，所以pb比较快。

void test_25()
{
	int a[2][5] = { { 1, 2, 3, 4, 5 },{ 6, 7, 8, 9, 10 } };
	int *ptr = (int *)(&a + 1);
	printf("%d\n", *(ptr - 3));

	int * test = (int *)(a + 1);
	printf("%d\n", *(test - 3));
}

void test_26()
{
	unsigned long val = 0;
	char a = 0x48;
	char b = 0x52;
	val = b << 8 | a;
}

void test_27()
{
	//	1. union：sizeof的取值不仅考虑sizeof最大的成员，还要考虑对齐字节。
	//	2. 注：如果结构体内类型的最大字节小于系统位数对应的字节，那么按类型的最大字节对齐。所以本题结构体按2字节对齐。
	typedef struct {
		char flag[3];
		short value;
	}sampleStruct;
	union {
		char flag[3];
		short value;
	}sampleUnion;

	cout << "sizeof(sampleStruct):" << sizeof(sampleStruct) << endl;
	cout << "sizeof(sampleUnion):" << sizeof(sampleUnion) << endl;
}

int func(int x, int y) {
	return (x + y);
}

void test_28()
{
	//1. 逗号表达式的结果是其最右边表达式的值。
	int a = 1, b = 2, c = 3, d = 4, e = 5;
	int k1 = (a + b, b + c, c + a);
	int k2 = (d, e);
	printf(" %d\n", func((a + b, b + c, c + a), (d, e)));
}

void test_29()
{
	int *ip = new int[12];
	for (int i = 0; i < 12; ++i)
		ip[i] = i;
	delete[]ip;
}

void test_30()
{
	//循环体一次也不执行 n = 0 返回值0 转成bool值false
	int m, n;
	for (m = 0, n = -1; n = 0; m++, n++)
		n++;
}

void test_31()
{
//#pragma pack(2)
	class BU {
		int number;
		struct UBffer {
			char buffer[13];
			int number;
		}ubuf;
		void foo() {}
		typedef char*(*f)(void*);
		enum { hdd, ssd, blueray }disk;
	}bu;

	cout << "sizeof(bu)：" << sizeof(bu) << endl;
}

void test_32()
{
	//  1. //后面被当做注释了。
	//	2. http本身是label。
    http://www.taobao.com
	cout << "welcome to taobao" << endl;
}

void test_33()
{
	//  1. szX和szY的值（指向的位置）相同。
	//	2. szX和szY指向的"abc"定义在常量区，不可以修改
	char acX[] = "abc";
	char acY[] = { 'a', 'b', 'c' };
	char *szX = "abc";
	char *szY = "abc";

	szX[0] = 'x';

	cout << "szX:" << szX << endl;
	cout << "szY:" << szY << endl;
}

void test_34()
{
	//  B->1
	//  virtual函数是动态绑定，而缺省参数值却是静态绑定。
	//	绝不重新定义继承而来的缺省参数值。
	class A {
	public:
		virtual void func(int val = 1) {
			std::cout << "A->" << val << std::endl;
		}
		virtual void test() {
			func();
		}
	};
	class B : public A {
	public:
		void func(int val = 0) {
			std::cout << "B->" << val << std::endl;
		}
	};


	B*p = new B;
	p->test();
	delete p;
}

#include <sstream>
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32


void test_35()
{

	std::ostringstream oss;
	oss << std::this_thread::get_id();
	std::string stid = oss.str();
	unsigned long long tid = std::stoull(stid);
	cout << "tid:" << tid << endl;
#ifdef _WIN32
	int ctid = (int)GetCurrentThreadId();
#endif // _WIN32
}

void Func(char str_arg[2]) {
	int m = sizeof(str_arg);
	int n = strlen(str_arg);
	printf("%d\n", m);
	printf("%d\n", n);
}

void test_36()
{
	char str[] = "Hello";
	Func(str);
}

void test_37()
{
	typedef char *String_t;
#define String_d char *
	String_t a, b;
	String_d c, d;
	//String_d c, d; -> char *c, d;
	//a, b, c是char*类型，而d为char类型。
}

void test_38()
{
#pragma pack(push)
#pragma pack(8)
	struct test
	{
		double d;
		int i[5];
		bool b;
		bool e;
		int k[5];
	};
	//数组是当成单个的依次放置
	test t{};
	cout << "sizeof(test): " << sizeof(test) << endl;
#pragma pack(pop)
}

#include "test.h"
void test_39()
{
	int tid1 = getThreadId();
	int tid2 = getThreadId2();

	cout << "thread id1:" << tid1 << endl;
	cout << "thread id2:" << tid2 << endl;
}

void test_40()
{
	class test
	{};

	cout << "sizeof(test)：" << sizeof(test) << endl;
}

void test_41()
{
	//构造函数顺序：基类-》成员-》自身，析构函数相反
	class A
	{
	public:
		A() {
			cout << "A constructor" << endl;
		}
		~A(){
			cout << "A destructor" << endl;
		}
	};

	class C
	{
	public:
		C() {
			cout << "C constructor" << endl;
		}
		~C() {
			cout << "C destructor" << endl;
		}
	};

	class B : public C
	{
	public:
		B() {
			cout << "B constructor" << endl;
		}
		~B() {
			cout << "B destructor" << endl;
		}

		A a;
	};

	B b;
}

#include <algorithm>
// 找出全部，改变数组，时间O(nlogn)，空间O(1)
void sort_version(int *arr) {
	sort(arr, arr + 7);
	for (int i = 0; i < 7; ++i) {
		int j;
		for (j = 1; arr[i] == arr[i + j]; ++j);
		if (j != 1) {
			cout << arr[i] << endl;
			i = i + j - 1;
		}
	}
}

#include <map>
// 找出全部，不改变数组，时间O(n)，空间O(n)
void hash_version(int *arr, int length) {
	map<int, int> map;
	// for(auto : arr)
	for (int i = 0; i < length; ++i) {
		++map[arr[i]];
	}
	for (auto var : map) {
		if (var.second != 1)
			cout << var.first << endl;
	}
}

void test_42()
{
	int arr[7] = { 2 ,3, 1, 0, 2, 5, 3 };
	sort_version(arr);
	hash_version(arr, 7);
}

int main()
{
	//test_1();
	//test_2();
	//test_3();
	//test_4();
	//test_5();
	//test_6();
	//test_7();
	//test_8();
	//test_9();
	//test_10();
	//test_11();
	//test_12();
	//test_13();
	//test_14();
	//test_15();
	//test_16();
	//test_17();
	//test_18();
	//test_19();
	//test_20();
	//test_21();
	//test_22();
	//test_23();
	//test_24();
	//test_25();
	//test_26();
	//test_27();
	//test_28();
	//test_29();
	//test_30();
	//test_31();
	//test_32();
	//test_33();
	//test_34();
	//test_35();
	//test_36();
	//test_37();
	//test_38();
	//test_39();
	//test_40();
	//test_41();
	test_42();

	return 0;
}
