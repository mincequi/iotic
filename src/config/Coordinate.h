#ifndef COORDINATE_H
#define COORDINATE_H


class Coordinate {
public:
    Coordinate();

    bool isValid() const;
    double latitude() const;
    double longitude() const;
};

#endif // COORDINATE_H
