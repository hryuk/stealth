string = raw_input("Dame la lista de IDS:\n")

for API in string.split(") ("):
    API = API.replace("(","").replace(")","")
    print ("/*%s*/") % API,
    print "HASH_AND_EMIT(",
    for char in API:
        print ("('%s')") % char,
    print ")"
