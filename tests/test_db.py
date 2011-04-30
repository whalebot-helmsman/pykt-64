# -*- coding: utf-8 -*-
from pykt import KyotoTycoon

def test_init():
    db = KyotoTycoon()
    assert(db != None)

def test_open():
    db = KyotoTycoon()
    db = db.open()
    assert db != None

def test_close():
    db = KyotoTycoon()
    db = db.open()
    ret = db.close()
    assert ret 

def test_non_connect_close():
    db = KyotoTycoon()
    ret = db.close()
    assert ret == False

def test_err_echo():
    db = KyotoTycoon()
    try:
        ret = db.echo()
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_echo():
    db = KyotoTycoon()
    db = db.open()
    ret = db.echo()
    db.close()
    assert ret 

def test_err_set():
    db = KyotoTycoon()
    try:
        ret = db.set("A", "B")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_set():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("A", "B" * 100)
    db.close()
    assert ret 

def test_set_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("あいうえお", "かきくけこ")
    db.close()
    assert ret 

def test_err_get():
    db = KyotoTycoon()
    try:
        ret = db.get("A")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_get():
    db = KyotoTycoon()
    db = db.open()
    ret = db.get("A")
    db.close()
    assert ret

def test_get_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.get("あいうえお")
    db.close()
    assert ret

