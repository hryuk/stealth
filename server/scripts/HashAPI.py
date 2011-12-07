from pprint import pprint
import itertools
def hash(s):
    r = 0
    for x in s:
        r>>=1
        r^=(ord(x)<<7)
    return r

ss = ["LoadLibraryA","ExitProcess","VirtualAlloc","VirtualFree","WSASocketA","connect","WSAStartup","closesocket","send","inet_addr","gethostbyname","recv","CallWindowProcA","CallWindowProcW"]

pprint(zip(map(hex,map(hash,ss)),ss))

r = map(hash,ss)
print (len(r)!=len(set(r)))
