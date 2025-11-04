#  API 設計書

## 概要
キーの押下ログをDBに登録・検索するためのAPI群。  
CLIから送信されたキーログデータを受信し、フロントエンドからのリクエストにより検索を行う。

---

## エンドポイント一覧

| メソッド | エンドポイント | 機能 | 内容 |
|-----------|----------------|------|------|
| `POST` | `/logs/batch` | ログ登録 | CLIがキー押下ログを一括で送信し、DBに保存 |
| `POST` | `/logs/search` | ログ検索 | 検索条件(JSON)を受信し、該当ログを返す |

---

## 1. POST /logs/batch

### 概要
キー押下ログをDBに登録します。  
単数または複数のログをまとめて送信可能です。

#### Body例
```json
[
  {
    "id" : 1,
    "session_id" : 1_20250101T0000_1234,
    "sequence_id" : 1,
    "timestamp" : "2025-01-01 00:00:00", 
    "key" : "A",
    "modifiers" : "Shift",
    "window_title" : "GitHub",
    "process_name" : "Explorer.exe",
    "user_id": 1,
  },
  {
    "id" : 2,
    "session_id" : 1_20250101T0001_1234,
    "sequence_id" : 2,
    "timestamp" : "2025-01-01 00:01:00", 
    "key" : "B",
    "modifiers" : "",
    "window_title" : "GitHub",
    "process_name" : "Explorer.exe",
    "user_id": 1,
  },
  {
    "id" : 3,
    "session_id" : 1_20250101T0003_1234,
    "sequence_id" : 3,
    "timestamp" : "2025-01-01 00:03:00", 
    "key" : "B",
    "modifiers" : "",
    "window_title" : "GitHub",
    "process_name" : "Explorer.exe",
    "user_id": 1,
  }
]
```

#### 成功時
```json
{
  "status_code": "200",
  "content": [{
    "success_ids": [1, 2, 3],
    "failed": [],
    "success_count": 3,
    "failed_count": 0
  }]
}

```
#### エラー時
```json
{
  "status_code": "400",
  "content": [{
    "success_ids": [1],
    "failed": [
      {
        "id": 2,
        "reason": "Missing timestamp field."
      },
      {
        "id": 3,
        "reason": "Invalid key value: expected string, got null."
      }
    ],
    "success_count": 1,
    "failed_count": 2
  }]
}
```

## 2. POST /logs/search

### 概要
検索条件に合致するログを取得します。
期間・キー・ユーザIDなどの検索条件をJSONで指定します。
### リクエスト

**Content-Type:** `application/json`

#### Body例
```json
{
  "select": ["count"],
  "group_by": ["key"],
  "order_by": [{"field": "count", "direction": "desc"}],
  "limit": 3
}
```

#### 成功時
```json
{
  "success": True,
  "data": [
    {
      "count": 578614
    },
    {
      "count": 71312
    },
    {
      "count": 39616
    }
  ],
  "error": null
}

```
#### エラー時
```json
{
  "success": False,
  "data": null,
  "error": "invalid key value: expected string, got null."
}
```
### モデル構造
```python
from datetime import datetime
from typing import Optional, List
from pydantic import BaseModel

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

class Reqtest(BaseModel):
    select: List[str]
    where: Optional[dict] = None
    group_by: Optional[List[str]] = None
    aggregates: Optional[List[AggregateClause]] = None
    order_by: Optional[List[OrderByClause]] = None
    limit: Optional[int] = None
```

### データの流れ
```
CLI -> POST /logs/batch -> FastAPI -> DB(PostgreSQL)
Frontend -> POST /logs/search -> FastAPI 
  -> DB -> JSON Response -> Frontend
```
### 補足
/logs/batch は汎用的な登録用エンドポイントです。
/logs/search は検索クエリをapiから叩くためのエンドポイントです。

RESTの命名規則に従い、リソース名を複数形で統一しています。
サーバーの再起動時間を考慮して、取得できなかった場合は何度か繰り返す設計になっています。