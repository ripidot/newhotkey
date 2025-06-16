import type { NextConfig } from 'next'

const nextConfig: NextConfig = {
  // Turbopackを無効にするには空オブジェクトを渡す（これでWebpackが有効化）
  experimental: {
    turbo: {},
  },
  webpack(config, { isServer }) {
    // Watch optionsを追加してホットリロードを改善
    if (!isServer) {
      config.watchOptions = {
        poll: 1000,
        aggregateTimeout: 300,
      };
    }
    return config;
  },
};

export default nextConfig;
