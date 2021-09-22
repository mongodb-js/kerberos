{
  'targets': [
    {
      'target_name': 'kerberos',
      'type': 'loadable_module',
      'include_dirs': [  "<!(node -p \"require('node-addon-api').include_dir\")" ],
      'sources': [
        'src/kerberos.cc'
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.12'
      },
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      'conditions': [
        ['OS=="mac"', {
            'cflags+': ['-fvisibility=hidden'],
            'xcode_settings': {
              'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
            }
        }],
        ['OS=="mac" or OS=="linux"', {
          'sources': [
            'src/unix/base64.cc',
            'src/unix/kerberos_gss.cc',
            'src/unix/kerberos_unix.cc'
          ],
          'link_settings': {
            'libraries': [
              '-lkrb5',
              '-lgssapi_krb5'
            ]
          },
          'conditions': [
            ['_type=="static_library"', {
              'link_settings': {
                'libraries': [
                  '-lcom_err'
                ]
              }
            }]
          ]
        }],
        ['OS=="win"',  {
          'sources': [
            'src/win32/kerberos_sspi.cc',
            'src/win32/kerberos_win32.cc'
          ],
          'link_settings': {
            'libraries': [
              '-lcrypt32',
              '-lsecur32',
              '-lShlwapi'
            ]
          }
        }]
      ]
    }
  ]
}
