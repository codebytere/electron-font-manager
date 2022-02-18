#include "FontConfigWrapper.hpp"

/**
 * @brief ressources
 * fontconfig doc: https://www.freedesktop.org/software/fontconfig/fontconfig-devel/x31.html
 * find font with gtk: https://stackoverflow.com/questions/57982315/is-there-a-way-to-get-a-list-of-all-installed-fonts-in-the-system-in-gtk
 */

FontConfigWrapper::FontConfigWrapper()
{
  std::cout << "create" << std::endl;
  FcInit();
  m_config = FcInitLoadConfigAndFonts();
  m_pattern = FcPatternCreate();
}

FontConfigWrapper::~FontConfigWrapper() // TODO handle correctly destruction
{
  // FcFontSetDestroy(m_fontSet);
  // FcObjectSetDestroy(m_set);
  // FcPatternDestroy(m_pattern);
  // FcConfigDestroy(m_config);
  // FcFini();
}


void ShowFontPanel(const Napi::CallbackInfo &info) {
  std::string title = info[0].As<Napi::String>().Utf8Value();

  GtkFontChooser *fd = (GtkFontChooser*)gtk_font_chooser_dialog_new (title.c_str(), NULL);

  gtk_widget_show(GTK_WIDGET(fd));
}

// trying with gtk
// Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info) {
//   // TODO Parameters gestion
  
//   PangoContext *context = pango_context_new();
//   PangoFontMap 
//   // retrieve fonts
  
//   PangoFontFamily **fontFamilies;
//   int size = 0;
//   context->pango_context_list_families(&fontFamilies, &size);
//   std::cout << size << std::endl;
//   for (int i = 0; i < size; i++) {
//     std::cout << pango_font_family_get_name(fontFamilies[i]) << std::endl;
//   }
//   return {};
// }

// with fontconfig
Napi::Array FontConfigWrapper::GetAvailableFontFamilies(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  m_set = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
  m_fontSet = FcFontList(m_config, m_pattern, m_set);

  std::vector<std::string>vecFontFamilies;
  for (int i = 0; m_fontSet && i < m_fontSet->nfont; i++) {
    FcPattern *currentPattern = m_fontSet->fonts[i];
    FcChar8 *family;
    if (FcPatternGetString(currentPattern, FC_FAMILY, 0, &family) == FcResultMatch) {
      std::string familyStr = reinterpret_cast<const char*>(family); // not sure cast is the best idea, return uchar string ?
      if (std::find(vecFontFamilies.begin(), vecFontFamilies.end(), familyStr) == vecFontFamilies.end())
        vecFontFamilies.push_back(familyStr);
    }
  }
  // fill result
  Napi::Array fontFamilies = Napi::Array::New(env, vecFontFamilies.size());
  for (int i = 0; i < vecFontFamilies.size(); i++) {
    fontFamilies[i] = vecFontFamilies[i];
  }
  return fontFamilies;
  // TODO cleanning ressources
}

std::vector<TraitComparator> FontConfigWrapper::parseTraits(const Napi::Array &traits) {
  std::vector<TraitComparator> res;
  int traits_length = static_cast<int>(traits.Length());
  for (int i = 0; i < traits_length; i++) {
    std::string trait = traits.Get(i).As<Napi::String>().Utf8Value();
    res.push_back(_traitsConditions[trait]);
  }
  return res;
}

Napi::Array FontConfigWrapper::GetAvailableFonts(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  // param gestion
  Napi::Object params = info[0].As<Napi::Object>();
  std::vector<TraitComparator> traitsFilters;
  if (params.Has("traits")) {
    Napi::Array traits = params.Get("traits").As<Napi::Array>();
    traitsFilters = parseTraits(traits);
  }

  m_set = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, FC_WIDTH, FC_FULLNAME, (char *) 0);
  m_fontSet = FcFontList(m_config, m_pattern, m_set);

  std::vector<std::string> vecFontNames;
  for (int i = 0; m_fontSet && i < m_fontSet->nfont; i++) {
    FcPattern *currentPattern = m_fontSet->fonts[i];
    FcChar8 *name;

    bool passAllFilters = true;
    //handle traits
    if (traitsFilters.size() > 0)
      for (const auto &filter : traitsFilters) {
        passAllFilters = passAllFilters && filter(currentPattern);
      }
    // sometime, fonts do not have a full name. What to do in this case ?
    if (passAllFilters && FcPatternGetString(currentPattern, FC_FULLNAME, 0, &name) == FcResultMatch) {
      vecFontNames.push_back(std::string(reinterpret_cast<const char*>(name)));
    }
  }

  // fill result
  Napi::Array fontNames = Napi::Array::New(env, vecFontNames.size());
  for (int i = 0; i < vecFontNames.size(); i++) {
    fontNames[i] = vecFontNames[i];
  }
  return fontNames;
  // TODO cleanning ressources
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
  static FontConfigWrapper fc;
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"),
    Napi::Function::New(env, std::bind(&FontConfigWrapper::GetAvailableFontFamilies, fc, std::placeholders::_1))
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFonts"),
    Napi::Function::New(env, std::bind(&FontConfigWrapper::GetAvailableFonts, fc, std::placeholders::_1))
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)