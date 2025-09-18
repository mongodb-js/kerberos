'use strict';

const { promisify } = require('util');
const { loadBindings } = require('./util');

const kerberos = loadBindings();
const KerberosClient = kerberos.KerberosClient;
const KerberosServer = kerberos.KerberosServer;

// GSS Flags
/** @kind constant */
const GSS_C_DELEG_FLAG = 1;
/** @kind constant */
const GSS_C_MUTUAL_FLAG = 2;
/** @kind constant */
const GSS_C_REPLAY_FLAG = 4;
/** @kind constant */
const GSS_C_SEQUENCE_FLAG = 8;
/** @kind constant */
const GSS_C_CONF_FLAG = 16;
/** @kind constant */
const GSS_C_INTEG_FLAG = 32;
/** @kind constant */
const GSS_C_ANON_FLAG = 64;
/** @kind constant */
const GSS_C_PROT_READY_FLAG = 128;
/** @kind constant */
const GSS_C_TRANS_FLAG = 256;

// GSS_OID
/** @kind constant */
const GSS_C_NO_OID = 0;
/** @kind constant */
const GSS_MECH_OID_KRB5 = 9;
/** @kind constant */
const GSS_MECH_OID_SPNEGO = 6;

/** @kind constant */
const version = require('../package.json').version;

/**
 * @class KerberosClient
 *
 * @property {string} username The username used for authentication
 * @property {string} response The last response received during authentication steps
 * @property {string} responseConf Indicates whether confidentiality was applied or not (GSSAPI only)
 * @property {boolean} contextComplete Indicates that authentication has successfully completed or not
 */

const promisifiedStep = promisify(KerberosClient.prototype.step);
/**
 * Processes a single kerberos client-side step using the supplied server challenge.
 *
 * @kind function
 * @memberof KerberosClient
 * @param {string} challenge A string containing the base64-encoded server data (which may be empty for the first step)
 * @return {Promise<string>}
 */
KerberosClient.prototype.step = async function step(challenge) {
  if (typeof challenge !== 'string') {
    throw new TypeError('parameter `challenge` must be a string.');
  }
  return await promisifiedStep.call(this, challenge);
};

const promsifiedWrap = promisify(KerberosClient.prototype.wrap);
/**
 * Perform the client side kerberos wrap step.
 *
 * @kind function
 * @memberof KerberosClient
 * @param {string} challenge The response returned after calling `unwrap`
 * @param {object} [options] Options
 * @param {string} [options.user] The user to authorize
 * @param {boolean} [options.protect] Indicates if the wrap should request message confidentiality
 * @return {Promise<string>}
 */
KerberosClient.prototype.wrap = async function wrap(challenge, options = {}) {
  if (typeof challenge !== 'string') {
    throw new TypeError('parameter `challenge` must be a string.');
  }

  return await promsifiedWrap.call(this, challenge, options);
};

const promisifiedUnwrap = promisify(KerberosClient.prototype.unwrap);
/**
 * Perform the client side kerberos unwrap step
 *
 * @kind function
 * @memberof KerberosClient
 * @param {string} challenge A string containing the base64-encoded server data
 * @return {Promise<string>}
 */
KerberosClient.prototype.unwrap = async function unwrap(challenge) {
  if (typeof challenge !== 'string') {
    throw new TypeError('parameter `challenge` must be a string.');
  }
  return await promisifiedUnwrap.call(this, challenge);
};

const promisifiedServerStep = promisify(KerberosServer.prototype.step);
/**
 * @class KerberosServer
 *
 * @property {string} username The username used for authentication
 * @property {string} response The last response received during authentication steps
 * @property {string} targetName The target used for authentication
 * @property {boolean} contextComplete Indicates that authentication has successfully completed or not
 */

/**
 * Processes a single kerberos server-side step using the supplied client data.
 *
 * @kind function
 * @memberof KerberosServer
 * @param {string} challenge A string containing the base64-encoded client data
 * @return {Promise<KerberosServer>}
 */
KerberosServer.prototype.step = async function step(challenge) {
  if (typeof challenge !== 'string') {
    throw new TypeError('parameter `challenge` must be a string.');
  }
  return await promisifiedServerStep.call(this, challenge);
};

const promisifiedCheckPassword = promisify(kerberos.checkPassword);

