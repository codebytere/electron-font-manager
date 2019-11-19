{
  "targets": [{
    "target_name": "font_manager",
    "sources": [ ],
    "conditions": [
      ['OS=="mac"', {
        "sources": [
          "src/mac_font_manager.mm"
        ],
      }],
      ['OS=="linux"', {
        "sources": [
          "src/gtk_font_manager.cc"
        ],
      }],
      ['OS=="win"', {
        "sources": [
          "src/win_font_manager.cc"
        ],
      }]
    ],
    'include_dirs': [
      "<!@(node -p \"require('node-addon-api').include\")"
    ],
    'libraries': [],
    'dependencies': [
      "<!(node -p \"require('node-addon-api').gyp\")"
    ],
    'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    "xcode_settings": {
      "OTHER_CPLUSPLUSFLAGS": ["-std=c++14", "-stdlib=libc++", "-mmacosx-version-min=10.8"],
      "OTHER_LDFLAGS": ["-framework CoreFoundation -framework AppKit -framework Contacts"]
    }
  }]
}