# ベースイメージ
FROM node:18

# 作業ディレクトリ作成
WORKDIR /app

# package.jsonとpackage-lock.jsonをコピー
COPY package*.json ./

# 依存関係インストール
RUN npm install

# 残りのファイルをコピー
COPY . .

# ポート3000を公開
EXPOSE 3000

# 開発サーバー起動
CMD ["npm", "run", "dev"]
