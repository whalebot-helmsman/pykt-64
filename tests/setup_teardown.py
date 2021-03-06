from pykt import KyotoTycoon
import subprocess
import time

_DB_Process =   None

def start_db():
    _EXECUTABLE     =   'ktserver'
    _IN_MEMORY_TREE =   '+'
    _PORT           =   '1978'
    args            =   [ _EXECUTABLE, '-port', _PORT, '-log', '/tmp/pykt_tests', _IN_MEMORY_TREE ]
    global _DB_Process
    _DB_Process     =   subprocess.Popen(args)
    time.sleep(0.6)
    status  =   _DB_Process.poll()
    if status is not None:
        _DB_Process =   None
        raise ValueError('cannot start server, someone already bind socket for port %s. That may cause data trashing' % (_PORT, ))

def stop_db():
    global _DB_Process
    _DB_Process.terminate()
    _DB_Process.wait()
    _DB_Process =   None


def clear():
    start_db()
    db = KyotoTycoon()
    db = db.open()
    db.clear()
    db.close()
