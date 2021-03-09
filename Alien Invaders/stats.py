""" Piotr Słowik
    Programowanie Obiektowe - Projekt
    Alien Invaders - prosta gra na bazie Space Invaders
    stats.py """

import pygame
from level import enemy_dict

class statistics:
    """Class that holds the basic informations about the game"""
    def __init__(self):
        pygame.font.init()
        self.stat_rect = pygame.Rect(0,640,860,60)
        self.myfont = pygame.font.SysFont('Arial', 30)

    def update(self, window, player, level):
        """Updates the game statistics and draws them in target window"""
        text = self.myfont.render('Level:{0}   Damage:{1}   Attack speed:{2}   Hp:{3}   Score: {4}'.format(level.current, player.bullet.damage, player.bullet.attack_speed, player.hp, player.score), False, (255, 255, 255))
        pygame.draw.rect(window,(0,128,128),self.stat_rect,0)
        window.blit(text,(15,640))

    def end_game(self, window, score):
        """Draws the end game informations in target window"""
        s1 = self.myfont.size("Thanks for playing")[0]
        s2 = self.myfont.size("Your score is: {0}".format(score))[0]
        s3 = self.myfont.size("Press ESC to end the game")[0]
        text1 = self.myfont.render("Thanks for playing", False, (255, 255, 255))
        text2 = self.myfont.render("Your score is: {0}".format(score), False, (255, 255, 255))
        text3 = self.myfont.render("Press ESC to end the game", False, (255, 255, 255))
        window.blit(text1,((840-s1)/2,320))
        window.blit(text2,((840-s2)/2,350))
        window.blit(text3,((840-s3)/2,380))

    def start_game(self, window):
        """Draws the starting informations in target window"""
        text1 = "Alien invaders"
        text2 = "Made by Piotr Slowik"
        text3 = "Your goal is to survive as long as you can"
        text4 = "Press SPACE to start the game"
        text5 = "Enemies info:"

        rtext1 = self.myfont.render(text1, False, (255, 255, 255))
        rtext2 = self.myfont.render(text2, False, (255, 255, 255))
        rtext3 = self.myfont.render(text3, False, (255, 255, 255))
        rtext4 = self.myfont.render(text4, False, (255, 255, 255))
        rtext5 = self.myfont.render(text5, False, (255, 255, 255))

        hp1 = self.myfont.render("Hp: 2", False, (255, 255, 255))
        hp2 = self.myfont.render("Hp: 3", False, (255, 255, 255))
        hp3 = self.myfont.render("Hp: 5", False, (255, 255, 255))
        hp4 = self.myfont.render("Hp: 7", False, (255, 255, 255))
        hp5 = self.myfont.render("Hp: 10", False, (255, 255, 255))

        s1 = self.myfont.size(text1)[0]
        s2 = self.myfont.size(text2)[0]
        s3 = self.myfont.size(text3)[0]
        s4 = self.myfont.size(text4)[0]
        s5 = self.myfont.size(text5)[0]

        img1 = pygame.image.load(enemy_dict['1'][0])
        img2 = pygame.image.load(enemy_dict['2'][0])
        img3 = pygame.image.load(enemy_dict['3'][0])
        img4 = pygame.image.load(enemy_dict['4'][0])
        img5 = pygame.image.load(enemy_dict['5'][0])

        window.blit(rtext1,((840-s1)/2,80))
        window.blit(rtext2,((840-s2)/2,110))
        window.blit(rtext3,((840-s3)/2,140))
        window.blit(rtext4,((840-s4)/2,170))
        window.blit(rtext5,((840-s5)/2,200))

        window.blit(hp1,(410,298))
        window.blit(hp2,(410,372))
        window.blit(hp3,(410,446))
        window.blit(hp4,(410,520))
        window.blit(hp5,(410,594))

        window.blit(img1,(334,280))
        window.blit(img2,(334,354))
        window.blit(img3,(334,428))
        window.blit(img4,(334,502))
        window.blit(img5,(334,576))
