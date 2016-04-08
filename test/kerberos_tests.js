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

    if (!process.env.KRB5_PW_TEST_USERNAME) {
        test.done();
        return;
    }

    kerberos.authUserKrb5Password(process.env.KRB5_PW_TEST_USERNAME, process.env.KRB5_PW_TEST_PASSWORD, process.env.KRB5_PW_TEST_SERVICE, function(err, ok) {
        console.log("err:",err);
        console.log("ok:", ok);
        test.equal(ok, true);
        test.done();
    });
};

//for this test, please set the environment variables shown below.
exports['Negotiate HTTP Client Test'] = function(test) {
    ///// REQUIRED ENVIRONMENT VARIABLES /////
    // give the host and path to a Negotiate protected resource on your network
    var httpHostname = process.env.NEGOTIATE_TEST_HOSTNAME;
    var httpPath = process.env.NEGOTIATE_TEST_PATH;
    ////  OPTIONAL ENVIRONMENT VARIABLES
    // don't use the cache in $KRB5CCNAME, use the one in $NEGOTIATE_TEST_KRB5CCNAME instead
    var krb5CcName = process.env.NEGOTIATE_TEST_KRB5CCNAME || ''; 
    /////

    if (!httpHostname) {
        test.done();
        return;
    }

    var serviceName = "HTTP@"+httpHostname;

    var Kerberos = require('../lib/kerberos.js').Kerberos;
    var kerberos = new Kerberos();
    var http = require('http');

    kerberos.authGSSClientInit(serviceName, 0, krb5CcName, function(err, ctx) {
       console.log("authGSSClientInit error: ",err);
       test.equal(null, err);

       kerberos.authGSSClientStep(ctx, "", function(err) {
           console.log("authGSSClientStep error: ",err);
           test.equal(null, err);

           var cleanupCtx = function() {
               kerberos.authGSSClientClean(ctx, function(err) {
                   console.log("authGSSClientClean error: ",err);
                   test.equal(null, err);
                   test.done(); 
               });
           };

           var negotiateHeader = "Negotiate " + ctx.response;

           var req = http.get({
               hostname: httpHostname,
               path: httpPath,
               headers: {
                   authorization: negotiateHeader
               }
           }, function(res) {
               console.log("http res: ", res.statusCode);
               test.ok(res.statusCode >= 200 && res.statusCode <= 299, "http response status indicates success");

               res.on('data', function(data) {console.log('data:' +data)});
               res.on('end', function() {
                   cleanupCtx();
               });

           });

           req.on('error', function(err) {
              test.ok(false, 'http.get request failed: '+err.message); 
              cleanupCtx();
           });

       }) ;
    });
};
