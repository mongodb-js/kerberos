{
  'targets': [
    {
      'target_name': 'kerberos',
      "cflags": [
        '-std=c++11',
      ],
      # 'cflags!': [ '-fno-exceptions', '-std=c++11' ],
      # 'cflags_cc!': [ '-fno-exceptions' ],
      'include_dirs': [ '<!(node -e "require(\'nan\')")' ],
      'conditions': [
        ['OS=="mac" or OS=="linux"', {
          'sources': [
            'src/base64.c',
            'src/kerberos.cc',
            'src/kerberos_client.cc',
            'src/kerberos_server.cc',
            'src/kerberos_gss.cc'
          ],
          'link_settings': {
            'libraries': [
              '-lkrb5',
              '-lgssapi_krb5'
            ]
          }
        }],
        ['OS=="win"',  {
          'sources': [
            'src/kerberos.cc'
          ]
        }],
        ['OS=="mac"', {
          'cflags!': [ '-fno-exceptions' ],
          'cflags_cc!': [ '-fno-exceptions' ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_RTTI': 'YES',
            'OTHER_CPLUSPLUSFLAGS' : [ '-std=c++11', '-stdlib=libc++' ],
            'OTHER_LDFLAGS': [ '-stdlib=libc++' ],
            'MACOSX_DEPLOYMENT_TARGET': "10.7"
          }
        }]
      ]
    }
  ]
}