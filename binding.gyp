{
  'targets': [
    {
      'target_name': 'kerberos',      
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'sources': [ 'lib/kerberos.cc', 'lib/worker.cc', 'lib/kerberosgss.c', 'lib/base64.c', 'lib/kerberos_context.cc' ],
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
        }],
        ['OS=="win"',  {
          'sources': [ 'lib/win32/kerberos.cc', 'lib/win32/kerberos_sspi.c' ],
          "link_settings": {
            "libraries": [
            ]
          }
        }]
      ]
    }
  ]
}