%module CalculatorPluginSWIG

%{
#define SWIG_FILE_WITH_INIT
#include "AddIntent.hpp"
#include "SubstractIntent.hpp"
#include "RouterHelper.hpp"
#include "../../PolyPlugin/src/pp/IntentInfo.hpp"
#include "../../PolyPlugin/src/pp/Handler.hpp"
%}

%include "AddIntent.hpp"
%include "SubstractIntent.hpp"
%include "RouterHelper.hpp"
%include "../../PolyPlugin/src/pp/IntentInfo.hpp"
%include "../../PolyPlugin/src/pp/Handler.hpp"

%template(Process_AddIntent) processIntent<AddIntent>;
%template(AddIntentHandler) pp::HandlerImpl<AddIntent>;