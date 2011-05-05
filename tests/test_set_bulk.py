# -*- coding: utf-8 -*-
from pykt import KyotoTycoon, KTException

d = dict(A="B", C="D")
d2 = {
    "あいうえお":"1", 
    "かきくけこ":"1", 
    }

def test_err_set_bulk():
    db = KyotoTycoon()
    try:
        ret = db.set_bulk(d)
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_set():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set_bulk(d)
    assert ret == 2
    ret = db.get("A")
    assert ret == "B"
    ret = db.get("C")
    assert ret == "D"
    db.close()

def test_set_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set_bulk(d2)
    assert ret == 2
    db.close()

