# -*- coding: utf8 -*-
from sys import argv
from shutil import copyfile
import os, re

def save2dump(s):
    f = open("./dump","a")
    f.write(s)
    f.close()

if __name__ == "__main__":
    if argv[1] == "-clean":
        print "[kP] Limpiando y restaurando ficheros."

        for file_name in os.listdir("./"):
            root, ext = os.path.splitext(file_name)
            if ext == ".kbak":
                os.remove(os.path.join("./", root))
                os.rename(os.path.join("./", file_name), os.path.join("./", root))

        os.remove("./dump")
    else:
        file_path = argv[1]
        print "[kP] Trabajando sobre el fichero '%s'" % (file_path)
        #Backup
        copyfile(file_path, file_path+".kbak")
        #Read SRC
        with open(file_path, "r") as r:
            raw = r.read()
        #INCLUDE_PYSRC()
        for i in re.findall("(?<=INCLUDE_PYSRC\()(.*?)(?=\)\n)", raw, re.DOTALL):
            with open(os.path.abspath(i), 'r') as fi:
                save2dump(fi.read()+"\n")
        #DEFINE_PYSRC()
        r = re.findall("(?<=DEFINE_PYSRC\(\n)(.*?)(?=\)DEFINE_END\(\))", raw, re.DOTALL)
        if len(r)>0:
            save2dump(''.join(r) + "\n")
        #Load src
        try:
            exec open("./dump", "r")
        except IOError as e:
            pass
        #Resolve macros
        for fu in re.findall("(?<=#define)(.*?)(?=\(\.\.\.\) *PYTHON_FUNCTION\(\))", raw, re.DOTALL):
            raw = re.sub("("+fu+"\([^\.?].*?\)(?=[^\:]))", lambda m:str(eval(m.group(1))), raw)
        #Save file
        with open(file_path, "w+") as f:
            f.write(raw)
