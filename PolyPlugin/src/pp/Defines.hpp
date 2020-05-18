// Handle __stdcall
#if defined(_WIN32)
    #define STDCALL __stdcall
    #define STDCALL_CAST (__stdcall *)
#else
    #define STDCALL
    #define STDCALL_CAST (*)
#endif

// Handle includes
#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

// Handle exporting symbols
#if defined(_WIN32)
    #define PP_EXPORT __declspec(dllexport)
#else
    #define PP_EXPORT __attribute__((visibility ("default")))
#endif
