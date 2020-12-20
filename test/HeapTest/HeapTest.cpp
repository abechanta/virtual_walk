#include "pch.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <stdio.h>
#include "../../VirtualWalk.h"
#include "../../VirtualWalk.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HeapTest
{
	TEST_CLASS(HeapTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// malloc関数/new演算子でヒープを割り当てる
			char* ptr1 = (char*)malloc(0x1000);
			printf("malloc:   0x%08lx\n", ptr1);

			char* ptr2 = new char[0x1000];
			printf("new:      0x%08lx\n", ptr2);

			char* ptr3 = (char*)malloc(0x1000);
			printf("malloc:   0x%08lx\n", ptr3);

			char* ptr4 = (char*)malloc(0x10000);
			printf("初期ヒープ・サイズを超えるmalloc:   0x%08lx\n", ptr4);
			printf("\n");

			// 3種類のAPIでヒープを割り当てる

			LPVOID ptrA = HeapAlloc(GetProcessHeap(), 0, 10000);
			printf("プロセス・ヒープ:   0x%08lx\n", ptrA);

			HGLOBAL gmem = GlobalAlloc(GMEM_MOVEABLE, 10000);
			LPVOID ptrB = GlobalLock(gmem);
			printf("グローバル・ヒープ: 0x%08lx\n", ptrB);

			HLOCAL lmem = LocalAlloc(LMEM_MOVEABLE, 10000);
			LPVOID ptrC = LocalLock(lmem);
			printf("ローカル・ヒープ:   0x%08lx\n\n", ptrC);

			int rc = VirtualWalk(0);

			// ヒープの解放
			LocalUnlock(lmem);
			LocalFree(lmem);

			GlobalUnlock(gmem);
			GlobalFree(gmem);

			HeapFree(GetProcessHeap(), 0, ptrA);

			free(ptr4);
			free(ptr3);
			delete[] ptr2;
			free(ptr1);

			Assert::AreEqual(rc, 0);
		}
	};
}
