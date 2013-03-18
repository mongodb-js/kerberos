exports.setUp = function(callback) {
  callback();
}

exports.tearDown = function(callback) {
  callback();
}

exports['Simple initialize of Kerberos object'] = function(test) {
  var kerberos = require('../build/Release/kerberos');
  console.dir(kerberos)

  test.done();
}