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

Napi::Array BuildTraits(Napi::Env env, NSFontTraitMask mask) {
    std::vector<std::string> traits;
    if ((NSBoldFontMask & mask) != 0)
        traits.push_back("bold");
    if ((NSCondensedFontMask & mask) != 0)
        traits.push_back("condensed");
    if ((NSExpandedFontMask & mask) != 0)
        traits.push_back("expanded");
    if ((NSFixedPitchFontMask & mask) != 0)
        traits.push_back("fixedPitch");
    if ((NSItalicFontMask & mask) != 0)
        traits.push_back("italic");
    if ((NSNarrowFontMask & mask) != 0)
        traits.push_back("narrow");
    if ((NSNonStandardCharacterSetFontMask & mask) != 0)
        traits.push_back("nonStandardCharacterSet");
    if ((NSPosterFontMask & mask) != 0)
        traits.push_back("poster");
    if ((NSSmallCapsFontMask & mask) != 0)
        traits.push_back("smallCaps");
    if ((NSUnboldFontMask & mask) != 0)
        traits.push_back("unbold");
    if ((NSUnitalicFontMask & mask) != 0)
        traits.push_back("unitalic");
    Napi::Array t = Napi::Array::New(env, traits.size());
    for (size_t i = 0; i < traits.size(); i++)
        t[i] = traits[i];

    return t;
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

@interface FontPanelTarget: NSObject
{
}

@property (assign) Napi::FunctionReference* emit;
@property (retain) NSFont* font;

@end

@implementation FontPanelTarget

- (nullable instancetype)initWithEmitter:(Napi::Function*)emit
{
    self = [super init];
    if (self != nil)
    {
        self.emit = new Napi::FunctionReference;
        *self.emit = Napi::Persistent(*emit);
        self.font = [NSFont boldSystemFontOfSize:12];
    }

    return self;
}

- (void)dealloc
{
    if (self.emit) {
        Napi::Env env = self.emit->Env();
        Napi::HandleScope scope(env);
        delete self.emit;
    }
    [super dealloc];
}

- (void)changeFont:(id)sender
{
    self.font = [sender convertFont:self.font];

    Napi::Env env = self.emit->Env();
    Napi::HandleScope scope(env);
    Napi::Object obj = Napi::Object::New(env);
    obj.Set(Napi::String::New(env, "family"), Napi::String::New(env, [self.font.familyName UTF8String]));
    obj.Set(Napi::String::New(env, "name"), Napi::String::New(env, [self.font.fontName UTF8String]));
    obj.Set(Napi::String::New(env, "pointSize"), Napi::Number::New(env, self.font.pointSize));
    NSFontTraitMask mask = self.font.fontDescriptor.symbolicTraits;
    obj.Set(Napi::String::New(env, "traits"), BuildTraits(env, mask));
    self.emit->Call({Napi::String::New(env, "fontSelected"), obj});
}

@end

FontPanelTarget* _target = nil;

Napi::Value ShowFontPanel(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  Napi::Function emit(info[0].As<Napi::Function>());
  Napi::Object options = info[1].As<Napi::Object>();
  bool show_styles = options.Get("showStyle").As<Napi::Boolean>().Value();
  std::string title;
  Napi::Value titleValue = options.Get("title");
  if (titleValue.IsString())
      title = titleValue.As<Napi::String>().Utf8Value();
  NSFontManager* font_manager = [NSFontManager sharedFontManager];
  FontPanelTarget* target = [[FontPanelTarget alloc] initWithEmitter:&emit];
  _target = target;
  [font_manager setTarget:target];
  [font_manager setDelegate:target];

  NSWindow *window = [[NSApplication sharedApplication] keyWindow];
  if (show_styles)
    [font_manager orderFrontStylesPanel:window];
  else
    [font_manager orderFrontFontPanel:window];

  return deferred.Promise();
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
