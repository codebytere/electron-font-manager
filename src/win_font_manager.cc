#include <napi.h>

/***** EXPORTED FUNCTIONS *****/

int CALLBACK EnumFontFamExProc(
   const LOGFONT    *lpelfe,
   const TEXTMETRIC *lpntme,
         DWORD      FontType,
         LPARAM     lParam
)
{
  const ENUMLOGFONTEX* logFont = (ENUMLOGFONTEX*)lpelfe;
  std::vector<ENUMLOGFONTEX>* results = (std::vector<ENUMLOGFONTEX>*)lParam;
  if (FontType == TRUETYPE_FONTTYPE)
  {
    results->append(lParam);
    logFont->elfFullName;
    logFont->elfStyle;
  }
}

Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info) {
  LOGFONT logFont;
  std::vector<ENUMLOGFONTEX> results;

  // To enumerate all styles and charsets of all fonts:
  lf.lfFaceName[0] = '\0';
  lf.lfCharSet = DEFAULT_CHARSET;
  int result = EnumFontFamiliesExW(
                        hdc,
                        &logFont,
                        EnumFontFamExProc,
                        (LPARAM)&results,
                        0);
  Napi::Array font_families = Napi::Array::New(env, results.size());
  for (std::vector<ENUMLOGFONTEX>::iterator i = results.begin();
       i != results.end();
       ++) {
    font_families[i] = results[i]->elfFullName;
    font_families[i] = results[i]->elfStyle;
  }
}

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

Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info) {
  // To enumerate all styles and character sets of the Arial font:
  hr = StringCchCopy((LPSTR)lf.lfFaceName, LF_FACESIZE, "Arial");
  if (FAILED(hr))
  {
    // TODO: write error handler
  }
  lf.lfCharSet = DEFAULT_CHARSET;
}

void ShowFontPanel(const Napi::CallbackInfo &info) {
  bool show_styles = info[0].As<Napi::Boolean>().Value();
  // It should be OK to always use GDI simulations
  DWORD flags = CF_SCREENFONTS /* | CF_NOSIMULATIONS */ ;

  LOGFONT logFont;

  CHOOSEFONT chooseFontStruct;
  chooseFontStruct.lStructSize = sizeof(CHOOSEFONT);
  chooseFontStruct.hwndOwner = GetTopWindow(NULL);
  chooseFontStruct.lpLogFont = &logFont;

  if ( m_fontData.GetRestrictSelection() & wxFONTRESTRICT_SCALABLE )
    flags |= CF_SCALABLEONLY;
  chooseFontStruct.Flags = flags;
  if ( ChooseFont(&chooseFontStruct) != 0 )
  {
    wxRGBToColour(m_fontData.m_fontColour, chooseFontStruct.rgbColors);
    m_fontData.m_chosenFont = wxFont(wxNativeFontInfo(logFont, this));
    m_fontData.EncodingInfo().facename = logFont.lfFaceName;
    m_fontData.EncodingInfo().charset = logFont.lfCharSet;

    return wxID_OK;
  }
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
