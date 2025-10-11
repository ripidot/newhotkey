# main.py
# ============================================================
# 標準ライブラリ
# ============================================================
import os
import sys
import time
from datetime import datetime, timedelta

# ============================================================
# サードパーティ（外部）ライブラリ
# ============================================================
from dotenv import load_dotenv
from fastapi import FastAPI, Depends, HTTPException, status, APIRouter
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from jose import JWTError, jwt
from passlib.context import CryptContext
from werkzeug.security import generate_password_hash, check_password_hash
from pydantic import BaseModel
from sqlalchemy import (
    create_engine,
    Column,
    Integer,
    String,
    DateTime,
    ForeignKey,
    func,
    cast,
    select,
    asc,
    desc,
)
from sqlalchemy.orm import sessionmaker, declarative_base, relationship, Session
from sqlalchemy.exc import IntegrityError, OperationalError
import uvicorn

# ============================================================
# アプリ内（自作モジュール）
# ============================================================
from models.base import Base
from models.user_model import User
from models.keylog_model import KeyLog


sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), *(['..'] * 1)))) # pythonpath設定で解消


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
    id: Optional[int]
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

class SentFlagUpdate(BaseModel):
    ids: list[int]
    sent_flag: int

class FailedLog(BaseModel):
    id: int
    reason: str

class OrderByClause(BaseModel):
    field: str   # "user_id" や "count"
    direction: str  # "asc" or "desc"
class AggregateClause(BaseModel):
    func: str            # "count", "sum", "avg", "max", "min"
    field: Optional[str] = None  # count(*) の場合は省略可能
    alias: str           # API レスポンスのカラム名
class Reqtest(BaseModel):
    select: List[str]
    where: Optional[dict] = None
    group_by: Optional[List[str]] = None
    aggregates: Optional[List[AggregateClause]] = None
    order_by: Optional[List[OrderByClause]] = None
    limit: Optional[int] = None

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

def make_batch_response(success_ids: list[int], failed: List[FailedLog]) -> JSONResponse:
    if len(failed) == 0:
        status_code = 200
    elif 0 < len(failed):
        status_code = 400
    elif all(reason == "duplicate" for reason in (f.reason for f in failed)):
        status_code = 400
    else:
        status_code = 500

    return JSONResponse(
        status_code=status_code,
        content={
            "success_ids": success_ids,
            "failed": [f.dict() for f in failed],
            "success_count": len(success_ids),
            "failed_count": len(failed),
        }
    )

@app.post("/keylogs/batch")
def create_keylogs(logs: List[KeyLogCreate], db: Session = Depends(get_db)):
    success_ids = []
    failed: List[FailedLog] = []
    for log in logs:
        try:
            db_log = (KeyLog(
                session_id=log.session_id,
                sequence_id=log.sequence_id,
                timestamp=log.timestamp,
                key=log.key,
                modifiers=log.modifiers,
                window_title=log.window_title,
                process_name=log.process_name,
                user_id=log.user_id
            ))
            db.add(db_log)
            db.commit()
            db.refresh(db_log)
            success_ids.append(log.id)

        except IntegrityError as e:
            db.rollback()
            failed.append(FailedLog(id=log.id, reason="duplicate"))
        except Exception as e:
            db.rollback()
            failed.append(FailedLog(id=log.id, reason=str(e)))
    return make_batch_response(success_ids, failed)

@app.put("/update_sent_flag")
def update_sent_flag(data: SentFlagUpdate, db: Session = Depends(get_db)):
    db.query(KeyLog).filter(KeyLog.id.in_(data.ids)) \
        .update({"sent_flag": data.sent_flag}, synchronize_session=False)
    db.commit()
    return {"status": "success", "updated_count": len(data.ids)}


@app.post("/postall")
def count_all(req: Reqtest, db: Session = Depends(get_db)):
    special_columns = {
        "day": func.date_trunc('day', cast(KeyLog.timestamp, DateTime)).label("day"),
        "week": func.date_trunc('week', cast(KeyLog.timestamp, DateTime)).label("week"),
        "month": func.date_trunc('month', cast(KeyLog.timestamp, DateTime)).label("month"),        
        "count": func.count().label("count")
    }

    select_clauses = []
    for col in req.select:
        if col in special_columns:
            select_clauses.append(special_columns[col]) # select句にspecial_columnsを追加
        else:
            select_clauses.append(getattr(KeyLog, col))
    stmt = select(*select_clauses).select_from(KeyLog)

    agg_exprs = {}
    if req.aggregates:
        for agg in req.aggregates:
            if agg.func == "count":
                expr = func.count().label(agg.alias)
            else:
                col = getattr(KeyLog, agg.field)
                if agg.func == "sum":
                    expr = func.sum(col).label(agg.alias)
                elif agg.func == "avg":
                    expr = func.avg(col).label(agg.alias)
                elif agg.func == "max":
                    expr = func.max(col).label(agg.alias)
                elif agg.func == "min":
                    expr = func.min(col).label(agg.alias)
                else:
                    raise ValueError(f"Unsupported aggregate: {agg.func}")
            agg_exprs[agg.alias] = expr
            stmt = stmt.add_columns(expr)

    if req.where:
        for field, value in req.where.items():
            stmt = stmt.where(getattr(KeyLog, field) == value)

    if req.group_by:
        group_clauses = []
        for col in req.group_by:
            if col in special_columns:
                group_clauses.append(special_columns[col])
            else:
                group_clauses.append(getattr(KeyLog, col))
        stmt = stmt.group_by(*group_clauses)

    if req.order_by:
        order_clauses = []
        for ob in req.order_by:
            if ob.field in special_columns:
                col = special_columns[ob.field]
            else:
                col = getattr(KeyLog, ob.field)
            if ob.direction.lower() == "asc":
                order_clauses.append(asc(col))
            else:
                order_clauses.append(desc(col))
        stmt = stmt.order_by(*order_clauses)
    
    if req.limit:
        stmt = stmt.limit(req.limit)

    results = db.execute(stmt).all()
    return {"results": [dict(row._mapping) for row in results]}


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
