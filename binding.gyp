{
  'targets': [
    {
      'target_name': 'kerberos',
      'include_dirs': [ '<!(node -e "require(\'nan\')")' ],
      'sources': [
        'src/kerberos.cc'
      ],
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.12',
        'OTHER_CFLAGS': [
          "-std=c++11",
          "-stdlib=libc++"
        ],
      },
      'conditions': [
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
          }
        }],
        ['OS=="win"',  {
          'sources': [
            'src/win32/kerberos_sspi.cc',
            'src/win32/kerberos_win32.cc'
          ],
          'link_settings': {
            'libraries': [
              'crypt32.lib',
              'secur32.lib',
              'Shlwapi.lib'
            ]
          }
        }]
      ]
    }
  ]
}