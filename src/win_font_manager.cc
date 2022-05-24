#include <napi.h>

#include <set>
#include <string>
#include <vector>

#include <windows.h>
#include <strsafe.h>

/***** EXPORTED FUNCTIONS *****/

typedef std::basic_string<WCHAR> FontString;

struct FindFontResults {
  std::vector<ENUMLOGFONTEXW> fonts;
  std::set<FontString> found;
};

int CALLBACK FindOneFont(
   const LOGFONTW   *lpelfe,
   const TEXTMETRICW*lpntme,
         DWORD      FontType,
         LPARAM     lParam
)
{
  (void)FontType;
  const ENUMLOGFONTEXW* logFont = (ENUMLOGFONTEXW*)lpelfe;
  FindFontResults* results = (FindFontResults*)lParam;
  FontString key(logFont->elfFullName);
  if (results->found.find(key) == results->found.end())
  {
    results->fonts.push_back(*logFont);
    results->found.insert(key);
  }
  return 1;
}

int CALLBACK FindOneFontFamily(
   const LOGFONTW   *lpelfe,
   const TEXTMETRICW*lpntme,
         DWORD      FontType,
         LPARAM     lParam
)
{
  (void)FontType;
  const ENUMLOGFONTEXW* logFont = (ENUMLOGFONTEXW*)lpelfe;
  FindFontResults* results = (FindFontResults*)lParam;
  FontString key(logFont->elfLogFont.lfFaceName);
  if (results->found.find(key) == results->found.end())
  {
    results->fonts.push_back(*logFont);
    results->found.insert(key);
  }
  return 1;
}

Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  LOGFONTW logFont;
  FindFontResults results;

  // To enumerate all styles and charsets of all fonts:
  memset(&logFont, 0, sizeof(logFont));
  logFont.lfFaceName[0] = '\0';
  logFont.lfCharSet = ANSI_CHARSET;//DEFAULT_CHARSET;
  HDC hdc = GetDC(GetTopWindow(NULL));
  int result = EnumFontFamiliesExW(
                        hdc,
                        &logFont,
                        FindOneFont,
                        (LPARAM)&results,
                        0);
  Napi::Array fonts = Napi::Array::New(env, results.fonts.size());
  size_t index = 0;
  for (std::vector<ENUMLOGFONTEXW>::iterator i = results.fonts.begin();
       i != results.fonts.end();
       ++i) {
    fonts[index++] = (const char16_t*)i->elfFullName;
  }

  return fonts;
}

Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  LOGFONTW logFont;
  FindFontResults results;

  // To enumerate all styles and charsets of all fonts:
  memset(&logFont, 0, sizeof(logFont));
  logFont.lfFaceName[0] = '\0';
  logFont.lfCharSet = DEFAULT_CHARSET;
  HDC hdc = GetDC(GetTopWindow(NULL));
  int result = EnumFontFamiliesExW(
                        hdc,
                        &logFont,
                        FindOneFontFamily,
                        (LPARAM)&results,
                        0);
  Napi::Array font_families = Napi::Array::New(env, results.fonts.size());
  size_t index = 0;
  for (std::vector<ENUMLOGFONTEXW>::iterator i = results.fonts.begin();
       i != results.fonts.end();
       ++i) {
    font_families[index++] = (const char16_t*)i->elfLogFont.lfFaceName;
  }

  return font_families;
}

Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  std::u16string family = info[0].As<Napi::String>().Utf16Value();

  LOGFONTW logFont;
  FindFontResults results;
  HRESULT hr;

  // To enumerate all styles and charsets of all fonts:
  memset(&logFont, 0, sizeof(logFont));
  hr = StringCchCopyW(logFont.lfFaceName, LF_FACESIZE, (LPCWSTR)family.c_str());
  if (FAILED(hr))
  {
    return Napi::Array::New(env, 0);
  }
  logFont.lfCharSet = DEFAULT_CHARSET;
  HDC hdc = GetDC(GetTopWindow(NULL));
  int result = EnumFontFamiliesExW(
                        hdc,
                        &logFont,
                        FindOneFont,
                        (LPARAM)&results,
                        0);
  Napi::Array font_families = Napi::Array::New(env, results.fonts.size() * 3);
  size_t index = 0;
  for (std::vector<ENUMLOGFONTEXW>::iterator i = results.fonts.begin();
       i != results.fonts.end();
       ++i) {
    font_families[index++] = (const char16_t*)i->elfFullName;
    font_families[index++] = (const char16_t*)i->elfStyle;
    font_families[index++] = i->elfLogFont.lfWeight;
  }

  return font_families;
}

