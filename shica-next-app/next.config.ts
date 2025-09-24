import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  webpack: (config, { isServer }) => {
    if (isServer) {
      // サーバーサイドでのws用の設定
      config.externals = config.externals || [];
      config.externals.push('ws');
    } else {
      // クライアントサイドでのWebSocket関連のfallback
      config.resolve.fallback = {
        ...config.resolve.fallback,
        ws: false,
      };
    }
    return config;
  },
  // WebSocketサポート改善（新しい設定名）
  serverExternalPackages: ['ws'],
};

export default nextConfig;
