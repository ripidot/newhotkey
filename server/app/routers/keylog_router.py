from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from sqlalchemy.exc import IntegrityError
from fastapi.responses import JSONResponse

from app.db.session import get_db
from app.models.keylog_model import KeyLog
from app.schemas.keylog_schema import KeyLogCreate, FailedLog, SentFlagUpdate

router = APIRouter(prefix="/keylogs", tags=["KeyLogs"])

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
