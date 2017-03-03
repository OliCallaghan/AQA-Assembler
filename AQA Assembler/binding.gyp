{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "main.cpp" ],
      'xcode_settings': {
        'OTHER_CFLAGS': [
          "-std=c++11",
          "-stdlib=libc++"
        ],
      }
    }
  ]
}
