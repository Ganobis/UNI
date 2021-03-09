""" Piotr Słowik
    Programowanie Obiektowe - Projekt
    Alien Invaders - prosta gra na bazie Space Invaders
    level.py """

import entities
import pygame
import random

enemy_dict = {
    '1' : ["images/enemy1.png",2],
    '2' : ["images/enemy2.png",3],
    '3' : ["images/enemy3.png",5],
    '4' : ["images/enemy4.png",7],
    '5' : ["images/enemy5.png",10],
    }

class level:
    """Class that handles the game level drawing,
       collisions, loading level from file"""
    def __init__(self):
        self.current = 0
        self.rows = 3
        self.cols = 5
        self.enemy_List = []
        self.time = 0

    def load_level(self, path):
        """Loads level from path"""
        file = open(path,"r")
        file_line = file.readline()
        self.current = int(file_line.split()[0])
        file_line = file.readline()
        file_line = file_line.split()
        self.rows = int(file_line[0])
        self.cols = int(file_line[1])
        self.enemy_List = []
        for i in range(self.rows):
            self.enemy_List.append(file.readline().split())

    def make_proper_enemy_List(self):
        """Makes enemy objects list from list of strings"""
        width = [0] * self.rows
        width_sum = 0
        for i in range(self.rows):
            for j in range(self.cols):
                img = pygame.image.load(enemy_dict[self.enemy_List[i][j]][0])
                width_sum+=img.get_size()[0]
            width[i] = width_sum
            width_sum = 0
        h_interval = 410/(self.rows+1)

        for i in range(self.rows):
            w_interval = (860 - width[i])/(self.cols+1)
            for j in range(self.cols):
                x =(1+j)*w_interval + j*64
                y =(1+i)*h_interval
                print(self.enemy_List[i][j])
                img = enemy_dict[self.enemy_List[i][j]][0]
                hp = enemy_dict[self.enemy_List[i][j]][1]
                self.enemy_List[i][j] = entities.enemy(img, x, y, hp)


    def is_level_beaten(self):
        """Checks if the level is beaten"""
        for i in range(self.rows):
            for j in range(self.cols):
                if self.enemy_List[i][j].is_destroyed == False:
                     return False
        return True

    def move_row(self,row):
        """Moves the rows randomly"""
        x = random.choice([-5,5])
        if x == -5 and self.enemy_List[row][0].x <=5:
            return
        if x == 5 and self.enemy_List[row][self.cols-1].x >=796:
            return
        for e in self.enemy_List[row]:
            e.move(x,0)

    def random_enemy_attack(self):
        """Makes random enemy attack"""
        i = random.choice(range(self.rows))
        j = random.choice(range(self.cols))
        self.enemy_List[i][j].shoot()

    def move_rows(self):
        """Moves every enemy row"""
        for i in range(self.rows):
            self.move_row(i)

    def next_random_level(self):
        """Generates random level"""
        self.rows = random.choice([3,4,5,6])
        self.cols = random.choice([5,6,7,8])
        self.enemy_List = [[()] * self.cols for x in range(self.rows)]
        for i in range(self.rows):
            for j in range(self.cols):
                self.enemy_List[i][j] = random.choice(['1','2','3','4','5'])
        self.current+=1
        self.make_proper_enemy_List()

    def draw_level(self, window, player):
        """Draws whole level, enemies, player in target window"""
        player.bullet.move_shot()
        if player.bullet.is_fired:
            player.bullet.draw(window)
        for i in range(self.rows):
            for j in range(self.cols):
                if self.enemy_List[i][j].bullet.is_fired:
                    self.enemy_List[i][j].bullet.move_shot()
                    if self.enemy_List[i][j].bullet.get_collision(player.img.get_rect(topleft=player.get_xy())):
                        player.hp -= self.enemy_List[i][j].bullet.damage
                        self.enemy_List[i][j].bullet.is_fired = False
                        if player.hp <= 0:
                            return True
                    self.enemy_List[i][j].bullet.draw(window)
                if player.bullet.is_fired:
                    if player.bullet.get_collision(self.enemy_List[i][j].img.get_rect(topleft=self.enemy_List[i][j].get_xy())) and self.enemy_List[i][j].is_destroyed == False:
                        self.enemy_List[i][j].hp -= player.bullet.damage
                        player.score+=20
                        if self.enemy_List[i][j].hp <= 0:
                            self.enemy_List[i][j].is_destroyed = True
                            player.score+=100
                        player.bullet.is_fired = False
                        player.bullet.y = 0
                if self.enemy_List[i][j].is_destroyed == False:
                    self.enemy_List[i][j].draw(window)
        player.draw(window)
        return False
