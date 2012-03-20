def offset_basis(s):
    FNV_prime = 0x1EF30EB
    r = 0
    for c in s:
        r = (r * FNV_prime) % (2**32)
        r ^=ord(c)
    return r
    
print hex(offset_basis("StealthRat"))
