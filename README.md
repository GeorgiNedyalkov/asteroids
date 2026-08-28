# Asteroids

Implementing the classic asteroids game.

Using Raylib
Rewrite with OpenGL

Contraints

- We only use raylib for the platform layer.
- All mathematical functions are written by us.
- We don't use sprites to draw objects

## Build

Simple build.bat script
Raylib is inside our project and we have to make it simple to build and reproduce

## Game

### State

Start Screen

Play

### Mechanics

Player is a space ship that can shoot asteroids.
Player has 5 bullets to shoot after which there is a slight delay then he can shoot again.
The movement of the player is simulated as it is in space there is an accelaration
There is a score for each ship asteroid killed which is 10 points.

There are three types of asteroids. Big, medium and small.
Every time an asteroid is destroyed it spawns two asteroids of its smaller size.

There is an enemy space ship that can shoot the player also.

### Psysics

Wrapping around a tourus world. When an entity reaches the end of the screen it is wrapped
around the other side pixel by pixel.

### Visual effects

Destroing an enemy ship has particle effects.

If the player dies the ship gets wrecked and all the relements are removed.

### Audio

8 bit music

## Assets list

Sounds
