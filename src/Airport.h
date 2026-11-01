#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>
using namespace std;

class Airport
{
public:
    string name;
    string city;
    string country;
    string iata;        // the BST key
    string icao;
    double lat;
    double lon;

    Airport()
    {
        lat = 0.0;
        lon = 0.0;
    }

    Airport(string name, string city, string country,
            string iata, string icao, double lat, double lon)
    {
        this->name    = name;
        this->city    = city;
        this->country = country;
        this->iata    = iata;
        this->icao    = icao;
        this->lat     = lat;
        this->lon     = lon;
    }

    string toString()
    {
        return iata + "  " + name + ", " + city + ", " + country;
    }
};

#endif
