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
        "include_dirs": [
          "`pkg-config --cflags gtk+-3.0`" # locate include dirs
        ],
        "libraries": [
          "-pthread -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0"
        ]
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