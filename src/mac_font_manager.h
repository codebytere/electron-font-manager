#include <napi.h>

Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info);

Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info);

Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info);