{
  'targets': [
    {
      'target_name': 'kerberos',
      'type': 'loadable_module',
      'include_dirs': [
        "<!(node -p \"require('node-addon-api').include_dir\")"
      ],
      'sources': [
        'src/kerberos.cc'
      ],
      'variables': {
        'ARCH': '<(host_arch)',
        'kerberos_use_rtld%': 'true'
      },
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.12',
        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
      },
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'msvs_configuration_attributes': {
        'SpectreMitigation': 'Spectre'
      },
      'msvs_settings': {
        'VCCLCompilerTool': {
          'ExceptionHandling': 1,
          'AdditionalOptions': [
            '/guard:cf',
            '/w34244',
            '/w34267',
            '/ZH:SHA_256'
          ]
        },
        'VCLinkerTool': {
          'AdditionalOptions': [
            '/guard:cf'
          ]
        }
      },
      'conditions': [
        ['OS=="mac"', { 'cflags+': ['-fvisibility=hidden'] }],
        ['_type!="static_library" and ARCH=="arm64"', {
          'xcode_settings': {
            "OTHER_CFLAGS": [
              "-arch x86_64",
              "-arch arm64"
            ],
            "OTHER_LDFLAGS": [
              "-arch x86_64",
              "-arch arm64"
            ]
          }
        }],
        ['OS=="mac" or OS=="linux"', {
          'sources': [
            'src/unix/base64.cc',
            'src/unix/kerberos_gss.cc',
            'src/unix/kerberos_unix.cc'
          ]
        }],
        ['(OS=="mac") or (OS=="linux" and kerberos_use_rtld!="true")', {
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
        ['(OS=="linux") and (kerberos_use_rtld=="true")', {
          'defines': ['KERBEROS_USE_RTLD=1'],
          'link_settings': {
            'libraries': [
              '-ldl',
            ]
          },
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
