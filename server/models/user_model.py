# user_model.py
from sqlalchemy import Column, Integer, String
from models.base import Base
from sqlalchemy.orm import relationship
class User(Base):
    __tablename__ = "users"
    user_id = Column(Integer, primary_key=True, index=True)
    username = Column(String, unique=True, index=True)
    password_hash = Column(String)
    logs = relationship("KeyLog", back_populates="user")