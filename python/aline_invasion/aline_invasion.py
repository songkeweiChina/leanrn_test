import pygame
import sys
from settings import Settings
from ship import Ship
class Alinevasion:
    def __init__(self):
        '''初始化背景设置'''
        pygame.init()
        self.own_set = Settings()
        '''新建窗口，set_mode返回的是一个surface，既整个窗口'''
        self.screen = pygame.display.set_mode((self.own_set.screen_width,self.own_set.screen_hight))
        '''设置窗口标题'''
        pygame.display.set_caption('Alien Invasion')
        self.own_ship = Ship(self)

    def run_game(self):
        while 1:
            '''相应按键和鼠标事件'''
            self._check_events()
            self.own_ship.updata()
            '''更新屏幕图像'''
            self._update_screen()
            

    def _check_events(self):
        '''pygame.event.get()返回一个列表，包含上次被调用后发生的所有事件，包括所有的键盘和鼠标事件'''
        for event in pygame.event.get():
            '''当关闭窗口，检测到关闭事件'''
            if event.type == pygame.QUIT:
                '''退出游戏'''
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RIGHT:
                    self.own_ship.moving_right = True
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_RIGHT:
                    self.own_ship.moving_right = False

    def _update_screen(self):
        '''填充屏幕颜色'''
        self.screen.fill(self.own_set.bg_color)
        '''在指定位置绘制飞船'''
        self.own_ship.blitme()
        '''让刚绘制的屏幕可见'''
        pygame.display.flip()

if __name__ == '__main__':
    print('----------运行main函数-----------')
    ai = Alinevasion()
    ai.run_game()
    print('----------main函数运行结束-------')






