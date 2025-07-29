FROM ubuntu:22.04

# 必要なツールをインストール
RUN apt-get update && \
    apt-get install -y \
    cmake \
    g++ \
    make \
    build-essential

# 作業ディレクトリを作成
WORKDIR /app

# プロジェクト全体をコピー
# COPY . .

# ビルドディレクトリを作ってそこに移動し、cmakeとmakeを実行
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make

# アプリを実行（ここではリリースビルドのパスを想定）
CMD ["./build/hotkey_app"]
