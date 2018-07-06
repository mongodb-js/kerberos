{
  'targets': [
    {
      'target_name': 'kerberos',
      'include_dirs': [ '<!(node -e "require(\'nan\')")' ],
      'sources': [
        'src/kerberos.cc',
        'src/kerberos_client.cc',
        'src/kerberos_server.cc'
      ],
      'conditions': [
        ['OS=="mac" or OS=="linux"', {
          'sources': [
            'src/gss/base64.cc',
            'src/gss/kerberos_gss.cc'
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