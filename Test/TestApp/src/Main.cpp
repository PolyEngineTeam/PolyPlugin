#include <iostream>

#include <pp/PolyPlugin.hpp>
#include <pp/Defines.hpp>
#include <AddIntent.hpp>

//------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
	pp::PluginsContainer container;

// probably Unix approach is crossplatform
#if defined(_WIN32)
	char ownPth[MAX_PATH];
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
		container.load(std::filesystem::path(ownPth).parent_path(), false);
	}
#else
	std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
	container.load(std::filesystem::current_path(), false);
#endif
    
	AddIntent intent{ 2, 3 };
	std::optional<AddIntent::Result> result = container.getRouter()->processIntent(std::move(intent));
	if (result.has_value())
		std::cout << "2 + 3 = " << result.value() << std::endl;
	else
		std::cout << "Couldn't find handler for AddIntent. Check if it exists and uses the correct major version of PolyPlugin." << std::endl;

    std::cout << std::endl << std::endl;

    return 0;
}