/*
 * Helper.cpp
 *
 *  Created on: 2 gru 2015
 *      Author: Kamil
 */

#include <src/helpers/Helper.h>

Helper::Helper()
{
    // TODO Auto-generated constructor stub

}

Helper::~Helper()
{
    // TODO Auto-generated destructor stub
}

double Helper::distance(double lat1, double lon1, double lat2, double lon2)
{
    lat1 = abs(lat1);
    lon1 = abs(lon1);
    lat2 = abs(lat2);
    lon2 = abs(lon2);
    const double R = 6378.137; // Radius of earth in KM
    double dLat = (lat2 - lat1) * M_PI / 180;
    double dLon = (lon2 - lon1) * M_PI / 180;
    double a = sin(dLat/2) * sin(dLat/2) + cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;
    return d * 1000; // meters
}
