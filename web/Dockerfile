FROM node:18

WORKDIR /app
ARG BUILD_ENV=development
# ARG BUILD_ENV=production

COPY package*.json ./


# RUN if [ "${BUILD_ENV}" = "development" ]; then \
#         npm install; \
#     else \
#         npm install; \
#     fi

RUN npm install

# 開発環境のみ
COPY . .

# 残りのソースコードは volume でマウントされるので COPY は不要
# docker-compose.ymlで上書きされる
# CMD ["npm", "run", "dev"]
