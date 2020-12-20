#include <windows.h>
#include <stdio.h>

// 指定されたプロセスの仮想メモリの状態を標準出力に出力する。
// 
// 引数
//   procID  プロセスID。0を指定した場合には，呼び出したプロセスについて出力する。
//
// 戻り値
//   0   正常終了
//   -1  エラー (指定されたプロセスIDのプロセスが見つからない場合)
int VirtualWalk(DWORD procID)
{
	fprintf(stderr, "Virtual Walk Utility version 1.5\n");
	fprintf(stderr, "Programmed by H.Yasumuro\n\n");

	if (procID == 0) procID = GetCurrentProcessId();

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, procID);
	if (hProc == NULL) {
		fprintf(stderr, "指定されたプロセスIDのプロセスが見つかりません。\n");
		return -1;
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	DWORD mask = sysinfo.dwAllocationGranularity - 1;

	unsigned char* p = (unsigned char*)0x0;

	printf("Addr       Size(bytes) 状態      アクセス許可          タイプ/モジュール名\n");
	printf("==========================================================================\n");

	MEMORY_BASIC_INFORMATION mbi;
	while (1) {
		if (VirtualQueryEx(hProc, p, &mbi, sizeof(mbi))) {
			DWORD granuOffset = ((unsigned long)mbi.BaseAddress & mask);
			bool atBoundary = granuOffset == 0;
			if (mbi.State == MEM_FREE && !atBoundary) {
				SIZE_T size = sysinfo.dwAllocationGranularity - granuOffset;
				if (mbi.RegionSize > size)	mbi.RegionSize = size;
			}

			printf("0x%08lx 0x%08lx  ", mbi.BaseAddress, mbi.RegionSize);

			switch (mbi.State) {
			case MEM_COMMIT:
				printf("コミット  ");
				break;

			case MEM_RESERVE:
				printf("予約      ");
				break;

			case MEM_FREE:
				if (atBoundary)	printf("フリー    ");
				else			printf("予約余り  ");
				break;

			default:
				printf("          ");
			}

			switch (mbi.Protect) {
			case PAGE_NOACCESS:
				printf("アクセス不可          ");
				break;

			case PAGE_READONLY:
				printf("読み込みのみ可        ");
				break;

			case PAGE_READWRITE:
				printf("読み書き可            ");
				break;

			case PAGE_READWRITE | PAGE_GUARD:
				printf("読み書き可(ガード)    ");
				break;

			case PAGE_WRITECOPY:
				printf("書き込み時コピー      ");
				break;

			case PAGE_EXECUTE:
				printf("実行可                ");
				break;

			case PAGE_EXECUTE_READ:
				printf("実行/読み込みのみ可   ");
				break;

			case PAGE_EXECUTE_READWRITE:
				printf("実行/読み書き可       ");
				break;

			case PAGE_EXECUTE_WRITECOPY:
				printf("実行/書き込み時コピー ");
				break;

			default:
				if (mbi.Protect != 0) {
					printf("保護属性=0x%08lx   ", mbi.Protect);
				}
				else {
					printf("                      ");
				}
			}
			switch (mbi.Type) {
			case MEM_IMAGE:
				printf("イメージ    ");
				break;

			case MEM_MAPPED:
				printf("マップ      ");
				break;

			case MEM_PRIVATE:
				printf("プライベート");
				break;

			default:
				printf("            ");
			}
			printf("\n");

			p = (unsigned char*)mbi.BaseAddress + mbi.RegionSize;
			if (p == 0)	break;
		}
		else {
			printf("0x%08lx     --- アクセス不許可\n", p);
			break;
		}
	}
	printf("--------------------------------------------------------------------------\n\n");
	CloseHandle(hProc);
	return 0;
}
