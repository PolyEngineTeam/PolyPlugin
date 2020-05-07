#include <iostream>

#include <pp/PolyPlugin.hpp>
#include <AddIntent.hpp>
#include <windows.h>

//------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
	pp::PluginsContainer container;
	char ownPth[MAX_PATH];

	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
		container.load(std::filesystem::path(ownPth).parent_path(), false);
	}
    
	AddIntent intent{ 2, 3 };
	std::optional<AddIntent::Result> result = container.getRouter()->processIntent(std::move(intent));
	if (result.has_value())
		std::cout << "2 + 3 = " << result.value() << std::endl;
	else
		std::cout << "Couldn't find handler for AddIntent. Check if it exists and uses the correct major version of PolyPlugin." << std::endl;

    std::cout << std::endl << std::endl;

    return 0;
}