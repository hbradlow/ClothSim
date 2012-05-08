import os
import time
from subprocess import Popen
import dircache

def drange(start, stop, step):
        list = []
        r = start
        while r < stop:
            list.append(r)
            r += step
        return list

def createVideo(kb):
        list = dircache.listdir('.')
        i = 0
        while str('tmp'+str(i)) in list:
            i+=1
        dir = 'tmp' + str(i)
        os.system('mkdir ' + dir)
        print list

        p = Popen(['./light','-outputDir',dir,'-kb',str(kb)])
        time.sleep(120)
        p.terminate()

        list = dircache.listdir(dir)
        list =  sorted(list, key=lambda file: int(file[10:(file.index('.'))]))

        convert_string = "time convert -verbose -quality 100 -delay 0 "
        i = 0
        for file in list:
            i+=1
            if i%2 == 0:
                convert_string += dir + "/" + file + " "
        convert_string += " " + dir + "/outvideo.mpeg"

        os.system(convert_string)

createVideo(4)
#for i in drange(0.5,2.5,.6):
#        print i 
#        createVideo(i)