/**
 * This function provides a simple way to verify that a user name and password
 * match those normally used for Kerberos authentication.
 * It does this by checking that the supplied user name and password can be
 * used to get a ticket for the supplied service.
 * If the user name does not contain a realm, then the default realm supplied
 * is used.
 *
 * For this to work properly the Kerberos must be configured properly on this
 * machine.
 * That will likely mean ensuring that the edu.mit.Kerberos preference file
 * has the correct realms and KDCs listed.
 *
 * IMPORTANT: This method is vulnerable to KDC spoofing attacks and it should
 * only be used for testing. Do not use this in any production system - your
 * security could be compromised if you do.
 *
 * @kind function
 * @param {string} username The Kerberos user name. If no realm is supplied, then the `defaultRealm` will be used.
 * @param {string} password The password for the user.
 * @param {string} service The Kerberos service to check access for.
 * @param {string} [defaultRealm] The default realm to use if one is not supplied in the user argument.
 * @return {Promise<null>} returns Promise that rejects if the password is invalid
 */
async function checkPassword(username, password, service, defaultRealm) {
  if (typeof username !== 'string') {
    throw new TypeError('parameter `username` must be a string.');
  }
  if (typeof password !== 'string') {
    throw new TypeError('parameter `password` must be a string.');
  }
  if (typeof service !== 'string') {
    throw new TypeError('parameter `service` must be a string.');
  }
  if (defaultRealm && typeof defaultRealm !== 'string') {
    throw new TypeError('if specified, parameter `defaultRealm` must be a string.');
  }
  return await promisifiedCheckPassword.call(this, username, password, service, defaultRealm);
}

const promisifiedPrincipalDetails = promisify(kerberos.principalDetails);
/**
 * This function returns the service principal for the server given a service type and hostname.
 *
 * Details are looked up via the `/etc/keytab` file.
 *
 * @kind function
 * @param {string} service The Kerberos service type for the server.
 * @param {string} hostname The hostname of the server.
 * @return {Promise} returns Promise
 */
async function principalDetails(service, hostname) {
  if (typeof service !== 'string') {
    throw new TypeError('parameter `service` must be a string.');
  }
  if (typeof hostname !== 'string') {
    throw new TypeError('parameter `hostname` must be a string.');
  }
  return await promisifiedPrincipalDetails.call(this, service, hostname);
}

const promisifiedInitializeClient = promisify(kerberos.initializeClient);
/**
 * Initializes a context for client-side authentication with the given service principal.
 *
 * @kind function
 * @param {string} service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 * @param {object} [options] Optional settings
 * @param {string} [options.principal] Optional string containing the client principal in the form 'user@realm' (e.g. 'jdoe@example.com').
 * @param {number} [options.flags] Optional integer used to set GSS flags. (e.g.  `GSS_C_DELEG_FLAG\|GSS_C_MUTUAL_FLAG\|GSS_C_SEQUENCE_FLAG` will allow for forwarding credentials to the remote host)
 * @param {number} [options.mechOID] Optional GSS mech OID. Defaults to None (GSS_C_NO_OID). Other possible values are `GSS_MECH_OID_KRB5`, `GSS_MECH_OID_SPNEGO`.
 * @param {string} [options.user] The username with which to authenticate.  Only used on Windows.
 * @param {string} [options.pass] The password with which to authenticate.  Only used on Windows.
 * @return {Promise<KerberosClient>} returns Promise
 */
async function initializeClient(service, options = { mechOID: GSS_C_NO_OID }) {
  if (typeof service !== 'string') {
    throw new TypeError('parameter `service` must be a string.');
  }
  return await promisifiedInitializeClient.call(this, service, options);
}

const promisifiedInitializeServer = promisify(kerberos.initializeServer);

/**
 * Initializes a context for server-side authentication with the given service principal.
 *
 * @kind function
 * @param {string} service A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com').
 * @return {Promise<KerberosServer>} returns Promise
 */
async function initializeServer(service) {
  if (typeof service !== 'string') {
    throw new TypeError('parameter `service` must be a string.');
  }
  return await promisifiedInitializeServer.call(this, service);
}

/**
 * @private
 *
 * The NAPI version kerberos is configured to use, defined in kerberos.h.
 */
const definedNapiVersion = kerberos.definedNapiVersion;

module.exports = {
  initializeClient,
  initializeServer,
  principalDetails,
  checkPassword,

  // gss flags
  GSS_C_DELEG_FLAG,
  GSS_C_MUTUAL_FLAG,
  GSS_C_REPLAY_FLAG,
  GSS_C_SEQUENCE_FLAG,
  GSS_C_CONF_FLAG,
  GSS_C_INTEG_FLAG,
  GSS_C_ANON_FLAG,
  GSS_C_PROT_READY_FLAG,
  GSS_C_TRANS_FLAG,
  GSS_C_NO_OID,

  // mechanism OIDs
  GSS_MECH_OID_KRB5,
  GSS_MECH_OID_SPNEGO,

  version,
  definedNapiVersion
};
