from datetime import datetime, timedelta
from jose import jwt, JWTError
from passlib.context import CryptContext
from werkzeug.security import generate_password_hash, check_password_hash
from app.core.config import SECRET_KEY, ALGORITHM

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

def hash_password(password: str):
    return generate_password_hash(password)

def verify_password(plain_password: str, hashed_password: str):
    return check_password_hash(plain_password, hashed_password)

def create_access_token(data: dict, expires_delta: timedelta | None = None):
    to_encode = data.copy()
    expire = datetime.utcnow() + (expires_delta or timedelta(minutes=15))
    to_encode.update({"exp": expire})
    return jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
