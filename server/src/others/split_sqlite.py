from sqlalchemy import create_engine, Column, Integer, String, ForeignKey
from sqlalchemy.orm import Session
from sqlalchemy.ext.declarative import declarative_base
Base = declarative_base()

class KeyLog(Base):
    __tablename__ = "keylogs"

    id = Column(Integer, primary_key=True, autoincrement=True)
    session_id = Column(String, index=True)
    sequence_id = Column(Integer)
    timestamp = Column(String)
    key = Column(String)
    modifiers = Column(String)
    window_title = Column(String)
    process_name = Column(String)
    user_id = Column(Integer, default=1)

# 元DBと新DBを定義
SOURCE_DB = "sqlite:///../db/keylog_bac.db"
DEST_DB = "sqlite:///../db/keylog_bac_2.db"

# エンジン作成
source_engine = create_engine(SOURCE_DB)
dest_engine = create_engine(DEST_DB)

# 新しいDBにテーブル作成
Base.metadata.create_all(dest_engine)

# セッション作成
source_session = Session(bind=source_engine)
dest_session = Session(bind=dest_engine)

# クエリ条件：2025年6月1日以降かつ user_id=1 に限定
query = (
    source_session.query(KeyLog)
    .filter((1000001 <= KeyLog.id) & (KeyLog.id <= 2000000))
)
# フィルタ通過分だけ新DBに保存（UTF-8判定付き）
for log in query.yield_per(1000):
    try:
        # log.window_title.encode("utf-8").decode("utf-8")

        new_log = KeyLog(
            session_id=log.session_id,
            sequence_id=log.sequence_id,
            timestamp=log.timestamp,
            key=log.key,
            modifiers=log.modifiers,
            window_title=log.window_title,
            process_name=log.process_name,
            user_id=log.user_id,
        )
        dest_session.add(new_log)
    except Exception as e:
        # print(f"error: {c}")
        print(f"Skipped one record due to error: {e}")
        continue

dest_session.commit()
print("Filtered DB を保存しました。")