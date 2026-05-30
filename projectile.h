#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <raylib.h>

typedef struct {
    Rectangle rect;
    Vector2 velocity;

    float lifetime;
    float timer;

    int damage;
    Vector2 knockback;

    bool active;

    Texture2D texture;
    int frame;
    float animTime;

} Projectile;

void UpdateProjectile(Projectile *p);
void DrawProjectile(Projectile p, int direction);

#endif