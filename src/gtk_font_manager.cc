#include <napi.h>
#include <gtk/gtk.h>
// #include <pango/pango.h>
// #include <pango/pango-context.h>
#include <fontconfig/fontconfig.h>
#include <iostream> // TODO remove this

/**
 * @brief ressources
 * fontconfig doc: https://www.freedesktop.org/software/fontconfig/fontconfig-devel/x31.html
 * find font with gtk: https://stackoverflow.com/questions/57982315/is-there-a-way-to-get-a-list-of-all-installed-fonts-in-the-system-in-gtk
 */

class FontConfigWrapper
{
  public:
    FontConfigWrapper(/* args */);
    ~FontConfigWrapper();

    Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info);
    Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info);
    Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info);

  private:
    FcConfig *m_config;
    FcPattern *m_pattern;
    FcObjectSet *m_set;
    FcFontSet *m_fontSet;
};

FontConfigWrapper::FontConfigWrapper()
{
  std::cout << "create" << std::endl;
  FcInit();
  m_config = FcInitLoadConfigAndFonts();
  m_pattern = FcPatternCreate();
  m_set = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
  m_fontSet = FcFontList(m_config, m_pattern, m_set);
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

  FcConfig *m_config = FcInitLoadConfigAndFonts();
  FcPattern *m_pattern = FcPatternCreate();
  FcObjectSet *m_set = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
  // passing null here use the default configuration
  FcFontSet *m_fontSet = FcFontList(m_config, m_pattern, m_set);

  // init result
  Napi::Array fontFamilies = Napi::Array::New(env, m_fontSet->nfont);
  for (int i = 0; m_fontSet && i < m_fontSet->nfont; i++) {
    FcPattern *currentPattern = m_fontSet->fonts[i];
    FcChar8 *family;
    if (FcPatternGetString(currentPattern, FC_FAMILY, 0, &family) == FcResultMatch) {
      fontFamilies[i] = std::string(reinterpret_cast<const char*>(family)); // not sure cast is the best idea, return uchar string ?
    }
  }
  return fontFamilies;
}



Napi::Object Init(Napi::Env env, Napi::Object exports) {
  static FontConfigWrapper fc;
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"), Napi::Function::New(env, std::bind(&FontConfigWrapper::GetAvailableFontFamilies, fc, std::placeholders::_1))
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)