#include <napi.h>
#include <gtk/gtk.h>

void ShowFontPanel(const Napi::CallbackInfo &info) {
  std::string title = info[0].As<Napi::String>().Utf8Value();

  GtkFontChooser *fd = (GtkFontChooser*)gtk_font_chooser_dialog_new (title.c_str(), NULL);

  gtk_widget_show(GTK_WIDGET(fd));
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)