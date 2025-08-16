# main.py
from fastapi import FastAPI, Depends, HTTPException, APIRouter, status
from pydantic import BaseModel
from typing import List, Optional, Dict
from sqlalchemy import create_engine, Column, Integer, String, DateTime, ForeignKey, func, cast
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
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), *(['..'] * 1)))) # pythonpath設定で解消
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
# Pydanticモデル
# --------------------
class KeyLogCreate(BaseModel):
    session_id: Optional[str]
    sequence_id: Optional[int]
    timestamp: Optional[datetime]
    key: Optional[str]
    modifiers: Optional[str]
    window_title: Optional[str]
    process_name: Optional[str]
    user_id: Optional[int]

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
    timestamp: Optional[datetime]
    key: Optional[str]
    modifiers: Optional[str]
    window_title: Optional[str]
    process_name: Optional[str]
    user_id: Optional[int]

    class Config:
        orm_mode = True

class WeeklyCountResponse(BaseModel):
    week_start: str
    count: int

class SentFlagUpdate(BaseModel):
    ids: list[int]
    sent_flag: int
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

@app.post("/keylogs")
def create_keylog(item: KeyLogCreate, db: Session = Depends(get_db)):
    log = KeyLog(
        session_id=item.session_id, 
        sequence_id = item.sequence_id,
        timestamp = item.timestamp,
        key = item.key,
        modifiers = item.modifiers,
        window_title = item.window_title,
        process_name = item.process_name,
        user_id = item.user_id
    )
    db.add(log)
    db.commit()
    return

@app.post("/keylogs/batch")
def create_keylogs(logs: List[KeyLogCreate], db: Session = Depends(get_db)):
    db_logs = []
    for log in logs:
        db_logs.append(KeyLog(
            session_id=log.session_id,
            sequence_id=log.sequence_id,
            timestamp=log.timestamp,
            key=log.key,
            modifiers=log.modifiers,
            window_title=log.window_title,
            process_name=log.process_name,
            user_id=log.user_id
        ))
    
    db.add_all(db_logs)
    db.commit()
    return {"inserted": len(db_logs)}

@app.put("/update_sent_flag")
def update_sent_flag(data: SentFlagUpdate, db: Session = Depends(get_db)):
    db.query(KeyLog).filter(KeyLog.id.in_(data.ids)) \
        .update({"sent_flag": data.sent_flag}, synchronize_session=False)
    db.commit()
    return {"status": "success", "updated_count": len(data.ids)}

@app.get("/keylogs", response_model=List[KeyLogResponse])
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog)
    return data.all()

@app.get("/testslogs", response_model=int)
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog)
    return data.count()

@app.get("/querytest", response_model=int)
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog).filter(200000 < KeyLog.id).filter(KeyLog.id < 250000).filter(KeyLog.process_name == "Explorer.EXE")
    return data.count()

@app.get("/querytest2", response_model=int)
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog).filter(KeyLog.process_name == "Explorer.EXE")
    return data.count()

@app.get("/querytest3", response_model=List[KeyLogResponse])
def read_keylogs(db: Session = Depends(get_db)):
    data = db.query(KeyLog).filter(KeyLog.id == 200000).all()
    return data

@app.get("/querytest4", response_model=List[WeeklyCountResponse])
def read_keylogs(db: Session = Depends(get_db)):
    results = (
        db.query(
            func.date_trunc('week', cast(KeyLog.timestamp, DateTime)).label('week_start'),
            func.count().label('count')
        )
        .filter(KeyLog.process_name == "Explorer.EXE")
        .group_by(func.date_trunc('week', cast(KeyLog.timestamp, DateTime)))
        .order_by(func.date_trunc('week', cast(KeyLog.timestamp, DateTime)))
        .all()
    )

    return [
        {"week_start": week_start.isoformat(), "count": count}
        for week_start, count in results
    ]

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
