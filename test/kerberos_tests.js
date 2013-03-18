exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Simple initialize of Kerberos object'] = function(test) {
  var Kerberos = require('../lib/kerberos.js').Kerberos;
  var kerberos = new Kerberos();
  console.dir(kerberos)

  // Initiate kerberos client
  kerberos.authGSSClientInit('mongodb@localhost:27017', Kerberos.GSS_C_MUTUAL_FLAG, function(err, result) {
    console.log("===================================== authGSSClientInit")
    console.dir(err)
    console.dir(result)

    test.done();
  });
}