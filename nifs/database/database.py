# -*- coding: utf-8
import psycopg2

VER = "$Id: database.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"

#
# データベース接続用パラメタ
#
DB = {"host": "egdb.lhd.nifs.ac.jp", "database": "db1", "user": "plasma", "password": "", "port": 5432}
# CONNECTION_STRING="egdb.lhd.nifs.ac.jp:db1:plasma::--client_encoding=UTF8:"


def __get_connection():
    """
    データベース接続用メソッド
    プラベート
    """
    #    db = pgdb.connect(host=DB['host'],database=DB['database'],user=DB['user'],password=DB['password'])
    #    db = pgdb.connect(dsn=CONNECTION_STRING)
    db = psycopg2.connect(host=DB["host"], database=DB["database"], user=DB["user"], password=DB["password"])
    return db


def get_current_shot_no():
    """
    [関数名] get_current_shot_no
    <引数> なし
    <返値>
      ショット番号 (int)
    <説明>
      データベースから最新ショット番号を検索し返す
    """
    db = __get_connection()
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


def find_val_in_explog(shotno):
    """
    [関数名] find_val_in_explog
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      explog2 から対応するショット番号のデータを取得する。
    """
    vals = {}
    db = __get_connection()
    cursor = db.cursor()
    sql = "SELECT * FROM explog2 WHERE nshotnumber = %d " % shotno
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
    """
    [関数名] find_val_in_exp_comment
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      exp_comment から対応するショット番号のデータを取得する。
    """
    vals = {}
    db = __get_connection()
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
    """
    [関数名] find_last_val_in_exp_comment
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      exp_comment から対応する直近のショット番号のデータを取得する。
    """
    vals = {}
    db = __get_connection()
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
    """
    [関数名] find_last_theme_in_exp_comment
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>

    説明
      exp_comment から対応する直近のショット番号の実験テーマを取得する。
    """
    vals = {}
    db = __get_connection()
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
    """
    [関数名] find_last_wallcondition_in_exp_comment
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>

    説明
      exp_comment から対応する直近のショット番号の壁状態を取得する。
    """
    vals = {}
    db = __get_connection()
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
    [関数名] find_val_in_lid_shotlog
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      seigyo.lid_shotlog から対応するショット番号のlidデータを取得する。
      LID計算機とのファイル共有の問題が解決されないままとなっているため、現状ではデータを取得できない。
      （しばらく舟場データを使用する）2013.10.21
    """
    vals = {}
    db = __get_connection()
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
    """
    [関数名] find_val_in_gas_shotlog
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      seigyo.gas_shotlog, seigyo.gas_name から対応するショット番号の gasデータを取得する。
    """
    vals = {}
    db = __get_connection()
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
    [関数名] find_val_in_shot_time
    <引数>
      shotno (int) :  ショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      explog2 から対応するショット番号のデータ作成時間を取得する。
      2011/10/06 t=0に変更 制御の seigyo.plc_shotlog l3ontimeとする
    """
    vals = {}
    db = __get_connection()
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
    """
    [関数名] set_last_theme_in_exp_comment
    <引数>
      shotno (int) :  ショット番号 (in)
      theme  (int) :  テーマ (in)
    <返値>

    説明
      指定されたショット番号に実験テーマを設定する。
    """
    db = __get_connection()
    cursor = db.cursor()
    # int_shotno = int(shotno)
    int_shotno = 125078
    cursor.execute("SELECT * FROM funaba.exp_comment WHERE shotno = %d", (int_shotno,))
    row = cursor.fetchone()
    if row is None:
        cursor.execute(
            "insert into funaba.exp_comment (shotno, subno, modified, theme)  VALUES ( %d, 1, 1, E%s )",
            (int_shotno, theme),
        )
    else:
        cursor.execute("update funaba.exp_comment set theme = E%s WHERE shotno = %d", (theme, int_shotno))

    db.commit()
    cursor.close()
    db.close()


def set_last_wallcondition_in_exp_comment(shotno, wall):
    """
    [関数名] set_last_wallcondition_in_exp_comment
    <引数>
      shotno (int) :  ショット番号 (in)
      wall   (int) :  壁の状態 (in)
    <返値>

    説明
      指定されたショット番号に壁状態を設定する。
    """
    db = __get_connection()
    cursor = db.cursor()
    int_shotno = int(shotno)
    cursor.execute("SELECT * FROM funaba.exp_comment WHERE shotno = %d", (int_shotno,))
    row = cursor.fetchone()
    if row is None:
        cursor.execute(
            "insert into funaba.exp_comment (shotno, subno, modified, wallcondition)  VALUES ( %d, 1, 1, E%s )",
            (int_shotno, wall),
        )
    else:
        cursor.execute("update funaba.exp_comment set wallcondition = E%s WHERE shotno = %d", (wall, int_shotno))

    db.commit()
    cursor.close()
    db.close()


def chk_exist(diag, shotno, subshot=1):
    """
    [関数名] chk_exist
    <引数>
      diag    (string) : diag name (in)
      shotno  (int)    :  ショット番号 (in)
      subshot (int)    :  サブショット番号 (in)
    <返値>
      カラムの値 辞書{'カラム名1':値1, 'カラム名2':値2, ...}
    説明
      seigyo.lid_shotlog から対応するショット番号のlidデータを取得する。
      LID計算機とのファイル共有の問題が解決されないままとなっているため、現状ではデータを取得できない。
      （しばらく舟場データを使用する）2013.10.21
    """
    status = False
    db = __get_connection()
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
