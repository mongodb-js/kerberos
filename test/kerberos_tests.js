exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Simple initialize of Kerberos object'] = function(test) {
  var Kerberos = require('../lib/kerberos.js').Kerberos;
  var kerberos = new Kerberos();
  // console.dir(kerberos)

  // Initiate kerberos client
  kerberos.authGSSClientInit('mongodb@kdc.10gen.me', Kerberos.GSS_C_MUTUAL_FLAG, function(err, context) {
    console.log("===================================== authGSSClientInit")
    test.equal(null, err);
    test.ok(context != null && typeof context == 'object');
    // console.log("===================================== authGSSClientInit")
    console.dir(err)
    console.dir(context)
    // console.dir(typeof result)

    // Perform the first step
    kerberos.authGSSClientStep(context, function(err, result) {
      console.log("===================================== authGSSClientStep")
      console.dir(err)
      console.dir(result)
      console.dir(context)

      test.done();
    });
  });
}

// for this test, please set the environment variables shown below.
exports['Simple username password test'] = function(test) {
    var Kerberos = require('../lib/kerberos.js').Kerberos;
    var kerberos = new Kerberos();
    kerberos.authUserKrb5Password(process.env.KRB5_PW_TEST_USERNAME, process.env.KRB5_PW_TEST_PASSWORD, process.env.KRB5_PW_TEST_SERVICE, function(err, ok) {
        console.log("err:",err);
        console.log("ok:", ok);
        test.equal(ok, true);
        test.done();
    });
};
