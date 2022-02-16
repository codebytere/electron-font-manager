#include <napi.h>
#include <gtk/gtk.h>

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

Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info) {
  // TODO Parameters gestion
  
  PangoContext *context = gtk_widget_create_pango_context(nullptr);
  // retrieve fonts
  PangoFontMap *fontmap = pango_context_get_font_map(context); // https://github.com/GNOME/pango/blob/main/pango/pango-fontmap.h
  PangoFontFamily **fontFamilies;
  int size = 0;
  fontmap->pango_font_map_list_families(&fontFamilies, &size);
  return {};
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)