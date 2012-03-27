def offset_basis(s):
    FNV_prime = 0x1EF30EB
    r = 0
    for c in s:
        r = (r * FNV_prime) % (2**32)
        r ^=ord(c)
    return r

def FNV1a(s,d):
    FNV_prime = 0x1EF30EB
    r = d
    for c in s:
        r ^= ord(c)
        r = (r * FNV_prime) % (2**32)
    return r

a = offset_basis("zaa")
print hex(a)
print hex(FNV1a("zaa",a))
