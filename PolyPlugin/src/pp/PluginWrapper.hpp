#pragma once

#include <filesystem>

#include <pp/Defines.hpp>
#include <pp/PluginsContainer.hpp>

namespace pp
{
	class IPlugin;
	// Helper using statement to simplify usage of type of a function 
	// that is loaded from the shared library and provides instance of
	// IPlugin.
	using PluginCreatorType = IPlugin* STDCALL_CAST ();

	class PluginWrapper final
	{
	public:
		PluginWrapper() = default;
		PluginWrapper(PluginWrapper&& other) 
			: PluginWrapper{ other.m_libHandle, other.m_functionPtr }
		{ 
			m_pluginEntry = other.m_pluginEntry;
			other.m_libHandle = nullptr;
			other.m_functionPtr = nullptr;
			other.m_pluginEntry = nullptr;
		}

		PluginWrapper& operator=(PluginWrapper&& other)
		{
			m_libHandle = other.m_libHandle;
			m_functionPtr = other.m_functionPtr;
			m_pluginEntry = other.m_pluginEntry;
			other.m_libHandle = nullptr;
			other.m_functionPtr = nullptr;
			other.m_pluginEntry = nullptr;
			return *this;
		}
		
		void operator()() 
		{ 
			if (!m_pluginEntry)
				m_pluginEntry = std::shared_ptr<IPlugin>(m_functionPtr());
		}

		std::shared_ptr<IPlugin> operator->()
		{
			return m_pluginEntry;
		}

		bool isValid() const { return m_functionPtr != nullptr; }

		static PluginWrapper loadPluginEntryPoint(const std::filesystem::path& path)
		{
#if defined(_WIN32)
			HINSTANCE libHandle = LoadLibrary(LPCSTR(path.string().c_str()));
			if (!libHandle)
				return PluginWrapper{ nullptr, nullptr };

			void* pluginEntry = GetProcAddress(libHandle, "createPolyPlugin");
			auto w = GetLastError();
			if (!pluginEntry)
				return PluginWrapper{ nullptr, nullptr };

			return PluginWrapper(libHandle, reinterpret_cast<PluginCreatorType*>(pluginEntry));
#else 
			void* libHandle = dlopen(path.string().c_str(), RTLD_NOW);
			if (const char* err = dlerror())
				return PluginWrapper{ nullptr, nullptr };

			void* pluginEntry = dlsym(libHandle, "createPolyPlugin");
			if (const char* err = dlerror())
			{
				dlclose(libHandle);
				return PluginWrapper{ nullptr, nullptr };
			}

			return PluginWrapper(libHandle, reinterpret_cast<PluginCreatorType*>(pluginEntry));
#endif
		}

#if defined(_WIN32)
		~PluginWrapper()
		{ 
			if (m_pluginEntry)
				m_pluginEntry = nullptr;

			if (m_libHandle) 
				FreeLibrary(m_libHandle);
		}
	private:
		PluginWrapper(HINSTANCE libHandle, PluginCreatorType* funcHandle) : m_libHandle(libHandle), m_functionPtr(funcHandle) {}
		HINSTANCE m_libHandle = nullptr;
#else
		~PluginWrapper()
		{
			if (m_pluginEntry)
				m_pluginEntry = nullptr;

			if (m_libHandle) 
				dlclose(m_libHandle);
		}
	private:
		PluginWrapper(void* libHandle, PluginCreatorType* funcHandle) : m_libHandle(libHandle), m_functionPtr(funcHandle) {}
		void* m_libHandle = nullptr;
#endif
		
		PluginCreatorType* m_functionPtr = nullptr;
		std::shared_ptr<IPlugin> m_pluginEntry = nullptr;
	};

}