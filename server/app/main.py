from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from app.db.session import init_db
from app.routers import user_router, keylog_router, query_router

app = FastAPI()

# CORS設定
origins = [
    "http://localhost:3000",                 # 開発用
    "https://newhotkey-8brf.vercel.app",     # 本番用（Vercelのドメイン）
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
# ルータ登録
app.include_router(user_router.router)
app.include_router(keylog_router.router)
app.include_router(query_router.router)

# DB初期化
init_db()

# ローカル開発用
if __name__ == "__main__":
    import uvicorn
    uvicorn.run("app.main:app", host="127.0.0.1", port=8000, reload=True)
