"""initial migration

Revision ID: 6176604d0987
Revises: 
Create Date: 2025-07-27 15:37:09.757803

"""
from typing import Sequence, Union

from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision: str = '6176604d0987'
down_revision: Union[str, Sequence[str], None] = None
branch_labels: Union[str, Sequence[str], None] = None
depends_on: Union[str, Sequence[str], None] = None


def upgrade() -> None:
    op.create_table(
        "keylogs",
        sa.Column("id", sa.Integer, primary_key=True, autoincrement=True),
        sa.Column("session_id", sa.String, index=True),
        sa.Column("sequence_id", sa.Integer),
        sa.Column("timestamp", sa.String),
        sa.Column("key", sa.String),
        sa.Column("modifiers", sa.String),
        sa.Column("window_title", sa.String),
        sa.Column("process_name", sa.String),
        sa.Column("user_id", sa.Integer, default=1),
    )


def downgrade() -> None:
    op.drop_table("keylogs")