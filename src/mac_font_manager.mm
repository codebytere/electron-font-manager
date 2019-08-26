#import <AppKit/AppKit.h>
#include "mac_font_manager.h"

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

