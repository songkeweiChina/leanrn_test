import os
files = os.listdir('C:\\Users\\kewei.song\\Desktop\\git\\learning-text\\python\\text_files')
i = 1
i_str = '%d'%(i)
for filename in files:
    portion = os.path.splitext(filename)
    i_str = '%d'%(i)
    newname = i_str + portion[1]
    if portion[1] == '.c':
        os.rename(filename,newname)
    print(newname)
    i = i + 1



