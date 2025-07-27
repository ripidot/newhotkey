import os
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from dotenv import load_dotenv

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), *(['..'] * 2))))
from models.user_model import User
from models.keylog_model import KeyLog
from models.base import Base

# 環境変数読み込み
load_dotenv()

# DB接続情報
SQLITE_DB_PATH = "sqlite:///./keylog_sample.db"
POSTGRES_DB_URL = os.getenv("DATABASE_URL")

# エンジン作成
sqlite_engine = create_engine(SQLITE_DB_PATH)
postgres_engine = create_engine(POSTGRES_DB_URL)

# セッション作成
SQLiteSession = sessionmaker(bind=sqlite_engine)
PostgresSession = sessionmaker(bind=postgres_engine)

sqlite_session = SQLiteSession()
postgres_session = PostgresSession()

def migrate_table(model_class):
    print(f"⏳ Migrating table: {model_class.__tablename__}")
    records = sqlite_session.query(model_class).all()
    for record in records:
        postgres_session.merge(record)  # `merge`は既存レコードの重複にも対応
    postgres_session.commit()
    print(f"✅ Done: {model_class.__tablename__}")

def main():
    # PostgreSQLにスキーマを作成（必要に応じて）
    Base.metadata.create_all(bind=postgres_engine)

    # 対象テーブルを列挙（順序に注意：外部キー依存がある場合はUser→Logなど順に）
    for model in [User, KeyLog]:
        migrate_table(model)

    sqlite_session.close()
    postgres_session.close()
    print("🎉 All data migrated successfully.")

if __name__ == "__main__":
    main()
