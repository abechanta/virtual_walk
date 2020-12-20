#include "pch.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <stdio.h>
#include "../../VirtualWalk.h"
#include "../../VirtualWalk.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void test3(DWORD procID)
{
	char buffer[0x100000];
	printf("スタックの最大許容サイズを超える --> buffer: 0x%08lx\n", buffer);
	VirtualWalk(procID);
}

void test2(DWORD procID)
{
	// 初期コミット領域を超えるローカル変数
	char buffer[0x3000];
	printf("初期コミット領域を超える --> buffer: 0x%08lx\n", buffer);
	VirtualWalk(procID);
	test3(procID);
}

void test(DWORD procID)
{
	char buffer[0x100];
	printf("初期コミット領域内 --> buffer: 0x%08lx\n", buffer);
	VirtualWalk(procID);
	test2(procID);
}

namespace StackTest
{
	TEST_CLASS(StackTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			DWORD procID = 0;
			int rc = VirtualWalk(procID);
			test(procID);
			Assert::AreEqual(rc, 0);
		}
	};
}
