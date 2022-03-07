#include <iostream>
#include <napi.h>
#include <gtk/gtk.h>
#include <fontconfig/fontconfig.h>
#include <algorithm>
#include <unordered_map>

/**
 * @brief ressources
 * fontconfig doc: https://www.freedesktop.org/software/fontconfig/fontconfig-devel/x31.html
 * find font with gtk: https://stackoverflow.com/questions/57982315/is-there-a-way-to-get-a-list-of-all-installed-fonts-in-the-system-in-gtk
 */

using TraitComparator = std::function<void(FcPattern *)>;

void ShowFontPanel(const Napi::CallbackInfo &info) {
  std::string title = info[0].As<Napi::String>().Utf8Value();

  GtkFontChooser *fd = (GtkFontChooser*)gtk_font_chooser_dialog_new (title.c_str(), NULL);

  gtk_widget_show(GTK_WIDGET(fd));
}

void parseTraits(const Napi::Array &traits, FcPattern *pattern) {
  /*! map applying options of front trait to a pattern */
  static std::unordered_map<std::string, TraitComparator> traitsConditions = {
      {"bold", TraitComparator([](FcPattern * pat) {
          FcPatternAddString(pat, FC_STYLE, reinterpret_cast<const FcChar8 *>("Bold"));
      })},
      {"condensed", TraitComparator([](FcPattern * pat) {
          FcRange *range = FcRangeCreateInteger(FC_WIDTH_ULTRACONDENSED, FC_WIDTH_CONDENSED);
          FcPatternAddRange(pat, FC_WIDTH, range);
      })},
      {"expanded", TraitComparator([](FcPattern * pat) {
          FcRange *range = FcRangeCreateInteger(FC_WIDTH_EXPANDED, FC_WIDTH_ULTRAEXPANDED);
          FcPatternAddRange(pat, FC_WIDTH, range);
      })},
      {"italic", TraitComparator([](FcPattern * pat) {
          FcPatternAddString(pat, FC_STYLE, reinterpret_cast<const FcChar8 *>("Italic"));
      })},
      // {"unbold", TraitComparator([](FcPattern * pat) {
      // })},
      // {"unitalic", TraitComparator([](FcPattern * pat) {
      // })},
  };

  int traits_length = static_cast<int>(traits.Length());
  for (int i = 0; i < traits_length; i++) {
    std::string trait = traits.Get(i).As<Napi::String>().Utf8Value();
    traitsConditions[trait](pattern);
  }
}

Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info) {
  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcPatternCreate();
  Napi::Env env = info.Env();

  // param gestion
  Napi::Object params = info[0].As<Napi::Object>();
  if (params.Has("traits")) {
    Napi::Array traits = params.Get("traits").As<Napi::Array>();
    parseTraits(traits, pattern);
  }

  FcObjectSet *set = FcObjectSetBuild(FC_FAMILY, FC_FULLNAME, FC_STYLE, FC_WIDTH, (char *) 0);
  FcFontSet *fontSet = FcFontList(config, pattern, set);

  std::vector<std::string> vecFontNames;
  for (int i = 0; fontSet && i < fontSet->nfont; i++) {
    FcPattern *currentPattern = fontSet->fonts[i];
    FcChar8 *name;
    if (FcPatternGetString(currentPattern, FC_FULLNAME, 0, &name) == FcResultMatch) {
      vecFontNames.push_back(std::string(reinterpret_cast<const char*>(name)));
    }
  }

  // fill result
  Napi::Array fontNames = Napi::Array::New(env, vecFontNames.size());
  for (unsigned int i = 0; i < vecFontNames.size(); i++) {
    fontNames[i] = vecFontNames[i];
  }

  // cleanning ressources
  FcFontSetDestroy(fontSet);
  FcObjectSetDestroy(set);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  return fontNames;
}


// trying with gtk without sucess
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

Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcPatternCreate();
  FcObjectSet *set = FcObjectSetBuild(FC_FAMILY, (char *) 0);
  FcFontSet *fontSet = FcFontList(config, pattern, set);

  std::vector<std::string>vecFontFamilies;
  FcChar8 *family;
  for (int i = 0; fontSet && i < fontSet->nfont; i++) {
    FcPattern *currentPattern = fontSet->fonts[i];
    if (FcPatternGetString(currentPattern, FC_FAMILY, 0, &family) == FcResultMatch) {
      std::string familyStr = reinterpret_cast<const char*>(family); // not sure cast is the best idea, return uchar string ?
      if (std::find(vecFontFamilies.begin(), vecFontFamilies.end(), familyStr) == vecFontFamilies.end())
        vecFontFamilies.push_back(familyStr);
    }
  }
  // fill result
  Napi::Array fontFamilies = Napi::Array::New(env, vecFontFamilies.size());
  for (unsigned int i = 0; i < vecFontFamilies.size(); i++) {
    fontFamilies[i] = vecFontFamilies[i];
  }

  // cleanning ressources
  FcFontSetDestroy(fontSet);
  FcObjectSetDestroy(set);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  return fontFamilies;
}

Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  std::string family = info[0].As<Napi::String>().Utf8Value();

  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pattern = FcPatternCreate();

  FcObjectSet *set = FcObjectSetBuild(FC_FAMILY, FC_WEIGHT, FC_FULLNAME, (char *) 0);
  FcFontSet *fontSet = FcFontList(config, pattern, set);

  std::vector<std::tuple<std::string, std::string, int>> resData;
  for (int i = 0; fontSet && i < fontSet->nfont; i++) {
    FcPattern *currentPattern = fontSet->fonts[i];
    FcChar8 *currentFamily;
    if (FcPatternGetString(currentPattern, FC_FAMILY, 0, &currentFamily) == FcResultMatch) {
      if (std::string(reinterpret_cast<const char*>(currentFamily)).find(family) != std::string::npos) {
        FcChar8 *name;
        int weight = -1;
        std::string strName = "";
        if (FcPatternGetString(currentPattern, FC_FULLNAME, 0, &name) == FcResultMatch) {
          strName = std::string(reinterpret_cast<const char*>(name));
          FcPatternGetInteger(currentPattern, FC_WEIGHT, 0, &weight);
          resData.emplace_back(strName, strName.substr(strName.find(family) + family.length() + 1), weight); // adding one to remove the separator
        }
      }
    }
  }
  Napi::Array members = Napi::Array::New(env, resData.size());
  unsigned int membersIdx = 0;
  for (const auto &fontCarac : resData) {
    Napi::Array member = Napi::Array::New(env, 3);
    member[0u] = std::get<0>(fontCarac);
    member[1u] = std::get<1>(fontCarac);
    member[2u] = std::get<2>(fontCarac);
    members[membersIdx] = Napi::Array(member);
    membersIdx++;
  }
  // cleanning ressources
  FcObjectSetDestroy(set);
  FcFontSetDestroy(fontSet);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  return members;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  // init library
  FcInit();
  exports.Set(
    Napi::String::New(env, "showFontPanel"), Napi::Function::New(env, ShowFontPanel)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFontFamilies"),
    Napi::Function::New(env, GetAvailableFontFamilies)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableFonts"),
    Napi::Function::New(env, GetAvailableFonts)
  );
  exports.Set(
    Napi::String::New(env, "getAvailableMembersOfFontFamily"),
    Napi::Function::New(env, GetAvailableMembersOfFontFamily)
  );

  // FIXME find a way to stop the library with FcFini()
  return exports;
}

NODE_API_MODULE(font_manager, Init)