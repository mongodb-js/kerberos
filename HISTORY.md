# Change Log

All notable changes to this project will be documented in this file. See [standard-version](https://github.com/conventional-changelog/standard-version) for commit guidelines.

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
