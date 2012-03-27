from pprint import pprint

def hash(s):
    r = 0
    for x in s:
        r^=(ord(x)*ord(x))
        
ss = ["LoadLibraryA","GetProcAddress","Sleep","ExitProcess","LocalAlloc","CreateMutexA","WSASocketA","connect","WSAStartup","closesocket","send","inet_addr","gethostbyname","recv","CryptAcquireContextA","CryptSetKeyParam","CryptImportKey","CryptDecrypt"]

r = map(hash, ss)
print (len(r)==len(set(r)))
