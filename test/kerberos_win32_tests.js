'use strict';
const kerberos = require('..');
const MongoClient = require('mongodb').MongoClient;
const expect = require('chai').expect;

/*
_HOST = os.environ.get('MONGODB_HOST', 'localhost')
_PORT = int(os.environ.get('MONGODB_PORT', 27017))
_SPN = os.environ.get('KERBEROS_SERVICE')
_PRINCIPAL = os.environ.get('KERBEROS_PRINCIPAL')
_UPN = os.environ.get('KERBEROS_UPN')
_USER = os.environ.get('KERBEROS_USER')
_DOMAIN = os.environ.get('KERBEROS_DOMAIN')
_PASSWORD = os.environ.get('KERBEROS_PASSWORD')
*/

const service = 'mongodb';
const username = 'drivers';
const password = 'powerbook17';
const realm = 'LDAPTEST.10GEN.CC';
const hostname = 'ldaptest.10gen.cc';
const port = '27017';
const upn = `${username}@${realm}`;

// _HOST = os.environ.get('MONGODB_HOST', 'localhost')
// _PORT = int(os.environ.get('MONGODB_PORT', 27017))
// _SPN = os.environ.get('KERBEROS_SERVICE')
// _PRINCIPAL = os.environ.get('KERBEROS_PRINCIPAL')
// _UPN = os.environ.get('KERBEROS_UPN')
// _USER = os.environ.get('KERBEROS_USER')
// _DOMAIN = os.environ.get('KERBEROS_DOMAIN')
// _PASSWORD = os.environ.get('KERBEROS_PASSWORD')

describe('Kerberos (win32)', function() {
  it('should work from windows', function(done) {
    function authenticate(db, krbClient, response, callback) {
      krbClient.step(response ? response : '', (err, response) => {
        if (err) return callback(err, null);
        db.command({ saslStart: 1, mechanism: 'GSSAPI', payload: response }, (err, res) => {
          if (err) return callback(err, null);
          if (client.contextComplete) {
            callback(null, res);
          } else {
            authenticate(db, krbClient, res, callback);
          }
        });
      });
    }

    MongoClient.connect(`mongodb://${hostname}:${port}/`, (err, client) => {
      expect(err).to.not.exist;

      const db = client.db('$external');

      kerberos.initializeClient(service, { user: username, domain: realm, password }, (err, krbClient) => {
        expect(err).to.not.exist;

        authenticate(db, krbClient, null, (err, response) => {
          krbClient.unwrap(response.payload, (err, unwrapped) => {
            expect(err).to.not.exist;

            // RFC-4752
            const challengeBytes = Buffer.from(unwrapped, 'base64');
            expect(challengeBytes).to.have.length(4);

            // Manually create an authorization message and encrypt it. This
            // is the "no security layer" message as detailed in RFC-4752,
            // section 3.1, final paragraph. This is also the message created
            // by calling authGSSClientWrap with the "user" option.
            const msg = Buffer.from(`\x01\x00\x00\x00${upn}`).toString('base64');
            krbClient.wrap(msg, (err, custom) => {
              expect(err).to.not.exist;

              // Wrap using unwrapped and user principal
              krbClient.wrap(unwrapped, { user: upn }, (err, wrapped) => {
                expect(err).to.not.exist;

                client.command({ saslContinue: 1, conversationId: response.conversationId, payload: custom }, err => {
                  expect(err).to.not.exist;
                  expect(client.username).to.exit;
                  done();
                });
              });
            });
          });
        });
      });
    });

    /*
    def test_authenticate(self):
        res, ctx = kerberos.authGSSClientInit(
            _SPN,
            None,
            kerberos.GSS_C_MUTUAL_FLAG,
            _USER,
            _DOMAIN,
            _PASSWORD)
        self.assertEqual(res, kerberos.AUTH_GSS_COMPLETE)

        res = kerberos.authGSSClientStep(ctx, "")
        self.assertEqual(res, kerberos.AUTH_GSS_CONTINUE)

        payload = kerberos.authGSSClientResponse(ctx)
        self.assertIsInstance(payload, str)

        response = self.db.command(
            'saslStart', mechanism='GSSAPI', payload=payload)
        while res == kerberos.AUTH_GSS_CONTINUE:
            res = kerberos.authGSSClientStep(ctx, response['payload'])
            payload = kerberos.authGSSClientResponse(ctx) or ''
            response = self.db.command(
               'saslContinue',
               conversationId=response['conversationId'],
               payload=payload)

        res = kerberos.authGSSClientUnwrap(ctx, response['payload'])
        self.assertEqual(res, 1)

        unwrapped = kerberos.authGSSClientResponse(ctx)
        self.assertIsInstance(unwrapped, str)
        self.assertIsInstance(kerberos.authGSSClientResponseConf(ctx), int)

        # RFC-4752
        challenge_bytes = base64.standard_b64decode(unwrapped)
        self.assertEqual(4, len(challenge_bytes))

        # Manually create an authorization message and encrypt it. This
        # is the "no security layer" message as detailed in RFC-4752,
        # section 3.1, final paragraph. This is also the message created
        # by calling authGSSClientWrap with the "user" option.
        msg = base64.standard_b64encode(
            b"\x01\x00\x00\x00" + _UPN.encode("utf8")).decode("utf8")
        res = kerberos.authGSSClientWrap(ctx, msg)
        self.assertEqual(res, 1)

        custom = kerberos.authGSSClientResponse(ctx)
        self.assertIsInstance(custom, str)

        # Wrap using unwrapped and user principal.
        res = kerberos.authGSSClientWrap(ctx, unwrapped, _UPN)
        self.assertEqual(res, 1)

        wrapped = kerberos.authGSSClientResponse(ctx)
        self.assertIsInstance(wrapped, str)

        # Actually complete authentication, using our custom message.
        response = self.db.command(
           'saslContinue',
           conversationId=response['conversationId'],
           payload=custom)
        self.assertTrue(response['done'])

        self.assertIsInstance(kerberos.authGSSClientUsername(ctx), str)
    */
  });
});
