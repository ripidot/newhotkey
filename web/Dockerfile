FROM node:18

WORKDIR /app

# package.json, package-lock.json だけを先にコピーしてキャッシュを活用
COPY package*.json ./
RUN npm install

# 開発環境のみ
COPY . .

# 残りのソースコードは volume でマウントされるので COPY は不要
# docker-compose.ymlで上書きされる
# CMD ["npm", "run", "dev"]
