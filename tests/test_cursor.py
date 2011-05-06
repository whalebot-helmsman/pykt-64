# -*- coding: utf-8 -*-
from nose.tools import *
from pykt import KyotoTycoon, KTException, Cursor

d = dict(A="B", C="D", E="F", G="H")
d2 = {
    "あいうえお": "ABC", 
    "かきくけこ": "てすと2", 
    "さしすせそ": "てすと2", 
    }

def clear():
    db = KyotoTycoon()
    db = db.open()
    db.clear()
    db.close()

@nottest
@raises(IOError)
def test_cursor_err():
    db = KyotoTycoon()
    db.cursor()


#@with_setup(setup=clear)
def test_cursor():
    db = KyotoTycoon()
    db = db.open()
    db.set_bulk(d)
    c = db.cursor()
    ok_(isinstance(c, Cursor))

