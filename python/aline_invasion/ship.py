import pygame
class Ship:
    def __init__(self,ai_game):
        '''获取传入对象的surface'''
        self.screen = ai_game.screen
        '''获取传入对象的屏幕属性'''
        self.screen_rect = ai_game.screen.get_rect()
        '''加载飞船图像，并获取外接矩形，返回一个surface'''
        self.image = pygame.image.load('images/pangpang.bmp')
        '''获取飞船的属性'''
        self.rect = self.image.get_rect()
        '''每个新的飞船都放在屏幕中央的底部'''
        self.rect.midbottom = self.screen_rect.midbottom
        '''移动标志默认静止'''
        self.moving_right = False

    '''根据移动标志调整位置'''
    def updata(self):
        if self.moving_right:
            self.rect.x += 1
            
    def blitme(self):
        '''在指定位置上绘制飞船'''
        self.screen.blit(self.image,self.rect)
