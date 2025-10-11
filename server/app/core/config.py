import os
import time
from dotenv import load_dotenv
from sqlalchemy import create_engine
from sqlalchemy.exc import OperationalError

load_dotenv(".env")

DATABASE_URL = os.getenv("DATABASE_URL")
SECRET_KEY = os.getenv("SECRET_KEY")
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30
