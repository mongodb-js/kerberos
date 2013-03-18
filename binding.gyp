{
  'targets': [
    {
      'target_name': 'kerberos',
      'sources': [ 'lib/kerberos.cc' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'defines': [
            '__MACOSX_CORE__'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          },
          "link_settings": {
            "libraries": [
              "-lkrb5"
            ]
          }
        }]
      ]
    }
  ]
}