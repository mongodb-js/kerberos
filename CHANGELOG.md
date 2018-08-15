# Change Log

All notable changes to this project will be documented in this file. See [standard-version](https://github.com/conventional-changelog/standard-version) for commit guidelines.

<a name="1.0.0"></a>
# [1.0.0](https://github.com/christkv/kerberos/compare/v0.0.24...v1.0.0) (2018-08-15)


### Bug Fixes

* **check-password:** correctly validate parameters, fix test ([b772dde](https://github.com/christkv/kerberos/commit/b772dde))
* **common:** ensure nan is being included everywhere appropriately ([7bddb24](https://github.com/christkv/kerberos/commit/7bddb24))
* **context:** add `NewInstance` methods, and make getters safer ([fd4b852](https://github.com/christkv/kerberos/commit/fd4b852))
* **gss:** fix issue with memory corruption ([ff4167e](https://github.com/christkv/kerberos/commit/ff4167e))
* **kerberos:** provide default gss flags ([b365934](https://github.com/christkv/kerberos/commit/b365934))
* **legacy:** support legacy import expectations ([615b23f](https://github.com/christkv/kerberos/commit/615b23f))
* **response:** ensure null or client/server response is returned ([083518f](https://github.com/christkv/kerberos/commit/083518f))
* **server:** use the correct internal method name for server init ([8c8dd35](https://github.com/christkv/kerberos/commit/8c8dd35))
* **this:** use the correct reference to `this` for object unwrapping ([1acfb20](https://github.com/christkv/kerberos/commit/1acfb20))
* **unique_ptr:** ensure we include <memory> where required ([e3d9afb](https://github.com/christkv/kerberos/commit/e3d9afb))
* **warnings:** set clang compiler pragmas only when clang is detected ([048479d](https://github.com/christkv/kerberos/commit/048479d))
* **win32:** `windows` -> `win32` in bindings.gyp ([0221c06](https://github.com/christkv/kerberos/commit/0221c06))
* **win32:** cleanup client state in addon destructor ([5394561](https://github.com/christkv/kerberos/commit/5394561))
* **win32:** initialize with a domain, if one is provided ([309ba61](https://github.com/christkv/kerberos/commit/309ba61))


### Features

* **async-worker:** introduce a `KerberosWorker` using lambdas ([1239ef7](https://github.com/christkv/kerberos/commit/1239ef7))
* **checkPassword:** add implementation for checking krb5 passwords ([60f476e](https://github.com/christkv/kerberos/commit/60f476e))
* **clean:** provide implementations for the clean methods ([77a77ce](https://github.com/christkv/kerberos/commit/77a77ce))
* **client:** add final wrap/unwrap api endpoints ([016222f](https://github.com/christkv/kerberos/commit/016222f))
* **client:** add implementation for client wrap/unwrap to win32 ([994604c](https://github.com/christkv/kerberos/commit/994604c))
* **gss:** add `new` methods for constructing state tracking types ([274cad6](https://github.com/christkv/kerberos/commit/274cad6))
* **jsdoc2md:** add jsdoc2md support, and README template ([60e1ee5](https://github.com/christkv/kerberos/commit/60e1ee5))
* **kerberos:** add getters to check for context completeness ([6a9a01d](https://github.com/christkv/kerberos/commit/6a9a01d))
* **kerberos:** implement client/server init, move to worker file ([1c857ea](https://github.com/christkv/kerberos/commit/1c857ea))
* **kerberos:** return value for `step` is the challenge response ([e153d24](https://github.com/christkv/kerberos/commit/e153d24))
* **promises:** allow to access all API by promise or callback ([3b77430](https://github.com/christkv/kerberos/commit/3b77430))
* **serverPrincipalDetails:** add server pricipal details method ([385fcd1](https://github.com/christkv/kerberos/commit/385fcd1))
* **src:** begin to develop the new version of the module in `src` ([f45da50](https://github.com/christkv/kerberos/commit/f45da50))
* **sspi:** introduce client initialization for SSPI ([6a40301](https://github.com/christkv/kerberos/commit/6a40301))
* **sspi:** provide implementation for `initializeClient` ([5943f1c](https://github.com/christkv/kerberos/commit/5943f1c))
* **step:** implement client and server step methods ([5a4327c](https://github.com/christkv/kerberos/commit/5a4327c))
