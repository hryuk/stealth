from collections import namedtuple
import hashlib

class Builder():
    def __init__(self):
        self.r = []
    def addByte(self, b):
        self.r+= int(b&0xFF),
    def addWord(self, w):
        self.addByte(w>>0)
        self.addByte(w>>8)
    def addDword(self, d):
        self.addWord(d>>0)
        self.addWord(d>>16)
    def addArray(self, a):
        for b in a:
            try:
                self.addByte(b)
            except:
                self.addByte(ord(b))
    def addBlob(self, b):
        self.addByte(b[0])
        self.addByte(b[1])
        self.addWord(b[2])
        self.addDword(b[3])
        self.addDword(b[4])
        self.addArray(b[5])
    def addString(self, s):
        s+="\0"
        self.addArray(s)
    def addTargets(self, ts):
        for t in ts:
            w, s = t
            self.addWord(w)
            self.addString(s)
    def padd(self, n=8):
        if (len(self.r)%8 != 0):
            for i in range(len(self.r)%8):
                self.r += 0x00,
    def XorCrypt(self, pw = [172, 121, 207, 209, 247, 23, 114, 177]):
        for i in range(len(self.r)):
            self.r[i] = self.r[i]^pw[i%len(pw)]
    def result(self):
        return self.r

PLAINTEXTKEYBLOB = 0x08
CUR_BLOB_VERSION = 0x02
CALG_AES_128     = 0x0000660E
aes128Blob = namedtuple("aes128Blob", "bType bVersion wReserved aiKeyAlg keySize keyData")

#sha1 = hashlib.sha1()
#sha1.update(PASSWORD)
