""" Piotr Słowik
    Programowanie Obiektowe - Projekt
    Alien Invaders - prosta gra na bazie Space Invaders
    game.py """


import entities
import sys
import level as lv
import pygame
import stats
from pygame.locals import *

#events
MOVEROWS = USEREVENT+1
ENEMYATTACK = USEREVENT+2


class game:
    """Class that handles the whole game"""
    
    def __init__(self, player):
        self.player = player
        self.level = lv.level()
        self.running = False
        self.clock = pygame.time.Clock()
        self.win_width = 860
        self.win_height = 700
        self.enemy_move_time = 300
        self.enemy_attack_time = 700
        self.running = False
        self.win = None
        self.stat = stats.statistics()

    def get_interactions(self):
        """Gets interactions from the user and events"""
    
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.display.quit()
                running = False
                sys.exit()
            if event.type == MOVEROWS:
                self.level.move_rows()
            if event.type == ENEMYATTACK:
                self.level.random_enemy_attack()

        keys = pygame.key.get_pressed()
        if keys[pygame.K_ESCAPE]:
            self.running = False
        if keys[pygame.K_a]:
            if self.player.x >= 10:
                self.player.move(-self.player.velocity,0)
        if keys[pygame.K_d]:
            if self.player.x <= 786:
                self.player.move(self.player.velocity,0)
        if keys[pygame.K_SPACE]:
            if self.player.bullet.is_fired == False:
                self.player.shoot()

    def init_game(self):
        """Game initialization, must be made before run()"""
    
        pygame.init()
        pygame.mouse.set_visible(0)
        self.win = pygame.display.set_mode((self.win_width,self.win_height))
        pygame.display.set_caption("Alien Invaders")
        self.running = True
        self.stat.start_game(self.win)
        pygame.display.update()

        while True:
            self.clock.tick(60)
            for event in pygame.event.get():
                keys = pygame.key.get_pressed()
                if event.type == pygame.QUIT or keys[pygame.K_ESCAPE]:
                    pygame.display.quit()
                    sys.exit()
                if keys[pygame.K_SPACE]:
                    return


    def run(self):
        """Makes the game run"""
        self.level.next_random_level()
        pygame.time.set_timer(MOVEROWS, self.enemy_move_time)
        pygame.time.set_timer(ENEMYATTACK, self.enemy_attack_time)

        #glowna petla gry
        while self.running:
            self.clock.tick(60)
            self.win.fill((0,0,0))
            self.get_interactions()
            if self.level.draw_level(self.win, self.player):
                break
            self.stat.update(self.win,self.player,self.level)
            if self.level.is_level_beaten():
                if self.level.current in [5,10,20]:
                    self.player.bullet.damage+=1
                if self.level.current % 2 == 0:
                    self.player.bullet.attack_speed+=2
                if self.level.current % 10 == 0:
                    self.player.score+=10000
                if self.enemy_move_time > 200:
                    self.enemy_move_time-=10
                if self.enemy_attack_time > 500:
                    self.enemy_attack_time*=0.9
                    self.enemy_attack_time = int(self.enemy_attack_time)
                self.run()
            pygame.display.update()
        self.win.fill((0,0,0))
        self.stat.end_game(self.win, self.player.score)
        pygame.display.update()
        while True:
            for event in pygame.event.get():
                keys = pygame.key.get_pressed()
                if event.type == pygame.QUIT or keys[pygame.K_ESCAPE]:
                    pygame.display.quit()
                    sys.exit()
                    
if __name__ == '__main__':
    g = game(entities.player("images/ship.png", 400, 550, 10, "images/shot.png"))
    g.init_game()
    g.run()
