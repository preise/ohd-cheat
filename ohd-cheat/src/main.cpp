/* ############################################# */
/* #██████╗ ██████╗ ███████╗██╗███████╗███████╗# */
/* #██╔══██╗██╔══██╗██╔════╝██║██╔════╝██╔════╝# */
/* #██████╔╝██████╔╝█████╗  ██║███████╗█████╗  # */
/* #██╔═══╝ ██╔══██╗██╔══╝  ██║╚════██║██╔══╝  # */
/* #██║     ██║  ██║███████╗██║███████║███████╗# */
/* #╚═╝     ╚═╝  ╚═╝╚══════╝╚═╝╚══════╝╚══════╝# */
/* ############################################# */
#include "memory.h"
#include "network.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	memory mem;
	network net;

	int user_choice = MessageBoxA(NULL, "Would you like to inject the cheat?", "Question", MB_YESNO | MB_ICONQUESTION);

	if (user_choice == IDNO)
		return 0;

	if (!net.get_dll())
		return 0;

	mem.dll_path = net.dll_path;

	if (mem.get_hash() != net.get_hash())
	{
		user_choice = MessageBoxA(NULL, "DLL is outdated or not from github.com/preise/ohd-dll/releases.\nStill inject dll?", "Question", MB_YESNO | MB_ICONQUESTION);

		if (user_choice == IDNO)
			return 0;
	}

	if (!mem.init())
		return 0;


	mem.delete_dll();

	return 0;
}