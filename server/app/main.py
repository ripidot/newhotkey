from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from app.db.session import init_db
from app.routers import user_router, logs_router
import os

app = FastAPI()

# CORS設定
# origins = [
#     "http://localhost:3000", # 開発用
#     "https://newhotkey.vercel.app", # vercelのドメイン
#     "https://newhotkey.onrender.com", # renderのドメイン
# ]
origins = os.getenv("ALLOWED_ORIGINS", "").split(",")

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
# ルータ登録
app.include_router(user_router.router)
app.include_router(logs_router.router)

# DB初期化
init_db()

# テスト
@app.get("/", tags=["Root"])
def root():
    return {"message": "API is running"}

# ローカル開発用
if __name__ == "__main__":
    import uvicorn
    uvicorn.run("app.main:app", host="127.0.0.1", port=8000, reload=True)
