#include "config.h"
#include "types.h"

#include "maths.h"
#include "maths3D.h"

//#include "vdp.h"
//#include "bmp.h"


#define MATOBJ      1
#define MATVIEW     2
#define MATPROJ     4
#define MATTRANS    8


Vect3D_f16 pts_3D[MAT3D_MAXPOINTS];
Vect2D_s16 pts_2D[MAT3D_MAXPOINTS];
//u16 face_index[MAT3D_MAXFACE];

static Vect3D_f16 light;

Vect3D_f16 light_trans;
Vect3D_f16 camview_trans;

static Trans3D_f16 trans;
static Mat3D_f16 mat;
static Mat3D_f16 matInv;

static fix16 camDist;

static u16 rebuildMat;


void reset3D()
{
    camDist = FIX16(30);

    light.x = FIX16(1);
    light.y = FIX16(0);
    light.z = FIX16(0);

    resetMat3D();
}


void setCamDist3D(fix16 value)
{
    camDist = FIX16(value);
}

void setLight3D(Vect3D_f16 *value)
{
    light.x = value->x;
    light.y = value->y;
    light.z = value->z;
}


void resetMat3D()
{
    trans.Tx = 0;
    trans.Ty = 0;
    trans.Tz = 0;
    trans.Rx = 0;
    trans.Ry = 0;
    trans.Rz = 0;

    rebuildMat = 1;
}


void setTXMat3D(fix16 tx)
{
    trans.Tx = tx;

    rebuildMat = 1;
}

void setTYMat3D(fix16 ty)
{
    trans.Ty = ty;

    rebuildMat = 1;
}

void setTZMat3D(fix16 tz)
{
    trans.Tz = tz;

    rebuildMat = 1;
}

void setTXYZMat3D(fix16 tx, fix16 ty, fix16 tz)
{
    trans.Tx = tx;
    trans.Ty = ty;
    trans.Tz = tz;

    rebuildMat = 1;
}

void setTransMat3D(Vect3D_f16 *t)
{
    trans.Tx = t->x;
    trans.Ty = t->y;
    trans.Tz = t->z;

    rebuildMat = 1;
}


void setRXMat3D(fix16 rx)
{
    trans.Rx = rx;

    rebuildMat = 1;
}

void setRYMat3D(fix16 ry)
{
    trans.Ry = ry;

    rebuildMat = 1;
}

void setRZMat3D(fix16 rz)
{
    trans.Rz = rz;

    rebuildMat = 1;
}

void setRXYZMat3D(fix16 rx, fix16 ry, fix16 rz)
{
    trans.Rx = rx;
    trans.Ry = ry;
    trans.Rz = rz;

    rebuildMat = 1;
}

void setRotMat3D(Vect3D_f16 *rot)
{
    trans.Rx = rot->x;
    trans.Ry = rot->y;
    trans.Rz = rot->z;

    rebuildMat = 1;
}


static void buildMat3D()
{
    fix16 sx, sy, sz;
    fix16 cx, cy, cz;
    fix16 sxsy, cxsy;

    sx = fix16Sin(fix16ToInt(trans.Rx));
    sy = fix16Sin(fix16ToInt(trans.Ry));
    sz = fix16Sin(fix16ToInt(trans.Rz));
    cx = fix16Cos(fix16ToInt(trans.Rx));
    cy = fix16Cos(fix16ToInt(trans.Ry));
    cz = fix16Cos(fix16ToInt(trans.Rz));

    sxsy = fix16Mul(sx, sy);
    cxsy = fix16Mul(cx, sy);

    mat.a.x = fix16Mul(cy, cz);
    mat.b.x = -fix16Mul(cy, sz);
    mat.c.x = sy;

    mat.a.y = fix16Mul(sxsy, cz) + fix16Mul(cx, sz);
    mat.b.y = fix16Mul(cx, cz) - fix16Mul(sxsy, sz);
    mat.c.y = -fix16Mul(sx, cy);

    mat.a.z = fix16Mul(sx, sz) - fix16Mul(cxsy, cz);
    mat.b.z = fix16Mul(cxsy, sz) + fix16Mul(sx, cz);
    mat.c.z = fix16Mul(cx, cy);

    matInv.a.x = mat.a.x;
    matInv.b.x = mat.a.y;
    matInv.c.x = mat.a.z;

    matInv.a.y = mat.b.x;
    matInv.b.y = mat.b.y;
    matInv.c.y = mat.b.z;

    matInv.a.z = mat.c.x;
    matInv.b.z = mat.c.y;
    matInv.c.z = mat.c.z;

    rebuildMat = 0;
}


