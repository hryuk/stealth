from itertools import combinations

comp_var = ['MUTEX','ERR_CHECK','SHELLCODE']

for i in range(0,len(comp_var)+1):
    for p in combinations(comp_var,i):
        print p
