FROM node:18

WORKDIR /app

# package.json, package-lock.json だけを先にコピーしてキャッシュを活用
COPY package*.json ./

RUN npm install
COPY . .
# 残りのソースコードは volume でマウントされるので COPY は不要
CMD ["npm", "run", "dev"]
