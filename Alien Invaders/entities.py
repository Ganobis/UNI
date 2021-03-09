""" Piotr Słowik
    Programowanie Obiektowe - Projekt
    Alien Invaders - prosta gra na bazie Space Invaders
    entities.py """

import pygame

class entity:
    """Entity class, holds entity image, size and x,y cordinates"""

    def __init__(self, img_src, x, y):
        self.img = pygame.image.load(img_src)
        self.size = self.img.get_size()
        self.x = x
        self.y = y

    def draw(self, target_window):
        """Draws the entity in target window"""
        target_window.blit(self.img ,(self.x,self.y))

    def move(self, x_diff, y_diff):
        """Moves the entity"""
        self.x += x_diff
        self.y += y_diff

    def get_xy(self):
        """Gets the (x,y) coordinate"""
        return (self.x, self.y)

class bullet(entity):
    """Bullet class that derives from entity"""
    def __init__(self, img_src, x, y, attack_speed, damage, direction):
        super().__init__(img_src, x, y)
        self.attack_speed = attack_speed
        self.is_fired = False
        self.direction = direction
        self.damage = damage
        pygame.mixer.init()
        self.sound = pygame.mixer.Sound("sounds/skorpion.wav")

    def make_shoot_sound(self):
        """Makes shoot sound"""
        self.sound.play()

    def move_shot(self):
        """Moves the bullet"""
        if self.y >= 0 and self.direction == "Up":
            self.y -= self.attack_speed
        elif self.y >= 0 and self.direction == "Down":
            if self.y >= 630:
                self.is_fired = False
                self.y = 9000
                return
            self.y += self.attack_speed
        else:
            self.is_fired = False
            self.y = 0

    def get_collision(self, rect):
        """Gets collision of bullet and rectangle"""
        return self.img.get_rect(topleft=self.get_xy()).colliderect(rect)


class player(entity):
    """Player class that derives from entity"""
    def __init__(self, img_src, x, y, velocity, bullet_img_src):
        super().__init__(img_src, x, y)
        self.velocity = velocity
        self.hp = 3
        self.bullet = bullet(bullet_img_src, 0, 2, 10, 2, "Up")
        self.score = 0

    def get_shoot_xy(self):
        """Gets player shoot x,y coordinates"""
        return (self.bullet.x ,self.bullet.y)

    def shoot(self):
        """Makes player shoot"""
        self.bullet.x = self.x+29.5
        self.bullet.y = self.y
        self.bullet.is_fired = True
        self.bullet.make_shoot_sound()

    def get_collision(self, rect):
        """Gets collision of the player and rectangle"""
        return self.img.get_rect(topleft=self.get_xy()).colliderect(rect)


class enemy(entity):
    """Enemy class that derives from entity"""
    def __init__(self, img_src, x, y, hp):
        super().__init__(img_src, x, y)
        self.hp = hp
        self.is_destroyed = False
        self.bullet = bullet("images/enemy_shot.png", 0, 1, 5, 1, "Down")

    def is_destroyed(self):
        """Checks if enemy is destroyed"""
        if self.hp <= 0:
            return True

    def shoot(self):
        """Makes enemy shoot"""
        if not self.is_destroyed:
            self.bullet.x = self.x + self.size[0]/2-self.bullet.size[0]
            self.bullet.y = self.y + self.size[1]
            self.bullet.is_fired = True
