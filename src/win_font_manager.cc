#include <napi.h>

Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Array font_families = Napi::Array::New(env, num_font_families);

  InstalledFontCollection installedFontCollection;
  INT num_font_families = installedFontCollection.GetFamilyCount();
  INT num_families_found = 0;
  WCHAR family_name[LF_FACESIZE];
  FontFamily* arr_font_families = new FontFamily[num_font_families];

  installedFontCollection.GetFamilies(num_font_families, arr_font_families, &num_families_found);

  for(INT idx = 0; idx < num_font_families; ++idx) {
    arr_font_families[idx].GetFamilyName(family_name);
    std::string converted_string(family_name.begin(), family_name.end());
    font_families[idx] = converted_string;
  }

  delete [] arr_font_families;

  return font_families;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"), Napi::Function::New(env, GetAvailableFontFamilies)
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)