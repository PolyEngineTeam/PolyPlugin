%module PolyPluginSWIG

%{
#define SWIG_FILE_WITH_INIT
#include "pp/IntentInfo.hpp"
#include "pp/PluginInfo.hpp"
#include "pp/IntentRouter.hpp"
#include "pp/PluginsContainer.hpp"
%}

%feature("director") pp::IPlugin;

%include "IntentInfo.hpp"
%include "PluginInfo.hpp"
%include "IntentRouter.hpp"
%include "PluginsContainer.hpp"