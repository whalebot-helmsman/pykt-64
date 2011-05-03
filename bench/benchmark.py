# -*- coding: utf-8 -*-
from pykt import KyotoTycoon as kt1
from kyototycoon import KyotoTycoon as kt2
import timeit

key = "A" *128
val = "B" * 1024

def pykt_get():
    db = kt1()
    db.open()
    print db.get(key)
    db.close()

def kyoto_get():
    db = kt2()
    db.open()
    print db.get(key)
    db.close()

if __name__ == "__main__":
    res = timeit.timeit("pykt_get()", "from __main__ import pykt_get", number=10)
    print(res)
    res = timeit.timeit("kyoto_get()", "from __main__ import kyoto_get", number=10)
    print(res)

