/**
 * @file FontConfigWrapper.hpp
 * @author Arthur Lang
 * @brief Wrapper containing method for font manipulation with font config
 * https://www.freedesktop.org/wiki/Software/fontconfig/
 * @version 0.1
 * @date 2022-02-18
 */

#include <napi.h>
#include <gtk/gtk.h>
// #include <pango/pango.h>
// #include <pango/pango-context.h>
#include <fontconfig/fontconfig.h>
#include <iostream> // TODO remove this
#include <algorithm>
#include <unordered_map>

using TraitComparator = std::function<bool(FcPattern *)>;

class FontConfigWrapper
{

    public:
        FontConfigWrapper(/* args */);
        ~FontConfigWrapper();

        Napi::Array GetAvailableFontFamilies(const Napi::CallbackInfo &info);
        Napi::Array GetAvailableMembersOfFontFamily(const Napi::CallbackInfo &info);
        Napi::Array GetAvailableFonts(const Napi::CallbackInfo &info);

    private:
        std::vector<TraitComparator> parseTraits(const Napi::Array &traits);

        FcConfig *m_config;
        FcPattern *m_pattern;
        FcObjectSet *m_set;
        FcFontSet *m_fontSet;

        /*! map with conditionnal functions applying to a font trait */
        std::unordered_map<std::string, TraitComparator> _traitsConditions = {
            // what to do with semibolded fonts ?
            {"bold", TraitComparator([](FcPattern * pat) {
                FcChar8* name;
                if (FcPatternGetString(pat, FC_STYLE, 0, &name) == FcResultMatch) {
                    return std::string(reinterpret_cast<const char*>(name)).find("Bold") != std::string::npos;
                } else {
                    return false;
                }
            })},
            {"condensed", TraitComparator([](FcPattern * pat) {
                int value;
                if (FcPatternGetInteger(pat, FC_WIDTH, 0, &value) == FcResultMatch) {
                    return value <= FC_WIDTH_CONDENSED;
                } else {
                    return false;
                }
            })},
            {"expanded", TraitComparator([](FcPattern * pat) {
                int value;
                if (FcPatternGetInteger(pat, FC_WIDTH, 0, &value) == FcResultMatch) {
                    return value >= FC_WIDTH_EXPANDED;
                } else {
                    return false;
                }
            })},
            {"italic", TraitComparator([](FcPattern * pat) {
                FcChar8* name;
                if (FcPatternGetString(pat, FC_STYLE, 0, &name) == FcResultMatch) {
                    return std::string(reinterpret_cast<const char*>(name)).find("Italic") != std::string::npos;
                } else {
                    return false;
                }
            })},
            {"unbold", TraitComparator([](FcPattern * pat) {
                FcChar8* name;
                if (FcPatternGetString(pat, FC_STYLE, 0, &name) == FcResultMatch) {
                    std::cout << std::string(reinterpret_cast<const char*>(name)) << std::endl;
                    return std::string(reinterpret_cast<const char*>(name)).find("Bold") == std::string::npos;
                } else {
                    return false;
                }
            })},
            {"unitalic", TraitComparator([](FcPattern * pat) {
                FcChar8* name;
                if (FcPatternGetString(pat, FC_STYLE, 0, &name) == FcResultMatch) {
                    std::cout << std::string(reinterpret_cast<const char*>(name)) << std::endl;
                    return std::string(reinterpret_cast<const char*>(name)).find("Italic") == std::string::npos;
                } else {
                    return false;
                }
            })},
        };

};
