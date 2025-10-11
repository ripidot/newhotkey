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

class SentFlagUpdate(BaseModel):
    ids: list[int]
    sent_flag: int

class FailedLog(BaseModel):
    id: int
    reason: str

class OrderByClause(BaseModel):
    field: str
    direction: str

class AggregateClause(BaseModel):
    func: str
    field: Optional[str] = None
    alias: str

class Reqtest(BaseModel):
    select: List[str]
    where: Optional[dict] = None
    group_by: Optional[List[str]] = None
    aggregates: Optional[List[AggregateClause]] = None
    order_by: Optional[List[OrderByClause]] = None
    limit: Optional[int] = None
