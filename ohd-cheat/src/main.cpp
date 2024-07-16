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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	memory mem;
	network net;

	int user_choice = MessageBoxA(NULL, "Would you like to inject the cheat?", "Question", MB_YESNO | MB_ICONQUESTION);

	if (user_choice == IDNO)
		return 0;

	if (!net.get_dll())
		return 0;

	mem.dll_path = net.dll_path;

	if (!mem.init())
		return 0;

	mem.delete_dll();

	return 0;
}