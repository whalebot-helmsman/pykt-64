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
    ret = db.set("A", "B")
    db.close()
    assert ret 

def test_set_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("あいうえお", "かきくけこ")
    db.close()
    assert ret 

def test_set_large_key():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("L" * 1024 * 4, "L")
    db.close()
    assert ret 

def test_set_large():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("L", "L" * 1024 * 1024 * 5)
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
    assert ret == "B"

def test_get_notfound():
    db = KyotoTycoon()
    db = db.open()
    ret = db.get("A"* 10)
    db.close()
    assert ret == None

def test_get_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.get("あいうえお")
    db.close()
    assert ret == "かきくけこ"

def test_err_head():
    db = KyotoTycoon()
    try:
        ret = db.head("A")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_head():
    db = KyotoTycoon()
    db = db.open()
    ret = db.head("A")
    db.close()
    assert ret  == True

def test_head_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.head("あいうえお")
    db.close()
    assert ret == True

def test_head_notfound():
    db = KyotoTycoon()
    db = db.open()
    ret = db.head("A"* 10)
    db.close()
    assert ret == False



def test_err_remove():
    db = KyotoTycoon()
    try:
        ret = db.remove("A")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_remove():
    db = KyotoTycoon()
    db = db.open()
    ret = db.remove("A")
    val = db.get("A")
    db.close()
    assert ret == True
    assert val == None

def test_remove_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.remove("あいうえお")
    val = db.get("あいうえお")
    db.close()
    assert ret == True
    assert val == None

def test_remove_notfound():
    db = KyotoTycoon()
    db = db.open()
    ret = db.remove("A"* 10)
    db.close()
    assert ret == False

