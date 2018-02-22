//Heriberto Jimenez
//DoS Project


/*
void physics()
{
if (gl.keys[XK_p]) {
                 //a little time between each bullet
                 struct timespec bt;
                 clock_gettime(CLOCK_REALTIME, &bt);
                 double ts = timeDiff(&g.bulletTimer, &bt);
                 if (ts > 0.00005) {
                         timeCopy(&g.bulletTimer, &bt);
                         if (g.nbullets < MAX_BULLETS) {
                                 //shoot a bullet...
                                 //Bullet *b = new Bullet;
                                 Bullet *b = &g.barr[g.nbullets];
                                 timeCopy(&b->time, &bt);
                                 b->pos[0] = g.ship.pos[0];
                                 b->pos[1] = g.ship.pos[1];
                                 b->vel[0] = g.ship.vel[0];
                                 b->vel[1] = g.ship.vel[1];
                                 //convert ship angle to radians
                                 Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
                                 //convert angle to a vector
                                 Flt xdir = cos(rad);
                                 Flt ydir = sin(rad);
                                 b->pos[0] += xdir*20.0f;
                                 b->pos[1] += ydir*20.0f;
                                 b->vel[0] += xdir*6.0f + rnd()*0.1;
                                 b->vel[1] += ydir*6.0f + rnd()*0.1;
                                 b->color[0] = 1.0f;
                                 b->color[1] = 1.0f;
                                 b->color[2] = 1.0f;
                                 g.nbullets++;
                         }
                 }
         }

}
*/
