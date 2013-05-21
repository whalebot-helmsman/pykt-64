# -*- coding: utf-8 -*-
from setup_teardown import start_db, stop_db
from nose.tools import *
from pykt import KyotoTycoon, KTException

@raises(IOError)
def test_err_clear():
    db = KyotoTycoon()
    ret = db.clear()

@with_setup(setup=start_db, teardown = stop_db)
def test_clear():
    db = KyotoTycoon()
    db = db.open()
    db.set("A", "B")
    ret = db.get("A")
    ok_(ret == "B")
    ret = db.clear()
    ok_(ret == True)
    ret = db.get("A")
    ok_(ret == None)
    db.close()

@with_setup(setup=start_db, teardown = stop_db)
def test_clear_with_db():
    db = KyotoTycoon("test")
    db = db.open()
    db.set("A", "B")
    ret = db.get("A")
    ok_(ret == "B")
    ret = db.clear()
    ok_(ret == True)
    ret = db.get("A")
    ok_(ret == None)
    db.close()

@with_setup(setup=start_db, teardown = stop_db)
def test_loop():
    db = KyotoTycoon()
    db = db.open()

    for i in xrange(100):
        db.set("A", "B")
        ret = db.get("A")
        ok_(ret == "B")
        ret = db.clear()
        ok_(ret == True)
        ret = db.get("A")
        ok_(ret == None)
    db.close()


