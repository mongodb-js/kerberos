'use strict';

const expect = require('chai').expect;
const http = require('http');
const Kerberos = require('..').Kerberos;

describe('Kerberos', function() {
  it('simple initialize of Kerberos object', function(done) {
    const kerberos = new Kerberos();

    // Initiate kerberos client
    kerberos.authGSSClientInit('mongodb@kdc.10gen.me', Kerberos.GSS_C_MUTUAL_FLAG, function(
      err,
      context
    ) {
      expect(err).to.not.exist;
      expect(context).to.exist;

      // Perform the first step
      kerberos.authGSSClientStep(context, function(/* err, result */) {
        // TODO: reenable next lines when osx support is complete
        // expect(err).to.not.exist;
        // expect(result).to.exist;

        done();
      });
    });
  });

  // for this test, please set the environment variables shown below.
  it('simple username password test', function(done) {
    const kerberos = new Kerberos();

    if (!process.env.KRB5_PW_TEST_USERNAME) {
      return done();
    }

    kerberos.authUserKrb5Password(
      process.env.KRB5_PW_TEST_USERNAME,
      process.env.KRB5_PW_TEST_PASSWORD,
      process.env.KRB5_PW_TEST_SERVICE,
      function(err, ok) {
        expect(err).to.not.exist;
        expect(ok).to.be.true;
        done();
      }
    );
  });

  //for this test, please set the environment variables shown below.
  it('negotiate HTTP Client Test', function(done) {
    ///// REQUIRED ENVIRONMENT VARIABLES /////
    // give the host and path to a Negotiate protected resource on your network
    const httpHostname = process.env.NEGOTIATE_TEST_HOSTNAME;
    const httpPath = process.env.NEGOTIATE_TEST_PATH;
    ////  OPTIONAL ENVIRONMENT VARIABLES
    // don't use the cache in $KRB5CCNAME, use the one in $NEGOTIATE_TEST_KRB5CCNAME instead
    const krb5CcName = process.env.NEGOTIATE_TEST_KRB5CCNAME || '';
    /////

    if (!httpHostname) {
      return done();
    }

    const serviceName = 'HTTP@' + httpHostname;
    const kerberos = new Kerberos();

    kerberos.authGSSClientInit(serviceName, 0, krb5CcName, function(err, ctx) {
      expect(err).to.not.exist;

      kerberos.authGSSClientStep(ctx, '', function(err) {
        expect(err).to.not.exist;

        const cleanupCtx = function() {
          kerberos.authGSSClientClean(ctx, function(err) {
            expect(err).to.not.exist;
            done();
          });
        };

        const negotiateHeader = 'Negotiate ' + ctx.response;

        const req = http.get(
          {
            hostname: httpHostname,
            path: httpPath,
            headers: {
              authorization: negotiateHeader
            }
          },
          function(res) {
            expect(res.statusCode).to.be.at.least(200);
            expect(res.statusCode).to.be.at.most(299);

            res.on('data', data => console.log(` >> ${data}`));
            res.on('end', () => cleanupCtx());
          }
        );

        req.on('error', function(err) {
          done(`http.get request failed: ${err.message}`);
          cleanupCtx();
        });
      });
    });
  });
});
