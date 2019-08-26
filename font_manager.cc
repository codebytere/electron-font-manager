#include <napi.h>
#include "src/mac_font_manager.h"

// Initializes all functions exposed to JS
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "getAvailableFonts"), Napi::Function::New(env, GetAvailableFonts)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"), Napi::Function::New(env, GetAvailableFontFamilies)
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)