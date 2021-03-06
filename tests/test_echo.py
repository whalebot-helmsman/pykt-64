# -*- coding: utf-8 -*-
from setup_teardown import start_db, stop_db
from nose.tools import *
from pykt import KyotoTycoon, KTException

@raises(IOError)
def test_err_echo():
    db = KyotoTycoon()
    db.echo()

@with_setup(setup=start_db,teardown=stop_db)
def test_echo():
    db = KyotoTycoon()
    db = db.open()
    ret = db.echo()
    ok_(ret == True)
    db.close()

@with_setup(setup=start_db,teardown=stop_db)
def test_echo_with_db():
    db = KyotoTycoon("test")
    db = db.open()
    ret = db.echo()
    ok_(ret == True)
    db.close()

@with_setup(setup=start_db,teardown=stop_db)
def test_echo_loop():
    db = KyotoTycoon()
    db = db.open()
    for i in xrange(100):
        ret = db.echo()
        ok_(ret == True)
    db.close()