Napi::Value ShowFontPanel(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Function emit = info[0].As<Napi::Function>();
  Napi::Object options = info[1].As<Napi::Object>();

  std::u16string title;
  Napi::Value titleValue = options.Get("title");
  if (titleValue.IsString())
      title = titleValue.As<Napi::String>().Utf16Value();
  DWORD flags = CF_SCREENFONTS | CF_SCALABLEONLY;

  LOGFONTW logFont;
  memset(&logFont, 0, sizeof(logFont));

  CHOOSEFONTW chooseFontStruct;
  memset(&chooseFontStruct, 0, sizeof(chooseFontStruct));
  chooseFontStruct.lStructSize = sizeof(CHOOSEFONTW);
  chooseFontStruct.hwndOwner = GetTopWindow(NULL);
  chooseFontStruct.lpLogFont = &logFont;
  Napi::Value familyValue = options.Get("family");
  if (familyValue.IsString()) {
    StringCchCopyW(logFont.lfFaceName, LF_FACESIZE, familyValue.As<Napi::String>().Utf16Value());
    flags |= CF_INITTOLOGFONTSTRUCT;
  }
  std::vector<TCHAR> styleBuf;
  Napi::Value styleValue = options.Get("style");
  if (styleValue.IsString()) {
    styleBuf.resize(LF_FACESIZE);
    chooseFontStruct.lpszStyle = &styleBuf[0];
    StringCchCopyW(chooseFontStruct.lpszStyle, LF_FACESIZE, styleValue.As<Napi::String>().Utf16Value());
    flags |= CF_USESTYLE;
  }
  Napi::Value pointSizeValue = options.Get("pointSize");
  if (pointSizeValue.IsNumber())
    chooseFontStruct.iPointSize = (INT)pointSizeValue.As<Napi::Number>().Int32Value();
  Nap::Value traitsIn = options.Get("traits");
  if (traitsIn.IsArray()) {
    Napi::Array traits = traits.As<Napi::Array>();
    int traits_length = static_cast<int>(traits.Length());
    for (int i = 0; i < traits_length; i++) {
      std::string trait = traits.Get(i).As<Napi::String>().Utf8Value();
      if (trait == "bold") {
        logFont.lfWeight = FW_BOLD;
        flags |= CF_INITTOLOGFONTSTRUCT;
      } else if (trait == "italic") {
        logFont.lfItalic = TRUE;
        flags |= CF_INITTOLOGFONTSTRUCT;
      }
    }
  }
  chooseFontStruct.Flags = flags;

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  if (ChooseFontW(&chooseFontStruct) != 0)
  {
      Napi::Object obj = Napi::Object::New(env);
      obj.Set("family", Napi::String::New(env, (const char16_t*)logFont.lfFaceName));
      if (chooseFontStruct.lpszStyle)
          obj.Set("style", Napi::String::New(env, (const char16_t*)chooseFontStruct.lpszStyle));
      obj.Set("pointSize", Napi::Number::New(env, chooseFontStruct.iPointSize));
      std::vector<std::string> traits;
      if ((chooseFontStruct.nFontType & BOLD_FONTTYPE) != 0)
        traits.push_back("bold");
      if ((chooseFontStruct.nFontType & ITALIC_FONTTYPE) != 0)
        traits.push_back("italic");
      Napi::Array t = Napi::Array::New(env, traits.size());
      for (size_t i = 0; i < traits.size(); i++)
        t[i] = traits[i];
      obj.Set(Napi::String::New(env, "traits"), t);
      emit.Call({Napi::String::New(env, "fontSelected"), obj});
      deferred.Resolve(obj);
  }
  else
  {
    deferred.Reject(Napi::String::New(env, "user cancel"));
  }
  return deferred.Promise();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "getAvailableFonts"), Napi::Function::New(env, GetAvailableFonts)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"), Napi::Function::New(env, GetAvailableFontFamilies)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableMembersOfFontFamily"), Napi::Function::New(env, GetAvailableMembersOfFontFamily)
  );
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)
