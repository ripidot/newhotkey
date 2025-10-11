from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from sqlalchemy import func, cast, DateTime, select, asc, desc

from app.db.session import get_db
from app.models.keylog_model import KeyLog
from app.schemas.keylog_schema import Reqtest

router = APIRouter(prefix="/query", tags=["Query"])

@router.post("/postall")
def count_all(req: Reqtest, db: Session = Depends(get_db)):
    # 特殊カラムの定義
    special_columns = {
        "day": func.date_trunc('day', cast(KeyLog.timestamp, DateTime)).label("day"),
        "week": func.date_trunc('week', cast(KeyLog.timestamp, DateTime)).label("week"),
        "month": func.date_trunc('month', cast(KeyLog.timestamp, DateTime)).label("month"),
        "count": func.count().label("count"),
    }

    # SELECT句
    select_clauses = [
        special_columns[col] if col in special_columns else getattr(KeyLog, col)
        for col in req.select
    ]
    stmt = select(*select_clauses).select_from(KeyLog)

    # Aggregates
    if req.aggregates:
        for agg in req.aggregates:
            if agg.func.lower() == "count":
                expr = func.count()
            else:
                col = getattr(KeyLog, agg.field)
                expr = {
                    "sum": func.sum,
                    "avg": func.avg,
                    "max": func.max,
                    "min": func.min
                }[agg.func.lower()](col)
            stmt = stmt.add_columns(expr.label(agg.alias))

    # WHERE句
    if req.where:
        for field, value in req.where.items():
            stmt = stmt.where(getattr(KeyLog, field) == value)

    # GROUP BY
    if req.group_by:
        group_by_cols = [
            special_columns[col] if col in special_columns else getattr(KeyLog, col)
            for col in req.group_by
        ]
        stmt = stmt.group_by(*group_by_cols)

    # ORDER BY
    if req.order_by:
        for ob in req.order_by:
            col = special_columns[ob.field] if ob.field in special_columns else getattr(KeyLog, ob.field)
            stmt = stmt.order_by(asc(col) if ob.direction.lower() == "asc" else desc(col))

    # LIMIT
    if req.limit:
        stmt = stmt.limit(req.limit)

    # 実行
    results = db.execute(stmt).all()
    return {"results": [dict(row._mapping) for row in results]}