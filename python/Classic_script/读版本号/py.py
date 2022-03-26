'''with open('McuVersion.c') as a:
    str = a.read()
    str = str[407:541]
    str = '{' + str
    str1 = str[:85]
    str2 = ' ' + str[-45:]
    str = str1 + str2
    print(str)
    i = 0
    ok = ''
    while(i <= 23):
        #print(str[2 + 5 * i])
        i = i+1
        ok = ok + str[2 + 5 * i]
print(ok)'''

'''with open('McuVersion.c') as a:
    str = a.read()
str = str.replace(' ','')
str = str.replace('\n','')
print(str)
print(len(str))'''


