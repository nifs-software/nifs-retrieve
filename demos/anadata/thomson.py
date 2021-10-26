#!/usr/bin/env python
# -*- coding: utf-8 -*-

from nifs.anadata import AnaData
import matplotlib.pyplot as plt


# トムソン のショット番号 8000, サブショット番号 1 のデータを取得
data = AnaData.retrieve("thomson", 80000, 1)

# 次元 'R' のデータを取得 (要素数137 の一次元配列が返ってくる)
r = data.getDimData("R")
r_unit = data.getDimUnit("R")


# 次元 'Time' のデータを取得 (要素数96 の一次元配列が返ってくる)
t = data.getDimData("Time")

# 変数 'Te' のデータを取得 (要素数 136x96 の二次元配列が返る)
Te = data.getValData("Te")

# 各時刻毎の R 対 Te のデータをプロット

plt.title("Thomson")
plt.xlabel("R [" + r_unit + "]")
plt.ylabel("Te")
labels = []

# 5 データ取得
for i in range(5):
    T = Te[i, :]  # R 次元でのスライスを取得
    plt.plot(r, T)
    label = "t = %05.3f (sec)" % (t[i] / 1000.0)  # 凡例用文字列
    labels.append(label)

plt.legend(labels)  # 凡例セット

plt.show()  # グラフ描画
