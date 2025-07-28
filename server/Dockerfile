FROM python:3.11-slim

# 作業ディレクトリを設定
WORKDIR /app

USER root

# 必要ファイルのコピー
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# 起動コマンド（任意で変更）
CMD ["uvicorn", "src.main:app", "--host", "0.0.0.0", "--port", "8000", "--reload"]
