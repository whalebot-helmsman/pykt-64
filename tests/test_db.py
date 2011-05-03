# -*- coding: utf-8 -*-
from pykt import KyotoTycoon, KtException

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


def test_err_report():
    db = KyotoTycoon()
    try:
        ret = db.report()
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_report():
    db = KyotoTycoon()
    db = db.open()
    ret = db.report()
    db.close()
    assert ret 

def test_err_status():
    db = KyotoTycoon()
    try:
        ret = db.status()
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_status():
    db = KyotoTycoon()
    db = db.open()
    ret = db.status()
    db.close()
    assert ret 

def test_status_db():
    db = KyotoTycoon()
    db = db.open()
    ret = db.status("db_0")
    db.close()
    assert ret 

def test_err_clear():
    db = KyotoTycoon()
    try:
        ret = db.clear()
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_clear():
    db = KyotoTycoon()
    db = db.open()
    ret = db.clear()
    db.close()
    assert ret 

def test_clear_db():
    db = KyotoTycoon()
    db = db.open()
    ret = db.clear("db_0")
    db.close()
    assert ret 

def test_err_increment():
    db = KyotoTycoon()
    try:
        ret = db.increment("I")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_increment():
    db = KyotoTycoon()
    db = db.open()
    ret = db.increment("I")
    db.close()
    assert ret == 1

def test_increment_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.increment("インクリメント")
    db.close()
    assert ret == 1

def test_increment_arg():
    db = KyotoTycoon()
    db = db.open()
    ret = db.increment("I", 100)
    db.close()
    assert ret == 101

def test_err_add():
    db = KyotoTycoon()
    try:
        ret = db.add("A", "B")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_add():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("A", "B")
    db.close()
    assert ret 

def test_add_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("あいうえお", "かきくけこ")
    db.close()
    assert ret 

def test_add_large_key():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("L" * 1024 * 4 , "L")
    db.close()
    assert ret 

def test_add_large():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("L", "L" * 1024 * 1024 * 5)
    db.close()
    assert ret 

def test_err_increment_double():
    db = KyotoTycoon()
    try:
        ret = db.increment_double("ID")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_increment_double():
    db = KyotoTycoon()
    db = db.open()
    ret = db.increment_double("ID")
    db.close()
    assert ret == 1.0

def test_increment_double_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.increment_double("インクリメントd")
    db.close()
    assert ret == 1.0

def test_increment_double_arg():
    db = KyotoTycoon()
    db = db.open()
    ret = db.increment_double("ID", 100.1)
    print ret
    db.close()
    assert ret == 101.1

def test_err_cas():
    db = KyotoTycoon()
    try:
        ret = db.cas("A")
        assert False
    except IOError:
        assert True
    except:
        assert False

def test_cas():
    db = KyotoTycoon()
    db = db.open()
    ret = db.cas("A", oval="B", nval="C")
    db.close()
    assert ret == True

def test_cas_few_param1():
    db = KyotoTycoon()
    db = db.open()
    try:
        ret = db.cas("A", nval="C")
        assert False
    except KtException:
        assert True
    except:
        assert False
    finally:
        db.close()

def test_cas_few_param2():
    db = KyotoTycoon()
    db = db.open()
    ret = db.cas("A", oval="C")
    db.close()
    assert ret == True
