# -*- coding: utf-8
import os.path as path
import json
import psycopg2

VER = "$Id: labcomdatabase.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"
BASE = path.dirname(__file__)


def get_labcomconnection():
    """Establish a new connection to LABCOM.

    Returns
    -------
    psycopg2.extensions.connection
        connection object
    """
    # Load database config
    with open(path.join(BASE, "setting", "labcom.json"), "r") as f:
        config = json.load(f)
    try:
        db = psycopg2.connect(**config)
        return db
    except Exception:
        raise ConnectionError("Failed to establish a new connection to LABCOM.")


def get_last_subshot_no(diag: str, shotno: int):
    """指定された計測名、ショット番号の最後のサブショット番号を取得する。

    Parameters
    ----------
    diag : int
        name of diagnostics
    shotno : int
        shot number

    Returns
    -------
    int
        sub-shot number
    """
    subshot = 1
    db = get_labcomconnection()
    cursor = db.cursor()
    sql = f"""
SELECT
    alias_arcshot,
    max(alias_subshot) as subshot
FROM
    shot, diag
WHERE
    diag.diag_name = trim({diag}) and
    shot.diag_id = diag.diag_id and
    shot.site_id=diag.site_id and
    alias_arcshot={shotno}
GROUP BY
    alias_arcshot
"""
    try:
        cursor.execute(sql)
        while 1:
            row = cursor.fetchone()
            if not row:
                break
            if 2 == len(row):
                subshot = row[1]
        cursor.close()
        db.close()
        return subshot
    except Exception:
        raise RuntimeError("Failed to excute the SQL statement.")


if __name__ == "__main__":
    import sys

    diag = sys.argv[1]
    shotno = sys.argv[2]
    subshot = get_last_subshot_no(diag, int(shotno))
    print(subshot)
