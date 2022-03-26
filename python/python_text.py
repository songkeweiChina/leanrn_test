import os
files = os.listdir('C:\\Users\\kewei.song\\Desktop\\git\\learning-text\\python\\text_files')
os.chdir('C:\\Users\\kewei.song\\Desktop\\git\\learning-text\\python\\text_files')
os.system('dir')
#print(files)
for filename in files:
    portion = os.path.splitext(filename)
    newname = portion[0] + ".c"
    #os.rename(filename,newname)
    print(newname)
















'''from fun import get_name
print('按q可以在任何时候退出')
while 1:
    X = input('\n输入姓')
    M = input('\n输入名')
    quan_ming = get_name(X,M)
    print(quan_ming)'''
'''file_name = 'C:\\Users\\windows\\Desktop\\learning-text\\python\\text_files'
with open(file_name + '\\rua.txt','a') as tmp:
    tmp.write("i wanna a girl\n")
    tmp.write("i wanna a girl who slap me\n")

a = 'i wanna a girl who slap me'
while 1:
    f = input("\n被除数：")
    if f == 'q':
        break
    s = input("\n除数：")
    if s == 'q':
        break
    try:
        q = int(f)/int(s)
    except ZeroDivisionError:
        pass #print('0不中啊，老弟')
    else:
        print(q)


'''
'''
class Car:
    def __init__(self,a,b,c):
        self.make = a
        self.model = b
        self.year = c
        self.odometer_reading = 600
        print('----------父类构造函数-----------')
    def get_descriptive(self):
        long_name = f"{self.year}{self.make}{self.model}"
        return long_name.title()
    def read_odometer(self):
        print(self.odometer_reading)
    def updata_odometer(self,d):
        if d > self.odometer_reading:
            self.odometer_reading = d
        else:
            print('里程表不能调哦')
    def increment_odometer(self,e):
        self.odometer_reading = self.odometer_reading + e
    def fill_gas_tank(self):
        print('超大500块钱油箱')


class Battery:
    def __init__(self,f):
        self.battery_size = f
    def describe_battery(self):
        print(f'电池大小：{self.battery_size}')






class ElectricCar(Car):
    def __init__(self,a,b,c):
        print('----------子类构造函数-----------')
        super().__init__(a,b,c)
        self.E_battery = Battery(75)
    def fill_gas_tank(self):
        print('电动车莫得油箱')



my_BYD = ElectricCar('比亚迪','汉EV',2022)
print(my_BYD.get_descriptive())
my_BYD.E_battery.describe_battery()
'''







































































