# -*- coding: utf-8 -*-
from setup_teardown import start_db, stop_db
from nose.tools import *
from pykt import KyotoTycoon, KTException

@raises(IOError)
def test_err_status():
    db = KyotoTycoon()
    db.status()

@with_setup(setup=start_db,teardown=stop_db)
def test_status():
    db = KyotoTycoon()
    db = db.open()
    ret = db.status()
    ok_(ret)
    ok_(isinstance(ret, dict))
    db.close()

@with_setup(setup=start_db,teardown=stop_db)
def test_status_with_db():
    db = KyotoTycoon("test")
    db = db.open()
    ret = db.status()
    ok_(ret)
    ok_(isinstance(ret, dict))
    db.close()

@with_setup(setup=start_db,teardown=stop_db)
def test_status_loop():
    db = KyotoTycoon()
    db = db.open()
    for i in xrange(100):
        ret = db.status()
        ok_(ret)
    db.close()
