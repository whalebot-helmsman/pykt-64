# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException

def clear():
    db = KyotoTycoon()
    db = db.open()
    db.clear()
    db.close()

@raises(IOError)
def test_err_add():
    db = KyotoTycoon()
    db.add("A", "B")

@with_setup(setup=clear)
def test_add():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("A", "B")
    ok_(ret)
    ret = db.get("A")
    ok_(ret == "B")
    db.close()

@with_setup(setup=clear)
def test_add_utf8():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("あいうえお", "かきくけこ")
    ok_(ret) 
    ret = db.get("あいうえお")
    ok_(ret == "かきくけこ")
    db.close()

@with_setup(setup=clear)
def test_add_large_key():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("L" * 1024 * 4, "L")
    ok_(ret) 
    ret = db.get("L" * 1024 * 4)
    ok_(ret == "L")
    db.close()

@with_setup(setup=clear)
def test_add_large():
    db = KyotoTycoon()
    db = db.open()
    ret = db.add("L", "L" * 1024 * 1024 * 1)
    ok_(ret) 
    ret = db.get("L")
    ok_(ret == "L" * 1024 * 1024 * 1)
    db.close()

