#ifndef RTCOLOR_H
#define RTCOLOR_H

#define BLUE RTColor{210, 245, 255}
#define PURPLE RTColor{190, 130, 250}
#define YELLOW RTColor{255, 230, 80}
#define RED RTColor{255, 0, 0}
#define DARKRED RTColor{85, 10, 10}
#define GRAY RTColor{50, 50, 50}
#define GREEN RTColor{0, 255, 0}
#define SANDY RTColor{220, 190, 90}

struct RTColor
{
    int r, g, b;
};

RTColor getMixedColor(RTColor c1, RTColor c2, float p);

#endif // RTCOLOR_H
