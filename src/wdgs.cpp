#include "application.h"
#include "debug.h"

#include <map>

#ifdef WIN32

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	glm::vec3 v = glm::reflect(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);

	int argc;
	char ** argv;

	LPWSTR* lpArgv = CommandLineToArgvW(GetCommandLineW(), &argc);
	argv = new char*[argc];

	int wlen;
	for (int i=0; i < argc; ++i)
	{
		wlen = wcslen(lpArgv[i]);
		argv[i] = new char[wlen + 1];
		WideCharToMultiByte(CP_ACP, 0, lpArgv[i], wlen, argv[i], wlen + 1, 0, NULL);
		argv[i][wlen] = 0;
	}
	LocalFree(lpArgv);

	int r = WDGS::Application::Run(argc, (const char**)argv);

	for (int i=0; i < argc; ++i)
		delete[] argv[i];

	delete[] argv;

	return r;
}

#else
int main(int argc, const char ** argv)
{
	return WDGS::Application::Run(argc, argv);
}
#endif