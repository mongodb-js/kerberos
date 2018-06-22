'use strict';

const KerberosNative = require('..').Kerberos;
const maybeDescribe = !/^win/.test(process.platform) ? describe.skip : describe;

maybeDescribe('Kerberos (Win32)', function() {
  it('simple initialize of Kerberos win32 object', function(done) {
    console.log(/^win/.test(process.platform));
    console.log('HERE THEN');
    var kerberos = new KerberosNative();
    console.log('=========================================== 0');
    console.dir(kerberos.acquireAlternateCredentials('dev1@10GEN.ME', 'a'));
    console.log('=========================================== 1');
    console.dir(kerberos.prepareOutboundPackage('mongodb/kdc.10gen.com'));
    console.log('=========================================== 2');
    done();
  });
});
