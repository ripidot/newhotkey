# keylog_model.py
from sqlalchemy import Column, Integer, String, ForeignKey
from models.base import Base
from sqlalchemy.orm import relationship

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
    user_id = Column(Integer, ForeignKey("users.user_id"), default=1)
    user = relationship("User", back_populates="logs")