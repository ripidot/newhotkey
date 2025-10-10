
import type { Configuration as WebpackDevMiddlewareConfig } from 'webpack-dev-middleware'

module.exports = {
  webpack: (config : WebpackDevMiddlewareConfig) => {
    config.watchOptions = {
      ignored: ['**/node_modules'],
      aggregateTimeout: 300,
      poll: 5000
    }
    return config;
  }
}
