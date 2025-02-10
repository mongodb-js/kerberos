'use strict';

const path = require('path');

const isProduction = process.env.NODE_ENV === 'production';

const config = {
  entry: './src/index.js',
  output: {
    path: path.resolve(__dirname, 'dist')
  },
  experiments: { topLevelAwait: true },
  target: 'node',
  module: {
    rules: [
      {
        test: /\.node$/i,
        loader: 'node-loader'
      }
    ]
  },
  resolve: {
    extensions: ['.js', '...', '.node'],
    fallback: { crypto: false }
  }
};

module.exports = () => {
  if (isProduction) {
    config.mode = 'production';
  } else {
    config.mode = 'development';
  }
  return config;
};
