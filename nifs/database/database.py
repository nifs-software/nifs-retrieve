# -*- coding: utf-8
import os.path as path
import json
import psycopg2

VER = "$Id: database.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"
BASE = path.dirname(__file__)


def get_connection():
    """Establish a new connection to Kaiseki-server.

    Returns
    -------
    psycopg2.extensions.connection
        connection object
    """
    # Load database config
    with open(path.join(BASE, "setting", "kaiseki.json"), "r") as f:
        config = json.load(f)
    try:
        db = psycopg2.connect(**config)
        return db
    except Exception:
        ConnectionError("Failed to establish a new connection")


def get_current_shot_no():
    """データベースから最新ショット番号を検索し返す

    Returns
    -------
    int
        latest shot number
    """
    db = get_connection()
    cursor = db.cursor()
    cursor.execute("SELECT no FROM current_shot")
    no = None
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        no = row[0]

    cursor.close()
    db.close()
    return no


def find_val_in_explog(shotno: int):
    """explog2 から対応するショット番号のデータを取得する。

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    sql = f"SELECT * FROM explog2 WHERE nshotnumber = {shotno}"
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_val_in_exp_comment(shotno):
    """exp_comment から対応するショット番号のデータを取得する。

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    --------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    sql = "SELECT * FROM funaba.exp_comment WHERE shotno = %d " % shotno
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_last_val_in_exp_comment(shotno):
    """exp_comment から対応する直近のショット番号のデータを取得する。

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    sql = "SELECT * FROM funaba.exp_comment WHERE shotno <= %d order by shotno desc limit 1" % shotno
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_last_theme_in_exp_comment(shotno):
    """exp_comment から対応する直近のショット番号の実験テーマを取得する。

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    sql = (
        "SELECT shotno, theme FROM funaba.exp_comment WHERE theme is not null and shotno <= %d order by shotno desc limit 1"
        % shotno
    )
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_last_wallcondition_in_exp_comment(shotno):
    """exp_comment から対応する直近のショット番号の壁状態を取得する。

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    sql = (
        "SELECT shotno, wallcondition FROM funaba.exp_comment WHERE wallcondition is not null and shotno <= %d order by shotno desc limit 1"
        % shotno
    )
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_val_in_lid_shotlog(shotno):
    """
    seigyo.lid_shotlog から対応するショット番号のlidデータを取得する。
    LID計算機とのファイル共有の問題が解決されないままとなっているため、現状ではデータを取得できない。
    しばらく舟場データを使用する）2013.10.21

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    説明
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    # sql = "SELECT * FROM seigyo.lid_shotlog, seigyo.lid_point WHERE seigyo.lid_shotlog.shotno= %d AND seigyo.lid_shotlog.point_id =  seigyo.lid_point.point_id " % shotno
    sql = (
        "select ilid1 as current1, ilid2 as current2, ilid3 as current3, CASE WHEN ilidpoint=0 then '7O' WHEN ilidpoint=1 THEN '6O' end as point_name from lid where shotno=%d"
        % shotno
    )
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_val_in_gas_shotlog(shotno):
    """seigyo.gas_shotlog, seigyo.gas_name から対応するショット番号の gasデータを取得する。

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    sql = (
        "SELECT distinct gas_name.gas_name as gas_name FROM seigyo.gas_shotlog gas_shotlog, seigyo.gas_name gas_name  WHERE gas_shotlog.shotno=%d and gas_name.gas_name_id=gas_shotlog.gas_name_id "
        % shotno
    )
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            if vals is None:
                vals[names[no]] = row[no]
            else:
                if names[no] in vals:
                    vals[names[no]] += ", "
                    vals[names[no]] += row[no]
                else:
                    vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def find_val_in_shot_time(shotno):
    """
    explog2 から対応するショット番号のデータ作成時間を取得する。
    2011/10/06 t=0に変更 制御の seigyo.plc_shotlog l3ontimeとする

    Parameters
    ----------
    shotno : int
        shot number

    Returns
    -------
    dict
    """
    vals = {}
    db = get_connection()
    cursor = db.cursor()
    #    sql = "SELECT ddatacreationtime FROM explog2 WHERE nshotnumber = %d " % shotno
    sql = "SELECT l3ontime as ddatacreationtime FROM seigyo.plc_shotlog WHERE shotno = %d " % shotno
    cursor.execute(sql)
    while 1:
        row = cursor.fetchone()
        if not row:
            break
        names = [desc[0] for desc in cursor.description]
        for no in range(len(names)):
            vals[names[no]] = row[no]

    cursor.close()
    db.close()
    return vals


def set_last_theme_in_exp_comment(shotno, theme):
    """指定されたショット番号に実験テーマを設定する。

    Parameters
    ----------
    shotno : int
        shot number
    theme : str
        experiment theme description
    """
    db = get_connection()
    cursor = db.cursor()
    cursor.execute("SELECT * FROM funaba.exp_comment WHERE shotno = %d", (shotno,))
    row = cursor.fetchone()
    if row is None:
        cursor.execute(
            "insert into funaba.exp_comment (shotno, subno, modified, theme)  VALUES ( %d, 1, 1, E%s )",
            (shotno, theme),
        )
    else:
        cursor.execute("update funaba.exp_comment set theme = E%s WHERE shotno = %d", (theme, shotno))

    db.commit()
    cursor.close()
    db.close()


def set_last_wallcondition_in_exp_comment(shotno: int, wall: str):
    """指定されたショット番号に壁状態を設定する。

    Parameters
    ----------
    shotno : int
        shot number
    wall : str
        description of wall condition
    """
    db = get_connection()
    cursor = db.cursor()
    cursor.execute("SELECT * FROM funaba.exp_comment WHERE shotno = %d", (shotno,))
    row = cursor.fetchone()
    if row is None:
        cursor.execute(
            "insert into funaba.exp_comment (shotno, subno, modified, wallcondition)  VALUES ( %d, 1, 1, E%s )",
            (shotno, wall),
        )
    else:
        cursor.execute("update funaba.exp_comment set wallcondition = E%s WHERE shotno = %d", (wall, shotno))

    db.commit()
    cursor.close()
    db.close()


def chk_exist(diag, shotno, subshot=1):
    """
    seigyo.lid_shotlog から対応するショット番号のlidデータを取得する。
    LID計算機とのファイル共有の問題が解決されないままとなっているため、現状ではデータを取得できない。
    (しばらく舟場データを使用する 2013.10.21)

    Parameters
    ----------
    diag :str
        diagnostics name
    shotno : int
        shot number
    subshot: int
        sub-shot number

    Returns
    -------
    dict
    """
    status = False
    db = get_connection()
    cursor = db.cursor()
    sql = "select shotno from _locationfileinfo_ where shotno=%d and subshotno=%d and diagnostics='%s'" % (
        shotno,
        subshot,
        diag,
    )
    cursor.execute(sql)
    row = cursor.fetchone()
    if row is None:
        status = False
    else:
        status = True
    cursor.close()
    db.close()
    return status


if __name__ == "__main__":
    import sys

    shotno = sys.argv[1]
    vals = find_last_val_in_exp_comment(int(shotno))
    print((vals["comment"]))
    vals = find_last_theme_in_exp_comment(int(shotno))
    print((vals["shotno"], vals["theme"]))
