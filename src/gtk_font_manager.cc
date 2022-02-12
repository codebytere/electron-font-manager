#include <napi.h>
// #include <gtk/gtk.h>

Napi::Value ShowFontPanel(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Function emit = info[0].As<Napi::Function>();
  Napi::Object options = info[1].As<Napi::Object>();

  std::string title;
  Napi::Value titleValue = options.Get("title");
  if (titleValue.IsString())
      title = titleValue.As<Napi::String>().Utf8Value();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  // GtkFontChooser *fd = (GtkFontChooser*)gtk_font_chooser_dialog_new (title.c_str(), NULL);

  // gtk_widget_show(GTK_WIDGET(fd));

  return deferred.Promise();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );

  return exports;
}

NODE_API_MODULE(font_manager, Init)
