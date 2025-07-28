# server.py
from fastapi import FastAPI, Depends, HTTPException, APIRouter, status
from pydantic import BaseModel
from typing import List, Optional
from sqlalchemy import create_engine, Column, Integer, String, DateTime, ForeignKey
from sqlalchemy.orm import sessionmaker, declarative_base, relationship, Session
from datetime import datetime, timedelta
from passlib.context import CryptContext
import uvicorn
from werkzeug.security import generate_password_hash, check_password_hash

from jose import JWTError, jwt
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from fastapi.middleware.cors import CORSMiddleware

from dotenv import load_dotenv
import time
from sqlalchemy.exc import OperationalError

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), *(['..'] * 1))))
from models.base import Base
from models.user_model import User
from models.keylog_model import KeyLog

app = FastAPI()
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

def hash_password(password: str):
    return generate_password_hash(password)

def verify_password(plain_password: str, hashed_password: str):
    return check_password_hash(plain_password, hashed_password)

ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

def create_access_token(data: dict, expires_delta: timedelta = None):
    to_encode = data.copy()
    expire = datetime.utcnow() + (expires_delta or timedelta(minutes=15))
    to_encode.update({"exp": expire})
    return jwt.encode(to_encode, os.getenv("SECRET_KEY"), algorithm=ALGORITHM)

# --------------------
# DB設定
# --------------------
load_dotenv(".env")
db_url = os.getenv("DATABASE_URL")

retries = 5
while retries > 0:
    try:
        engine = create_engine(db_url)
        connection = engine.connect()
        break
    except OperationalError:
        retries -= 1
        print("connection failed... retry connecting to server")
        time.sleep(3)
else:
    raise Exception("failed to connect to server")

SessionLocal = sessionmaker(bind=engine, autoflush=False, autocommit=False)


# --------------------
# CORSを許可する
# --------------------

app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],  # ReactなどのフロントエンドのURL
    allow_credentials=True,
    allow_methods=["*"],  # 全てのHTTPメソッドを許可
    allow_headers=["*"],  # 全てのHTTPヘッダーを許可
)
# --------------------
# DBモデル
# --------------------
# class User(Base):
#     __tablename__ = "users"
#     user_id = Column(Integer, primary_key=True, index=True)
#     username = Column(String, unique=True, index=True)
#     password_hash = Column(String)
#     logs = relationship("KeyLog", back_populates="user_id")

# class KeyLog(Base):
#     __tablename__ = "keylogs"

#     id = Column(Integer, primary_key=True, autoincrement=True)
#     session_id = Column(String, index=True)
#     sequence_id = Column(Integer)
#     timestamp = Column(String)
#     key = Column(String)
#     modifiers = Column(String)
#     window_title = Column(String)
#     process_name = Column(String)
#     user_id = Column(Integer, ForeignKey("users.user_id"), default=1)

Base.metadata.create_all(bind=engine)

# --------------------
# Pydanticモデル
# --------------------
class LogEntry(BaseModel):
    message: str
    user_id: int

class UserCreate(BaseModel):
    username: str
    password: str

class UserResponse(BaseModel):
    user_id: int
    username: str

    class Config:
        orm_mode = True

class KeyLogResponse(BaseModel):
    id: int
    session_id: Optional[str]
    sequence_id: Optional[int]
    timestamp: Optional[str]
    key: Optional[str]
    modifiers: Optional[str]
    window_title: Optional[str]
    process_name: Optional[str]
    user_id: Optional[int]

    class Config:
        orm_mode = True
# --------------------
# DBsession
# --------------------
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

# --------------------
# APIendpoint
# --------------------
@app.post("/users", response_model=UserResponse)
def create_user(user: UserCreate, db: Session = Depends(get_db)):
    existing_user = db.query(User).filter(User.username == user.username).first()
    if existing_user:
        raise HTTPException(status_code=400, detail="Username already exists")
    db_user = User(username=user.username, password_hash=hash_password(user.password))
    db.add(db_user)
    db.commit()
    db.refresh(db_user)
    return db_user

@app.get("/keylogs", response_model=List[KeyLogResponse])
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog)
    return data.all()

@app.get("/testlogs", response_model=int)
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog)
    return data.filter(KeyLog.process_name == "Explorer.EXE").count()

@app.get("/testslogs", response_model=int)
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog)
    return data.count()

@app.get("/qlogs", response_model=List[KeyLogResponse])
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog)
    return data.filter(KeyLog.process_name == "Explorer.EXE").all()

@app.post("/token")
def login(form_data: OAuth2PasswordRequestForm = Depends(), db: Session = Depends(get_db)):
    user = db.query(User).filter(User.username == form_data.username).first()
    
    if not user or not verify_password(user.password_hash, form_data.password):
        raise HTTPException(status_code=401, detail="Invalid credentials")
    access_token = create_access_token(data={"sub": user.username})
    return {"access_token": access_token, "token_type": "bearer"}

def get_current_user(token: str = Depends(oauth2_scheme), db: Session = Depends(get_db)):
    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, os.getenv("SECRET_KEY"), algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        if not username:
            raise credentials_exception
    except JWTError:
        raise credentials_exception
    user = db.query(User).filter(User.username == username).first()
    if not user:
        raise credentials_exception
    return user

if __name__ == "__main__":
    uvicorn.run("main:app", host="127.0.0.1", port=8000, reload=True)
