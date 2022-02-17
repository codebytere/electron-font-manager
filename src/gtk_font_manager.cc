#include <napi.h>
#include <gtk/gtk.h>
// #include <pango/pango.h>
// #include <pango/pango-context.h>
#include <fontconfig/fontconfig.h>
#include <iostream> // TODO remove this

/**
 * @brief ressources
 * fontconfig softare: https://www.freedesktop.org/software/fontconfig/fontconfig-devel/x19.html
 * find font with gtk: https://stackoverflow.com/questions/57982315/is-there-a-way-to-get-a-list-of-all-installed-fonts-in-the-system-in-gtk
 */

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
Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info) {
  // TODO Parameters gestion
  Napi::Env env = info.Env();

  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcPatternCreate();
  FcObjectSet *set = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
  // passing null here use the default configuration
  FcFontSet *fontSet = FcFontList(config, pattern, set);

  // init result
  Napi::Array fontFamilies = Napi::Array::New(env, fontSet->nfont);
  for (int i = 0; fontSet && i < fontSet->nfont; i++) {
    FcPattern *currentPattern = fontSet->fonts[i];
    FcChar8 *family;
    if (FcPatternGetString(currentPattern, FC_FAMILY, 0, &family) == FcResultMatch) {
      fontFamilies[i] = std::string(reinterpret_cast<const char*>(family)); // not sure cast is the best idea, return uchar string ?
    }
  }
  return fontFamilies;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"), Napi::Function::New(env, GetAvailableFontFamilies)
  );
  FcInit();

  return exports;
}

NODE_API_MODULE(font_manager, Init)