exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Simple initialize of Kerberos win32 object'] = function(test) {
  var KerberosNative = require('../build/Release/kerberos').Kerberos;
  console.dir(KerberosNative)
  new KerberosNative();
  test.done();
}
