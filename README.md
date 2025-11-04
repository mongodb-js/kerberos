Kerberos
========
The `kerberos` package is a C++ extension for Node.js that provides cross-platform support for kerberos authentication
using GSSAPI on linux/osx, and SSPI on windows. Much of the code in this module is adapted from
[ccs-kerberos](https://github.com/apple/ccs-pykerberos) and [winkerberos](https://github.com/mongodb-labs/winkerberos).

### Requirements

**Linux**
- `python` v2.7
- `make`
- A proper C/C++ compiler toolchain, like [GCC](https://gcc.gnu.org/)
- Distribution-specific kerberos packages (e.g. `krb5-dev` on Ubuntu)

**macOS**
- `Xcode Command Line Tools`: Can be installed with `xcode-select --install`
- Distribution-specific kerberos packages (e.g. `krb5` on Homebrew)

**Windows**
- **Option 1:** Install all the required tools and configurations using Microsoft's
[windows-build-tools](https://github.com/felixrieseberg/windows-build-tools) by running `npm install -g
windows-build-tools` from an elevated PowerShell (run as Administrator).
- **Option 2:** Install dependencies and configuration manually
1. Visual C++ Build Environment:
* **Option 1:** Install [Visual C++ Build Tools](http://go.microsoft.com/fwlink/?LinkId=691126) using the *Default
Install* option.
* **Option 2:** Install [Visual Studio 2015](https://www.visualstudio.com/products/visual-studio-community-vs) (or
modify an existing installation) and select *Common Tools for Visual C++* during setup.

> :bulb: [Windows Vista / 7 only] requires [.NET Framework
4.5.1](http://www.microsoft.com/en-us/download/details.aspx?id=40773)

2. Install [Python 2.7](https://www.python.org/downloads/) or [Miniconda 2.7](http://conda.pydata.org/miniconda.html)
(`v3.x.x` is not supported), and run `npm config set python python2.7`
3. Launch cmd, `npm config set msvs_version 2015`

### MongoDB Node.js Driver Version Compatibility

Only the following version combinations with the [MongoDB Node.js
Driver](https://github.com/mongodb/node-mongodb-native) are considered stable.

|               | `kerberos@1.x` | `kerberos@2.x` | `kerberos@7.x` |
| ------------- | -------------- | -------------- | -------------- |
| `mongodb@7.x` | N/A            | N/A            | ✓              |
| `mongodb@6.x` | N/A            | ✓              | N/A            |
| `mongodb@5.x` | ✓              | ✓              | N/A            |
| `mongodb@4.x` | ✓              | ✓              | N/A            |
| `mongodb@3.x` | ✓              | N/A            | N/A            |

### Installation

Now you can install `kerberos` with the following:

```bash
npm install kerberos
```

#### Prebuild Platforms

Below are the platforms that are available as prebuilds on each github release.
`prebuild-install` downloads these automatically depending on the platform you are running npm install on.

- Linux GLIBC 2.28 or later
- s390x
- arm64
- x64
- MacOS universal binary
- x64
- arm64
- Windows
- x64

### Release Integrity

Releases are created automatically and signed using the [Node team's GPG key](https://pgp.mongodb.com/node-driver.asc).
This applies to the git tag as well as all release packages provided as part of a GitHub release. To verify the provided
packages, download the key and import it using gpg:

```
gpg --import node-driver.asc
```

The GitHub release contains a detached signature file for the NPM package (named
`kerberos-X.Y.Z.tgz.sig`).

The following command returns the link npm package.
```shell
npm view kerberos@vX.Y.Z dist.tarball
```

Using the result of the above command, a `curl` command can return the official npm package for the release.

To verify the integrity of the downloaded package, run the following command:
```shell
gpg --verify kerberos-X.Y.Z.tgz.sig kerberos-X.Y.Z.tgz
```

>[!Note]
No verification is done when using npm to install the package. To ensure release integrity when using npm, download the
tarball manually from the GitHub release, verify the signature, then install the package from the downloaded tarball
using npm install mongodb-X.Y.Z.tgz.

To verify the native `.node` packages, follow the same steps as above.

### Testing

Run the test suite using:

```bash
docker run -i -v PATH_TO_KERBEROS_REPO:/app -w /app -e PROJECT_DIRECTORY=/app ubuntu:20.04 /bin/bash
/app/.evergreen/run-tests-ubuntu.sh
```

NOTE: The test suite requires an active kerberos deployment.

# Documentation

## Classes

<dl>
<dt><a href="#KerberosClient">KerberosClient</a></dt>
<dd></dd>
<dt><a href="#KerberosServer">KerberosServer</a></dt>
<dd></dd>
</dl>

## Constants

<dl>
<dt><a href="#GSS_C_DELEG_FLAG">GSS_C_DELEG_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_MUTUAL_FLAG">GSS_C_MUTUAL_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_REPLAY_FLAG">GSS_C_REPLAY_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_SEQUENCE_FLAG">GSS_C_SEQUENCE_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_CONF_FLAG">GSS_C_CONF_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_INTEG_FLAG">GSS_C_INTEG_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_ANON_FLAG">GSS_C_ANON_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_PROT_READY_FLAG">GSS_C_PROT_READY_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_TRANS_FLAG">GSS_C_TRANS_FLAG</a></dt>
<dd></dd>
<dt><a href="#GSS_C_NO_OID">GSS_C_NO_OID</a></dt>
<dd></dd>
<dt><a href="#GSS_MECH_OID_KRB5">GSS_MECH_OID_KRB5</a></dt>
<dd></dd>
<dt><a href="#GSS_MECH_OID_SPNEGO">GSS_MECH_OID_SPNEGO</a></dt>
<dd></dd>
<dt><a href="#version">version</a></dt>
<dd></dd>
</dl>

## Functions

<dl>
<dt><a href="#checkPassword">checkPassword(username, password, service, [defaultRealm])</a> ⇒ <code>Promise.&lt;null&gt;</code></dt>
<dd><p>This function provides a simple way to verify that a user name and password
match those normally used for Kerberos authentication.
It does this by checking that the supplied user name and password can be
used to get a ticket for the supplied service.
If the user name does not contain a realm, then the default realm supplied
is used.</p>
<p>For this to work properly the Kerberos must be configured properly on this
machine.
That will likely mean ensuring that the edu.mit.Kerberos preference file
has the correct realms and KDCs listed.</p>
<p>IMPORTANT: This method is vulnerable to KDC spoofing attacks and it should
only be used for testing. Do not use this in any production system - your
security could be compromised if you do.</p>
</dd>
<dt><a href="#principalDetails">principalDetails(service, hostname)</a> ⇒ <code>Promise</code></dt>
<dd><p>This function returns the service principal for the server given a service type and hostname.</p>
<p>Details are looked up via the <code>/etc/keytab</code> file.</p>
</dd>
<dt><a href="#initializeClient">initializeClient(service, [options])</a> ⇒ <code><a href="#KerberosClient">Promise.&lt;KerberosClient&gt;</a></code></dt>
<dd><p>Initializes a context for client-side authentication with the given service principal.</p>
</dd>
<dt><a href="#initializeServer">initializeServer(service)</a> ⇒ <code><a href="#KerberosServer">Promise.&lt;KerberosServer&gt;</a></code></dt>
<dd><p>Initializes a context for server-side authentication with the given service principal.</p>
</dd>
</dl>

<a name="KerberosClient"></a>

## KerberosClient
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| username | <code>string</code> | The username used for authentication |
| response | <code>string</code> | The last response received during authentication steps |
| responseConf | <code>string</code> | Indicates whether confidentiality was applied or not (GSSAPI only) |
| contextComplete | <code>boolean</code> | Indicates that authentication has successfully completed or not |


* [KerberosClient](#KerberosClient)

    * [.step(challenge)](#KerberosClient+step)

    * [.wrap(challenge, [options])](#KerberosClient+wrap)

    * [.unwrap(challenge)](#KerberosClient+unwrap)


<a name="KerberosClient+step"></a>

### *kerberosClient*.step(challenge)

| Param | Type | Description |
| --- | --- | --- |
| challenge | <code>string</code> | A string containing the base64-encoded server data (which may be empty for the first step) |

Processes a single kerberos client-side step using the supplied server challenge.

<a name="KerberosClient+wrap"></a>

### *kerberosClient*.wrap(challenge, [options])

| Param | Type | Description |
| --- | --- | --- |
| challenge | <code>string</code> | The response returned after calling `unwrap` |
| [options] | <code>object</code> | Options |
| [options.user] | <code>string</code> | The user to authorize |
| [options.protect] | <code>boolean</code> | Indicates if the wrap should request message confidentiality |

Perform the client side kerberos wrap step.

<a name="KerberosClient+unwrap"></a>

### *kerberosClient*.unwrap(challenge)

| Param | Type | Description |
| --- | --- | --- |
| challenge | <code>string</code> | A string containing the base64-encoded server data |

Perform the client side kerberos unwrap step

<a name="KerberosServer"></a>

## KerberosServer
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| username | <code>string</code> | The username used for authentication |
| response | <code>string</code> | The last response received during authentication steps |
| targetName | <code>string</code> | The target used for authentication |
| contextComplete | <code>boolean</code> | Indicates that authentication has successfully completed or not |

<a name="KerberosServer+step"></a>

### *kerberosServer*.step(challenge)

| Param | Type | Description |
| --- | --- | --- |
| challenge | <code>string</code> | A string containing the base64-encoded client data |

Processes a single kerberos server-side step using the supplied client data.

<a name="GSS_C_DELEG_FLAG"></a>

## GSS_C_DELEG_FLAG
<a name="GSS_C_MUTUAL_FLAG"></a>

## GSS_C_MUTUAL_FLAG
<a name="GSS_C_REPLAY_FLAG"></a>

## GSS_C_REPLAY_FLAG
<a name="GSS_C_SEQUENCE_FLAG"></a>

## GSS_C_SEQUENCE_FLAG
<a name="GSS_C_CONF_FLAG"></a>

## GSS_C_CONF_FLAG
<a name="GSS_C_INTEG_FLAG"></a>

## GSS_C_INTEG_FLAG
<a name="GSS_C_ANON_FLAG"></a>

## GSS_C_ANON_FLAG
<a name="GSS_C_PROT_READY_FLAG"></a>

## GSS_C_PROT_READY_FLAG
<a name="GSS_C_TRANS_FLAG"></a>

## GSS_C_TRANS_FLAG
<a name="GSS_C_NO_OID"></a>

## GSS_C_NO_OID
<a name="GSS_MECH_OID_KRB5"></a>

## GSS_MECH_OID_KRB5
<a name="GSS_MECH_OID_SPNEGO"></a>

## GSS_MECH_OID_SPNEGO
<a name="version"></a>

## version
<a name="checkPassword"></a>

## checkPassword(username, password, service, [defaultRealm])

| Param | Type | Description |
| --- | --- | --- |
| username | <code>string</code> | The Kerberos user name. If no realm is supplied, then the `defaultRealm` will be used. |
| password | <code>string</code> | The password for the user. |
| service | <code>string</code> | The Kerberos service to check access for. |
| [defaultRealm] | <code>string</code> | The default realm to use if one is not supplied in the user argument. |

This function provides a simple way to verify that a user name and password
match those normally used for Kerberos authentication.
It does this by checking that the supplied user name and password can be
used to get a ticket for the supplied service.
If the user name does not contain a realm, then the default realm supplied
is used.

For this to work properly the Kerberos must be configured properly on this
machine.
That will likely mean ensuring that the edu.mit.Kerberos preference file
has the correct realms and KDCs listed.

IMPORTANT: This method is vulnerable to KDC spoofing attacks and it should
only be used for testing. Do not use this in any production system - your
security could be compromised if you do.

**Returns**: <code>Promise.&lt;null&gt;</code> - returns Promise that rejects if the password is invalid  
<a name="principalDetails"></a>

## principalDetails(service, hostname)

| Param | Type | Description |
| --- | --- | --- |
| service | <code>string</code> | The Kerberos service type for the server. |
| hostname | <code>string</code> | The hostname of the server. |

This function returns the service principal for the server given a service type and hostname.

Details are looked up via the `/etc/keytab` file.

**Returns**: <code>Promise</code> - returns Promise  
<a name="initializeClient"></a>

## initializeClient(service, [options])

| Param | Type | Description |
| --- | --- | --- |
| service | <code>string</code> | A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com'). |
| [options] | <code>object</code> | Optional settings |
| [options.principal] | <code>string</code> | Optional string containing the client principal in the form 'user@realm' (e.g. 'jdoe@example.com'). |
| [options.flags] | <code>number</code> | Optional integer used to set GSS flags. (e.g.  `GSS_C_DELEG_FLAG\|GSS_C_MUTUAL_FLAG\|GSS_C_SEQUENCE_FLAG` will allow for forwarding credentials to the remote host) |
| [options.mechOID] | <code>number</code> | Optional GSS mech OID. Defaults to None (GSS_C_NO_OID). Other possible values are `GSS_MECH_OID_KRB5`, `GSS_MECH_OID_SPNEGO`. |
| [options.user] | <code>string</code> | The username with which to authenticate.  Only used on Windows. |
| [options.pass] | <code>string</code> | The password with which to authenticate.  Only used on Windows. |

Initializes a context for client-side authentication with the given service principal.

**Returns**: [<code>Promise.&lt;KerberosClient&gt;</code>](#KerberosClient) - returns Promise  
<a name="initializeServer"></a>

## initializeServer(service)

| Param | Type | Description |
| --- | --- | --- |
| service | <code>string</code> | A string containing the service principal in the form 'type@fqdn' (e.g. 'imap@mail.apple.com'). |

Initializes a context for server-side authentication with the given service principal.

**Returns**: [<code>Promise.&lt;KerberosServer&gt;</code>](#KerberosServer) - returns Promise  
