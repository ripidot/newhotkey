"""change timestamp column to TIMESTAMP

Revision ID: e1e335f6f3fd
Revises: 6176604d0987
Create Date: 2025-08-13 23:43:08.871200

"""
from typing import Sequence, Union

from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision: str = 'e1e335f6f3fd'
down_revision: Union[str, Sequence[str], None] = '6176604d0987'
branch_labels: Union[str, Sequence[str], None] = None
depends_on: Union[str, Sequence[str], None] = None


def upgrade():
    # KeyLog.timestamp を文字列から TIMESTAMP 型に変換
    op.alter_column(
        'keylogs',  # テーブル名（複数形じゃないなら注意）
        'timestamp',
        type_=sa.TIMESTAMP(),
        postgresql_using='timestamp::timestamp'
    )

def downgrade():
    # 逆マイグレーション（TIMESTAMP → String）
    op.alter_column(
        'keylogs',
        'timestamp',
        type_=sa.String(),
        postgresql_using='timestamp::text'
    )