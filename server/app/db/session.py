from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from app.core.config import DATABASE_URL
from sqlalchemy.exc import OperationalError
import time

engine = None
SessionLocal = None

def init_db():
    global engine, SessionLocal
    retries = 5
    while retries > 0:
        try:
            engine = create_engine(
                DATABASE_URL,
                pool_pre_ping=True,  # 死んだコネクションを検出して再接続
                poolclass=NullPool,  # 毎回新しい接続を張る（Neon向け）
            )
            connection = engine.connect()
            connection.close()
            SessionLocal = sessionmaker(bind=engine, autoflush=False, autocommit=False)
            print("✅ DB connected successfully.")
            return
        except OperationalError:
            retries -= 1
            print("❌ connection failed... retry connecting to server")
            time.sleep(3)
    raise Exception("Failed to connect to database after retries.")

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()
