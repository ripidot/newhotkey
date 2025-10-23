from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from sqlalchemy.exc import IntegrityError
from sqlalchemy import func, cast, DateTime, select, asc, desc

from fastapi.responses import JSONResponse

from app.db.session import get_db
from app.models.keylog_model import KeyLog
from app.schemas.keylog_schema import KeyLogCreate, FailedLog, SentFlagUpdate
from app.schemas.keylog_schema import Reqtest

router = APIRouter(prefix="/logs", tags=["Logs"])

@router.post("/")
def create_keylog(item: KeyLogCreate, db: Session = Depends(get_db)):
    log = KeyLog(**item.dict())
    db.add(log)
    db.commit()
    return

def make_batch_response(success_ids: list[int], failed: list[FailedLog]) -> JSONResponse:
    status_code = 200 if len(failed) == 0 else 400
    return JSONResponse(
        status_code=status_code,
        content={
            "success_ids": success_ids,
            "failed": [f.dict() for f in failed],
            "success_count": len(success_ids),
            "failed_count": len(failed),
        },
    )

@router.post("/batch")
def create_keylogs(logs: list[KeyLogCreate], db: Session = Depends(get_db)):
    success_ids = []
    failed: list[FailedLog] = []
    for log in logs:
        try:
            db_log = KeyLog(**log.dict())
            db.add(db_log)
            db.commit()
            db.refresh(db_log)
            success_ids.append(log.id)
        except IntegrityError:
            db.rollback()
            failed.append(FailedLog(id=log.id, reason="duplicate"))
        except Exception as e:
            db.rollback()
            failed.append(FailedLog(id=log.id, reason=str(e)))
    return make_batch_response(success_ids, failed)

@router.put("/update_sent_flag")
def update_sent_flag(data: SentFlagUpdate, db: Session = Depends(get_db)):
    db.query(KeyLog).filter(KeyLog.id.in_(data.ids)) \
        .update({"sent_flag": data.sent_flag}, synchronize_session=False)
    db.commit()
    return {"status": "success", "updated_count": len(data.ids)}

@router.post("/search")
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
    try: 
        data = db.execute(stmt).all()
    except Exception as e:
        return {"success": False, "data" : null, "error": str(e)}
    return {"success": True, "data": [dict(row._mapping) for row in data], "error": null}