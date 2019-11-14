#ifdef _WIN32
#include <Windows.h>
#endif
#include <unordered_map>
#include <fstream>
#include <thread>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

std::unordered_map<std::string, std::ofstream> file_handles;

extern "C" EXPORT const char* open_file(int n_args, const char** args)
{
	std::string filename(args[0]);
	if (file_handles.find(filename) != file_handles.end())
	{
		return "WARNING: File already open.";
	}
	file_handles.emplace(filename, std::ofstream(filename, std::ios::app));
	std::ofstream& file = file_handles[filename];
	if (!file)
	{
		file_handles.erase(filename);
		return "ERROR: Failed to open file.";
	}
	return "";
}

extern "C" EXPORT const char* close_file(int n_args, const char** args)
{
	std::string filename(args[0]);
	if (file_handles.find(filename) != file_handles.end())
	{
		std::ofstream& file = file_handles[filename];
		file.close();
		file_handles.erase(filename);
		return "";
	}
	return "ERROR: Attempted to close unopened file.";
}

extern "C" EXPORT const char* write_file(int n_args, const char** args)
{
	std::string filename(args[0]);
	if (file_handles.find(filename) != file_handles.end())
	{
		file_handles[filename] << args[1];
		return "";
	}
	return "ERROR: Attempted to write to unopened file.";
}

extern "C" EXPORT const char* close_all(int n_args, const char** args)
{
	for (auto it = file_handles.begin(); it != file_handles.end(); )
	{
		it->second.close();
		it = file_handles.erase(it);
	}
	return "";
}

#ifdef _WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif