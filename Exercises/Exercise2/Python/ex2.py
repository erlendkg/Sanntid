# Python 3.3.3 and 2.7.6
# python helloworld_python.py

from threading import Thread
import threading

i = 0
lock = threading.Lock()

def someThreadFunction1():


# Potentially useful thing:
#   In Python you "import" a global variable, instead of "export"ing it when you declare it
#   (This is probably an effort to make you feel bad about typing the word "global")
    global i


    for x in xrange(1,10):
        lock.aquire()
        try:
            i = i + 1
        finally:
            lock.release()

def someThreadFunction2():


# Potentially useful thing:
#   In Python you "import" a global variable, instead of "export"ing it when you declare it
#   (This is probably an effort to make you feel bad about typing the word "global")
    global i


    for x in xrange(1,10):
        lock.aquire()
        try:
            i = i - 1
        finally:
            lock.release()



def main():
    someThread1 = Thread(target = someThreadFunction1, args = (),)
    someThread1.start()

    someThread2 = Thread(target = someThreadFunction2, args = (),)
    someThread2.start()

    someThread1.join()
    someThread2.join()

    print(i);


main()
