# create_tables.py
from sqlalchemy import create_engine
from models.base import Base

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), *(['..'] * 2))))
from models.user_model import User
from models.keylog_model import KeyLog


load_dotenv(".env")
db_url = os.getenv("DATABASE_URL")
engine = create_engine(db_url)

Base.metadata.create_all(bind=engine)

print("tables created successfully")
print(Base.metadata.tables)