void transform3D(const Vect3D_f16 *src, Vect3D_f16 *dest, u16 numv)
{
    const Vect3D_f16 *s;
    Vect3D_f16 *d;
    u16 i;

    if (rebuildMat) buildMat3D();

    s = src;
    d = dest;
    i = numv;
    while (i--)
    {
        d->x = fix16Mul(s->x, mat.a.x) + fix16Mul(s->y, mat.a.y) + fix16Mul(s->z, mat.a.z) + trans.Tx;
        d->y = fix16Mul(s->x, mat.b.x) + fix16Mul(s->y, mat.b.y) + fix16Mul(s->z, mat.b.z) + trans.Ty;
        d->z = fix16Mul(s->x, mat.c.x) + fix16Mul(s->y, mat.c.y) + fix16Mul(s->z, mat.c.z) + trans.Tz;

        s++;
        d++;
    }

    // transform light vector
    light_trans.x = fix16Mul(light.x, matInv.a.x) + fix16Mul(light.y, matInv.a.y) + fix16Mul(light.z, matInv.a.z);
    light_trans.y = fix16Mul(light.x, matInv.b.x) + fix16Mul(light.y, matInv.b.y) + fix16Mul(light.z, matInv.b.z);
    light_trans.z = fix16Mul(light.x, matInv.c.x) + fix16Mul(light.y, matInv.c.y) + fix16Mul(light.z, matInv.c.z);

    // transform camview vector (0, 0, 1)
    camview_trans.x = fix16Mul(FIX16(1), matInv.a.z);
    camview_trans.y = fix16Mul(FIX16(1), matInv.b.z);
    camview_trans.z = fix16Mul(FIX16(1), matInv.c.z);
}

void project3DFixed(const Vect3D_f16 *src, Vect2D_f16 *dest, u16 numv, u16 w, u16 h)
{
    const Vect3D_f16 *s;
    Vect2D_f16 *d;
    fix16 zi;
    fix16 wi, hi;
    u16 i;

//    wi = intToFix16(BMP_PIXWIDTH / 2);
//    hi = intToFix16(BMP_PIXHEIGHT / 2);
    wi = intToFix16(w / 2);
    hi = intToFix16(h / 2);
    s = src;
    d = dest;
    i = numv;
    while (i--)
    {
        if (s->z >> 3)
        {
            zi = fix16Div(camDist, s->z >> 3);
            d->x = wi + fix16Mul(s->x / 2, zi);
            d->y = hi - fix16Mul(s->y, zi);
        }
        else
        {
            d->x = FIX16(-1);
            d->y = FIX16(-1);
        }

        s++;
        d++;
    }
}

void project3DInt(const Vect3D_f16 *src, Vect2D_s16 *dest, u16 numv, u16 w, u16 h)
{
    const Vect3D_f16 *s;
    Vect2D_s16 *d;
    fix16 zi;
    u16 wi, hi;
    u16 i;

//    wi = BMP_PIXWIDTH / 2;
//    hi = PMP_PIXHEIGHT / 2;
    wi = w / 2;
    hi = h / 2;
    s = src;
    d = dest;
    i = numv;
    while (i--)
    {
        if (s->z)
        {
            zi = fix16Div(camDist, s->z >> 3);
            d->x = wi + fix16ToInt(fix16Mul(s->x / 2, zi));
            d->y = hi - fix16ToInt(fix16Mul(s->y, zi));
        }
        else
        {
            d->x = -1;
            d->y = -1;
        }

        s++;
        d++;
    }
}
