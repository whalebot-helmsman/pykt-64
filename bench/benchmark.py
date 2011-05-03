# -*- coding: utf-8 -*-
from pykt import KyotoTycoon as kt1
from kyototycoon import KyotoTycoon as kt2
import timeit

key = "A" * 128
val = "B" * 1024 * 4

def pykt_set():
    db = kt1()
    db.open()
    ret = db.set(key, val)
    assert ret == True
    db.close()

def kyoto_set():
    db = kt2()
    db.open()
    ret = db.set(key, val)
    assert ret == True
    db.close()

def pykt_get():
    db = kt1()
    db.open()
    ret = db.get(key)
    assert ret == val
    db.close()

def kyoto_get():
    db = kt2()
    db.open()
    ret = db.get(key)
    assert ret == val
    db.close()

def pykt_gets():
    db = kt1()
    db.open()
    
    for i in xrange(10):
        ret = db.get(key)
        assert ret == val
    db.close()

def kyoto_gets():
    db = kt2()
    db.open()
    
    for i in xrange(10):
        ret = db.get(key)
        assert ret == val
    db.close()

if __name__ == "__main__":
    res = timeit.timeit("pykt_set()", "from __main__ import pykt_set",
            number=1000)
    print "pykt_set %f" % res
    res = timeit.timeit("kyoto_set()", "from __main__ import kyoto_set",
            number=1000)
    print "kt_set %f" % res
    res = timeit.timeit("pykt_get()", "from __main__ import pykt_get",
            number=1000)
    print "pykt_get %f" % res
    res = timeit.timeit("kyoto_get()", "from __main__ import kyoto_get",
            number=1000)
    print "kt_get %f" % res

    res = timeit.timeit("pykt_gets()", "from __main__ import pykt_gets",
            number=100)
    print "pykt_gets %f" % res
    res = timeit.timeit("kyoto_gets()", "from __main__ import kyoto_gets",
            number=100)
    print "kt_gets %f" % res


