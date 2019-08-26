#include <napi.h>
#import <AppKit/AppKit.h>

/***** HELPERS *****/

NSFontTraitMask ParseTraits(Napi::Array traits) {
  NSFontTraitMask mask = 0;

  int traits_length = static_cast<int>(traits.Length());
  for (int i = 0; i < traits_length; i++) {
    std::string trait = traits.Get(i).As<Napi::String>().Utf8Value();
    if (trait == "bold")
      mask |= NSBoldFontMask;
    if (trait == "compressed")
      mask |= NSCompressedFontMask;
    if (trait == "condensed")
      mask |= NSCondensedFontMask;
    if (trait == "expanded")
      mask |= NSExpandedFontMask;
    if (trait == "fixedPitch")
      mask |= NSFixedPitchFontMask;
    if (trait == "italic")
      mask |= NSItalicFontMask;
    if (trait == "narrow")
      mask |= NSNarrowFontMask;
    if (trait == "nonStandardCharacterSet")
      mask |= NSNonStandardCharacterSetFontMask;
    if (trait == "poster")
      mask |= NSPosterFontMask;
    if (trait == "smallCaps")
      mask |= NSSmallCapsFontMask;
    if (trait == "unbold")
      mask |= NSUnboldFontMask;
    if (trait == "unitalic")
      mask |= NSUnitalicFontMask;
  }

  return mask;
}

/***** EXPORTED FUNCTIONS *****/

Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  NSFontManager *font_manager = [NSFontManager sharedFontManager];
  NSArray *available_fonts = [NSArray new];

  Napi::Object params = info[0].As<Napi::Object>();
  if (params.Has("traits")) {
    Napi::Array traits = params.Get("traits").As<Napi::Array>();
    NSFontTraitMask traitMask = ParseTraits(traits);
    available_fonts = [font_manager availableFontNamesWithTraits:traitMask];
  } else {
    available_fonts = [font_manager availableFonts];
  }

  int num_fonts = [available_fonts count];
  Napi::Array fonts = Napi::Array::New(env, num_fonts);
  for (int i = 0; i < num_fonts; i++) {
    NSString *font = [available_fonts objectAtIndex:i];
    fonts[i] = std::string([font UTF8String]);
  }

  return fonts;
}

Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  NSArray *availableFontFamilies = [[NSFontManager sharedFontManager] availableFontFamilies];
  int num_font_families = [availableFontFamilies count];

  Napi::Array font_families = Napi::Array::New(env, num_font_families);
  for (int i = 0; i < num_font_families; i++) {
    NSString *font_family = [availableFontFamilies objectAtIndex:i];
    font_families[i] = std::string([font_family UTF8String]);
  }

  return font_families;
}

Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  std::string family_utf8 = info[0].As<Napi::String>().Utf8Value();
  NSString* family = [NSString stringWithUTF8String:family_utf8.c_str()];

  NSArray *family_members = [[NSFontManager sharedFontManager] availableMembersOfFontFamily:family];

  int num_members = [family_members count];
  Napi::Array members = Napi::Array::New(env, num_members);

  for (int i = 0; i < num_members; i++) {
    NSArray *family_member = [family_members objectAtIndex:i];

    Napi::Array member = Napi::Array::New(env, 3);
    for (int j = 0; j < 3; j++) {
      if (j == 2)
        member[j] = [[family_member objectAtIndex:j] intValue]; // font weight
      else 
        member[j] = std::string([[family_member objectAtIndex:j] UTF8String]);
    }
    members[i] = member;
  }

  return members;
}

void ShowFontPanel(const Napi::CallbackInfo &info) {
  bool show_styles = info[0].As<Napi::Boolean>().Value();
  NSFontManager *font_manager = [NSFontManager sharedFontManager];

  if (show_styles)
    [font_manager orderFrontStylesPanel:nil];
  else
    [font_manager orderFrontFontPanel:nil];
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
