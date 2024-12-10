# Changelog

All notable changes to this project will be documented in this file. See [standard-version](https://github.com/conventional-changelog/standard-version) for commit guidelines.

## [2.2.1](https://github.com/mongodb-js/kerberos/compare/v2.2.1...v2.2.1) (2024-12-10)


### ⚠ BREAKING CHANGES

* **NODE-3848:** update dependencies ([#142](https://github.com/mongodb-js/kerberos/issues/142))
* **NODE-3472:** convert to Node-API ([#137](https://github.com/mongodb-js/kerberos/issues/137))

### Features

* **async-worker:** introduce a `KerberosWorker` using lambdas ([1239ef7](https://github.com/mongodb-js/kerberos/commit/1239ef750581a00fd089f6c8ae3074ccb33f1e65))
* **checkPassword:** add implementation for checking krb5 passwords ([60f476e](https://github.com/mongodb-js/kerberos/commit/60f476e80d0c0c95c51b771e59ca1af4fba19a8b))
* **clean:** provide implementations for the clean methods ([77a77ce](https://github.com/mongodb-js/kerberos/commit/77a77ce10928a080e9a92628d71b135fa8174c0e))
* **client:** add final wrap/unwrap api endpoints ([016222f](https://github.com/mongodb-js/kerberos/commit/016222f466de5e2454b8affdb3f6d9b08d588bde))
* **client:** add implementation for client wrap/unwrap to win32 ([994604c](https://github.com/mongodb-js/kerberos/commit/994604cf3432deed883fc0859cbdd986aea655dd))
* **gss:** add `new` methods for constructing state tracking types ([274cad6](https://github.com/mongodb-js/kerberos/commit/274cad676649e98155edd398a8f40ffccc40b64d))
* **jsdoc2md:** add jsdoc2md support, and README template ([60e1ee5](https://github.com/mongodb-js/kerberos/commit/60e1ee5adab74563e4b289dc07e043d91a646306))
* **kerberos:** add getters to check for context completeness ([6a9a01d](https://github.com/mongodb-js/kerberos/commit/6a9a01dde33bfb327fa3b46f432b32729e20c04a))
* **kerberos:** implement client/server init, move to worker file ([1c857ea](https://github.com/mongodb-js/kerberos/commit/1c857eae0ae72f75ec952b34bb8b0e3740767388))
* **kerberos:** return value for `step` is the challenge response ([e153d24](https://github.com/mongodb-js/kerberos/commit/e153d240ff1a47dac2e39499d487bc1e4e6b11bf))
* **NODE-3472:** convert to Node-API ([#137](https://github.com/mongodb-js/kerberos/issues/137)) ([f9481a4](https://github.com/mongodb-js/kerberos/commit/f9481a42877c604f8aac961536fc5674ce8baa6c))
* **NODE-5505:** add compiler warnings and cast lengths ([#158](https://github.com/mongodb-js/kerberos/issues/158)) ([1e73b98](https://github.com/mongodb-js/kerberos/commit/1e73b98340d244e7c409afa6d293be43ed89080b))
* **NODE-5746:** allow runtime linking against system kerberos library ([#165](https://github.com/mongodb-js/kerberos/issues/165)) ([ce2feb3](https://github.com/mongodb-js/kerberos/commit/ce2feb3fa100b8bb3fdff502f6444ab331619108))
* **NODE-6333:** Allow callers to specify the 'protect' flag ([#198](https://github.com/mongodb-js/kerberos/issues/198)) ([515f4bf](https://github.com/mongodb-js/kerberos/commit/515f4bfa9ed2f98be2670143d34b2e1356eb7089))
* **package:** export the package version ([5be618f](https://github.com/mongodb-js/kerberos/commit/5be618fbadac269ae0e8cd4ef4fcc78f98ada151))
* **promises:** allow to access all API by promise or callback ([3b77430](https://github.com/mongodb-js/kerberos/commit/3b774306779a6c84f9d6ccf5da7a46d96ccec58e))
* **serverPrincipalDetails:** add server pricipal details method ([385fcd1](https://github.com/mongodb-js/kerberos/commit/385fcd1f55c86d75bb9f27f0d1c9840c7b2de6df))
* **src:** begin to develop the new version of the module in `src` ([f45da50](https://github.com/mongodb-js/kerberos/commit/f45da50ae4c745ff223eee5b517a9a5e10f5b125))
* **sspi:** introduce client initialization for SSPI ([6a40301](https://github.com/mongodb-js/kerberos/commit/6a403014ec4b83e9730e17527a222b483a3c648e))
* **sspi:** provide implementation for `initializeClient` ([5943f1c](https://github.com/mongodb-js/kerberos/commit/5943f1c368c8dd3f3bd1fc096e784684304e3089))
* **step:** implement client and server step methods ([5a4327c](https://github.com/mongodb-js/kerberos/commit/5a4327c17a98a4b4c07866868ba5fd3c02674336))


### Bug Fixes

* add support for node 12 ([ae6755d](https://github.com/mongodb-js/kerberos/commit/ae6755db6b1ab7256afd7c33f2051fcb74ea1060))
* **auth-process:** only send username/password if provided ([334ca9c](https://github.com/mongodb-js/kerberos/commit/334ca9c6d4ae32f8bbd76efc5981dc7b37be2ce6))
* **auth-process:** use canonicalized hostname in client init ([b1802d1](https://github.com/mongodb-js/kerberos/commit/b1802d143b0de3a8f535a1c9c57de14285fe3ae6))
* **build:** make addon buildable as static library ([#119](https://github.com/mongodb-js/kerberos/issues/119)) ([786e7d8](https://github.com/mongodb-js/kerberos/commit/786e7d83672ad5ff2718c9a440dbd180f8e7b24a))
* **check-password:** correctly validate parameters, fix test ([b772dde](https://github.com/mongodb-js/kerberos/commit/b772dded06234aca89a9554ecb1c7967725da109))
* **common:** ensure nan is being included everywhere appropriately ([7bddb24](https://github.com/mongodb-js/kerberos/commit/7bddb2406e51e58566a7127135496c2097f02788))
* **context:** add `NewInstance` methods, and make getters safer ([fd4b852](https://github.com/mongodb-js/kerberos/commit/fd4b85227c20ab301b5cb6f1266748c6ed75d9d1))
* **define-operation:** ensure optional parameters are considered ([cee7b83](https://github.com/mongodb-js/kerberos/commit/cee7b830f2aafc27a9ca1242d49ec89ba4b86003))
* **gss:** fix issue with memory corruption ([ff4167e](https://github.com/mongodb-js/kerberos/commit/ff4167e115831b96a27b50dc2dc6bbb14c0d0b49))
* **kerberos:** provide default gss flags ([b365934](https://github.com/mongodb-js/kerberos/commit/b365934676b802fb0b1ce0b7017be87640c1066e))
* **legacy:** support legacy import expectations ([615b23f](https://github.com/mongodb-js/kerberos/commit/615b23f0365313e819529fab6f647791e740b42b))
* **MONGOSH-1808:** only build universal macos binaries when creating loadable_library ([#186](https://github.com/mongodb-js/kerberos/issues/186)) ([ec3ab7a](https://github.com/mongodb-js/kerberos/commit/ec3ab7a34ea2de587c4a19cb8aad760e793564fc))
* **NODE-2129:** fix sporadic AcquireCredentialsHandle error ([#133](https://github.com/mongodb-js/kerberos/issues/133)) ([adf8346](https://github.com/mongodb-js/kerberos/commit/adf834665d7b927778669c1197d53d6f4ed6e797))
* **NODE-3350:** do not export Init function symbol ([#130](https://github.com/mongodb-js/kerberos/issues/130)) ([acdd746](https://github.com/mongodb-js/kerberos/commit/acdd7466c131494e9a2ca36eb9ad64ecda2d1366))
* **NODE-3982:** only pass username to SSPI if password is set ([#141](https://github.com/mongodb-js/kerberos/issues/141)) ([2d307a3](https://github.com/mongodb-js/kerberos/commit/2d307a3131e546d046b865e6c1d0a256f7612e0d))
* **NODE-4297:** bump prebuild install to 7.1.1 ([#145](https://github.com/mongodb-js/kerberos/issues/145)) ([142842f](https://github.com/mongodb-js/kerberos/commit/142842fae048e873caac5c83fda43c6b5b45280e))
* **NODE-5600:** use ubuntu 18 to build and publish ([#162](https://github.com/mongodb-js/kerberos/issues/162)) ([c02db0e](https://github.com/mongodb-js/kerberos/commit/c02db0e1f1a9618bb705782771535feb162d1923))
* **NODE-6108:** allow building from source on latest Node.js 20.x ([#172](https://github.com/mongodb-js/kerberos/issues/172)) ([c1f7aca](https://github.com/mongodb-js/kerberos/commit/c1f7acafb211d1b449086433578495d4ae0b869f))
* **NODE-6253:** use runtime linking against system kerberos libraries by default ([#188](https://github.com/mongodb-js/kerberos/issues/188)) ([04044d2](https://github.com/mongodb-js/kerberos/commit/04044d2814ad1d01e77f1ce87f26b03d86692cf2))
* **NODE-6265:** add Spectre Mitigation and CFG ([#190](https://github.com/mongodb-js/kerberos/issues/190)) ([54b9799](https://github.com/mongodb-js/kerberos/commit/54b97991deaaa04e87e4f3704b0dfcdcdd098a4b))
* **NODE-6320:** macos runtime linking name conflict with SSL ([#193](https://github.com/mongodb-js/kerberos/issues/193)) ([d382b56](https://github.com/mongodb-js/kerberos/commit/d382b56f2e55dab25a003c416925ba2967761a72))
* **NODE-6592:** remove dependency on `bindings` ([#220](https://github.com/mongodb-js/kerberos/issues/220)) ([b07a5d2](https://github.com/mongodb-js/kerberos/commit/b07a5d21a219a06303d7be9ad9577d2ffe3aa7c4))
* release versioning ([#184](https://github.com/mongodb-js/kerberos/issues/184)) ([a39dfcb](https://github.com/mongodb-js/kerberos/commit/a39dfcb35819ea83642505788e75d91e7f527ca3))
* **response:** ensure null or client/server response is returned ([083518f](https://github.com/mongodb-js/kerberos/commit/083518f96bfcddad15dffc123e79d89f6ec07554))
* **server:** use the correct internal method name for server init ([8c8dd35](https://github.com/mongodb-js/kerberos/commit/8c8dd3522ac682f0e0a8e618c66378b1a5bd9fb1))
* **sspi:** correct invalid null checks for user data ([163bdb9](https://github.com/mongodb-js/kerberos/commit/163bdb92e392755eb87286881fbb282420207f26))
* **sspi:** only add password and domain if they are provided ([bc48814](https://github.com/mongodb-js/kerberos/commit/bc488146d9e24501c643a9c8a57f6bfe04f3a373))
* temporarily roll back node-abi until lgeiger/node-abi/[#90](https://github.com/mongodb-js/kerberos/issues/90) is resolved ([880ae2e](https://github.com/mongodb-js/kerberos/commit/880ae2eee6a8fe565ab627717d1d81ae85896abf))
* **this:** use the correct reference to `this` for object unwrapping ([1acfb20](https://github.com/mongodb-js/kerberos/commit/1acfb20dd7aa5bb771a0dfbdeac2223afacf2164))
* **unique_ptr:** ensure we include &lt;memory&gt; where required ([e3d9afb](https://github.com/mongodb-js/kerberos/commit/e3d9afb3730bcd6d8161c5486a03180d9fa6eda1))
* **validation:** identify invalid types for non terminal optionals ([a5b98b6](https://github.com/mongodb-js/kerberos/commit/a5b98b61726e7a0dbc0609ef06d79d6445973683))
* **warnings:** set clang compiler pragmas only when clang is detected ([048479d](https://github.com/mongodb-js/kerberos/commit/048479d8c0bbd3f9ef63dbcdfac6e9ff02147b3b))
* **win32:** `windows` -&gt; `win32` in bindings.gyp ([0221c06](https://github.com/mongodb-js/kerberos/commit/0221c065c000e1457f28c8b868fb207582a807cb))
* **win32:** cleanup client state in addon destructor ([5394561](https://github.com/mongodb-js/kerberos/commit/53945614d556f5af42e8483af03127602f98eb19))
* **win32:** initialize with a domain, if one is provided ([309ba61](https://github.com/mongodb-js/kerberos/commit/309ba6178e7b667a99ec50186bd8424101f54d40))


### Miscellaneous Chores

* fix release please version ([#224](https://github.com/mongodb-js/kerberos/issues/224)) ([94668a2](https://github.com/mongodb-js/kerberos/commit/94668a2100e098c03eaf6c4ad09d90ecb0cff4a9))
* **main:** release 2.1.1 [skip-ci] ([#187](https://github.com/mongodb-js/kerberos/issues/187)) ([c2acb18](https://github.com/mongodb-js/kerberos/commit/c2acb189837296303b4cfe84de9c55475edf96d8))
* **NODE-3848:** update dependencies ([#142](https://github.com/mongodb-js/kerberos/issues/142)) ([8c06728](https://github.com/mongodb-js/kerberos/commit/8c067286add33a2b56aeaf10e41f0409c5fe1d5b))

## [2.2.1](https://github.com/mongodb-js/kerberos/compare/v2.2.0...v2.2.1) (2024-12-03)


### Bug Fixes

* **NODE-6592:** remove dependency on `bindings` ([#220](https://github.com/mongodb-js/kerberos/issues/220)) ([b07a5d2](https://github.com/mongodb-js/kerberos/commit/b07a5d21a219a06303d7be9ad9577d2ffe3aa7c4))

## [2.2.0](https://github.com/mongodb-js/kerberos/compare/v2.1.2...v2.2.0) (2024-09-05)


### Features

* **NODE-6333:** Allow callers to specify the 'protect' flag ([#198](https://github.com/mongodb-js/kerberos/issues/198)) ([515f4bf](https://github.com/mongodb-js/kerberos/commit/515f4bfa9ed2f98be2670143d34b2e1356eb7089))

## [2.1.2](https://github.com/mongodb-js/kerberos/compare/v2.1.1...v2.1.2) (2024-08-12)


### Bug Fixes

* **NODE-6320:** macos runtime linking name conflict with SSL ([#193](https://github.com/mongodb-js/kerberos/issues/193)) ([d382b56](https://github.com/mongodb-js/kerberos/commit/d382b56f2e55dab25a003c416925ba2967761a72))

## [2.1.1](https://github.com/mongodb-js/kerberos/compare/v2.1.1...v2.1.0) (2024-08-06)


### Bug Fixes

* **MONGOSH-1808:** only build universal macos binaries when creating loadable_library ([#186](https://github.com/mongodb-js/kerberos/issues/186)) ([ec3ab7a](https://github.com/mongodb-js/kerberos/commit/ec3ab7a34ea2de587c4a19cb8aad760e793564fc))
* **NODE-6253:** use runtime linking against system kerberos libraries by default ([#188](https://github.com/mongodb-js/kerberos/issues/188)) ([04044d2](https://github.com/mongodb-js/kerberos/commit/04044d2814ad1d01e77f1ce87f26b03d86692cf2))
* **NODE-6265:** add Spectre Mitigation and CFG ([#190](https://github.com/mongodb-js/kerberos/issues/190)) ([54b9799](https://github.com/mongodb-js/kerberos/commit/54b97991deaaa04e87e4f3704b0dfcdcdd098a4b))
* **NODE-6108:** allow building from source on latest Node.js 20.x ([#172](https://github.com/mongodb-js/kerberos/issues/172)) ([c1f7aca](https://github.com/mongodb-js/kerberos/commit/c1f7acafb211d1b449086433578495d4ae0b869f))

## [2.1.1-alpha.0](https://github.com/mongodb-js/kerberos/compare/v2.1.1-alpha.0...v2.1.0) (2024-06-28)


### Bug Fixes

* release versioning ([#184](https://github.com/mongodb-js/kerberos/issues/184)) ([a39dfcb](https://github.com/mongodb-js/kerberos/commit/a39dfcb35819ea83642505788e75d91e7f527ca3))


## [2.1.0](https://github.com/mongodb-js/kerberos/compare/v2.0.3...v2.1.0) (2023-11-21)


### Features

* **NODE-5746:** allow runtime linking against system kerberos library ([#165](https://github.com/mongodb-js/kerberos/issues/165)) ([ce2feb3](https://github.com/mongodb-js/kerberos/commit/ce2feb3fa100b8bb3fdff502f6444ab331619108))

### [2.0.3](https://github.com/mongodb-js/kerberos/compare/v2.0.2...v2.0.3) (2023-09-01)


### Bug Fixes

* **NODE-5600:** use ubuntu 18 to build and publish ([#162](https://github.com/mongodb-js/kerberos/issues/162)) ([c02db0e](https://github.com/mongodb-js/kerberos/commit/c02db0e1f1a9618bb705782771535feb162d1923))

### [2.0.2](https://github.com/mongodb-js/kerberos/compare/v2.0.1...v2.0.2) (2023-08-28)


### Features

* **NODE-5505:** add compiler warnings and cast lengths ([#158](https://github.com/mongodb-js/kerberos/issues/158)) ([1e73b98](https://github.com/mongodb-js/kerberos/commit/1e73b98340d244e7c409afa6d293be43ed89080b))

### [2.0.1](https://github.com/mongodb-js/kerberos/compare/v2.0.0...v2.0.1) (2022-07-14)


### Bug Fixes

* **NODE-4297:** bump prebuild install to 7.1.1 ([#145](https://github.com/mongodb-js/kerberos/issues/145)) ([142842f](https://github.com/mongodb-js/kerberos/commit/142842fae048e873caac5c83fda43c6b5b45280e))

## [2.0.0](https://github.com/mongodb-js/kerberos/compare/v2.0.0-beta.0...v2.0.0) (2022-02-22)


### ⚠ BREAKING CHANGES

* **NODE-3848:** update dependencies (#142)

### Bug Fixes

* **NODE-3982:** only pass username to SSPI if password is set ([#141](https://github.com/mongodb-js/kerberos/issues/141)) ([2d307a3](https://github.com/mongodb-js/kerberos/commit/2d307a3131e546d046b865e6c1d0a256f7612e0d))


* **NODE-3848:** update dependencies ([#142](https://github.com/mongodb-js/kerberos/issues/142)) ([8c06728](https://github.com/mongodb-js/kerberos/commit/8c067286add33a2b56aeaf10e41f0409c5fe1d5b))

## [2.0.0-beta.0](https://github.com/mongodb-js/kerberos/compare/v1.1.7...v2.0.0-beta.0) (2021-10-06)


### ⚠ BREAKING CHANGES

* **NODE-3472:** convert to Node-API (#137)

### Features

* **NODE-3472:** convert to Node-API ([#137](https://github.com/mongodb-js/kerberos/issues/137)) ([f9481a4](https://github.com/mongodb-js/kerberos/commit/f9481a42877c604f8aac961536fc5674ce8baa6c))

### [1.1.7](https://github.com/mongodb-js/kerberos/compare/v1.1.5...v1.1.7) (2021-07-20)


### Bug Fixes

* **NODE-2129:** fix sporadic AcquireCredentialsHandle error ([#133](https://github.com/mongodb-js/kerberos/issues/133)) ([adf8346](https://github.com/mongodb-js/kerberos/commit/adf834665d7b927778669c1197d53d6f4ed6e797))
* **NODE-3350:** do not export Init function symbol ([#130](https://github.com/mongodb-js/kerberos/issues/130)) ([acdd746](https://github.com/mongodb-js/kerberos/commit/acdd7466c131494e9a2ca36eb9ad64ecda2d1366))

<a name="1.1.6"></a>
## [1.1.6](https://github.com/mongodb-js/kerberos/compare/v1.1.5...v1.1.6) (2021-07-20)


### Bug Fixes

* **NODE-2129:** fix sporadic AcquireCredentialsHandle error ([#133](https://github.com/mongodb-js/kerberos/issues/133)) ([adf8346](https://github.com/mongodb-js/kerberos/commit/adf834665d7b927778669c1197d53d6f4ed6e797))
* **NODE-3350:** do not export Init function symbol ([#130](https://github.com/mongodb-js/kerberos/issues/130)) ([acdd746](https://github.com/mongodb-js/kerberos/commit/acdd7466c131494e9a2ca36eb9ad64ecda2d1366))


<a name="1.1.5"></a>
## [1.1.5](https://github.com/mongodb-js/kerberos/compare/v1.1.4...v1.1.5) (2021-04-06)


### Bug Fixes

* temporarily roll back node-abi until lgeiger/node-abi/[#90](https://github.com/mongodb-js/kerberos/issues/90) is resolved ([880ae2e](https://github.com/mongodb-js/kerberos/commit/880ae2eee6a8fe565ab627717d1d81ae85896abf))
* **build:** make addon buildable as static library ([#119](https://github.com/mongodb-js/kerberos/issues/119)) ([786e7d8](https://github.com/mongodb-js/kerberos/commit/786e7d83672ad5ff2718c9a440dbd180f8e7b24a))

<a name="1.1.4"></a>
## [1.1.4](https://github.com/mongodb-js/kerberos/compare/v1.1.3...v1.1.4) (2020-10-13)



<a name="1.1.3"></a>
## [1.1.3](https://github.com/mongodb-js/kerberos/compare/v1.2.0...v1.1.3) (2019-08-27)


### Bug Fixes

* add support for node 12 ([ae6755d](https://github.com/mongodb-js/kerberos/commit/ae6755d))



<a name="1.1.2"></a>
## [1.1.2](https://github.com/mongodb-js/kerberos/compare/v1.1.1...v1.1.2) (2018-11-01)


### Bug Fixes

* **auth-process:** only send username/password if provided ([334ca9c](https://github.com/mongodb-js/kerberos/commit/334ca9c))
* **auth-process:** use canonicalized hostname in client init ([b1802d1](https://github.com/mongodb-js/kerberos/commit/b1802d1))



<a name="1.1.1"></a>
## [1.1.1](https://github.com/mongodb-js/kerberos/compare/v1.1.0...v1.1.1) (2018-10-30)


### Bug Fixes

* **sspi:** only add password and domain if they are provided ([bc48814](https://github.com/mongodb-js/kerberos/commit/bc48814))



<a name="1.1.0"></a>
# [1.1.0](https://github.com/mongodb-js/kerberos/compare/v1.0.0...v1.1.0) (2018-10-26)


### Bug Fixes

* **sspi:** correct invalid null checks for user data ([163bdb9](https://github.com/mongodb-js/kerberos/commit/163bdb9))


### Features

* **package:** export the package version ([5be618f](https://github.com/mongodb-js/kerberos/commit/5be618f))

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


0.0.23 07-03-2017
-----------------
- SSPI implemented missing _sspi_FreeCredentialsHandle to correctly cleanup credentials allocation on call to destructor of the C++ instance.
- Updated nan.h dependency to 2.5.x series for Node 7.6.x or higher.

0.0.22 10-11-2016
-----------------
- Updated nan.h dependency to 2.4.x series for Node 6.8.x or higher.
- The length calculations are off by one meaning it impossible to not set the password (Issue #54, http://www.github.com/tlbdk).

0.0.21 04-28-2016
-----------------
- Updated nan.h dependency to 2.3.x series for Node 6.0.

0.0.20 04-26-2016
-----------------
- Updated nan.h dependency to 2.2.x series.
- Fixed minor compilation warnings due to v8 C++ ABI changes.

0.0.19 03-07-2016
-----------------
- Fix installation error (Issue #1).
- Allow passing down off CANONICALIZE_HOST_NAME and SERVICE_REALM options.

0.0.18 01-19-2016
-----------------
- remove builderror.log.

0.0.17 10-30-2015
-----------------
- Reverted changes in package.json from 0.0.16.

0.0.16 10-26-2015
-----------------
- Removed (exit 0) on build to let correct failure happen.
