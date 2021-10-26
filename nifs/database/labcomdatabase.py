# -*- coding: utf-8
import psycopg2

VER = "$Id: labcomdatabase.py,v 1.2 2020/07/15 02:59:48 yoshida Exp $"

#
# データベース接続用パラメタ
#
DB = {
    "host": "dasindex.lhd.nifs.ac.jp",
    "database": "index",
    "user": "pg_ro",
    "password": "20020122retrieve5",
    "port": 5432,
}
# CONNECTION_STRING="egdb.lhd.nifs.ac.jp:db1:plasma::--client_encoding=UTF8:"


def __get_labcomconnection():
    """
    LABCOMデータベース接続用メソッド
    プラベート
    """
    #    db = pgdb.connect(host=DB['host'],database=DB['database'],user=DB['user'],password=DB['password'])
    #    db = pgdb.connect(dsn=CONNECTION_STRING)
    db = psycopg2.connect(host=DB["host"], database=DB["database"], user=DB["user"], password=DB["password"])
    return db


def get_last_subshot_no(diag, shotno):
    """
    [関数名] get_last_subshot_no
    <引数>
      diag (int)   :  計測名 (in)
      shotno (int) :  ショット番号 (in)
    <返値>
      サブショット番号 (int)
    説明
      指定された計測名、ショット番号の最後のサブショット番号を取得する。
    """
    subshot = 1
    db = __get_labcomconnection()
    cursor = db.cursor()
    sql = (
        "select alias_arcshot, max(alias_subshot) as subshot from shot, diag where diag.diag_name = trim('%s') and shot.diag_id = diag.diag_id and shot.site_id=diag.site_id and alias_arcshot=%d group by alias_arcshot"
        % (diag, shotno)
    )
    # print sql
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


if __name__ == "__main__":
    import sys

    diag = sys.argv[1]
    shotno = sys.argv[2]
    subshot = get_last_subshot_no(diag, int(shotno))
    print(subshot)
