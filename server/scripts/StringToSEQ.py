string = raw_input("Dame la cadena:\n")
for char in string:
    if char == '\\': char+="\\"
    print ("('%s')") % char,
print "(0)"
