# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

def clear():
    db = KyotoTycoon()
    db = db.open()
    db.clear()
    db.close()

@raises(IOError)
def test_err_set():
    db = KyotoTycoon()
    db.set("A", "B")

@with_setup(setup=clear)
def test_set():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("A", "B")
    ok_(ret)
    ret = db.get("A")
    ok_(ret == "B")
    db.close()

@with_setup(setup=clear)
def test_set_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("あいうえお", "かきくけこ")
    ok_(ret) 
    ret = db.get("あいうえお")
    ok_(ret == "かきくけこ")
    db.close()

@with_setup(setup=clear)
def test_set_large_key():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("L" * 1024 * 4, "L")
    ok_(ret) 
    ret = db.get("L" * 1024 * 4)
    ok_(ret == "L")
    db.close()

@with_setup(setup=clear)
def test_set_large():
    db = KyotoTycoon()
    db = db.open()
    ret = db.set("L", "L" * 1024 * 1024 * 1)
    ok_(ret) 
    ret = db.get("L")
    ok_(ret == "L" * 1024 * 1024 * 1)
    db.close()

@with_setup(setup=clear)
def test_mapping_protocol():
    db = KyotoTycoon()
    db = db.open()
    ret = db["M"]  = "MAP"
    ok_(ret) 
    ret = db.get("M")
    ok_(ret == "MAP")
    db.close()

