{
  'targets': [
    {
      'target_name': 'kerberos',
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'include_dirs': [ '<!(node -e "require(\'nan\')")' ],
      'conditions': [
        ['OS=="mac" or OS=="linux"', {
          'sources': [
            'src/kerberos.cc'
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
        }]
      ]
    }
  ]
}