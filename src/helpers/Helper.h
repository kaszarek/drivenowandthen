/*
 * Helper.h
 *
 *  Created on: 2 gru 2015
 *      Author: Kamil
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <QtCore/qmath.h>

class Helper
{
public:
    Helper();
    virtual ~Helper();
    /**
     * Calculated distance between two points on the map
     *
     * @lat1
     * Latitude of the first point
     * @lon1
     * Longitude of the first point
     * @lat2
     * Latitude of the second point
     * @lon2
     * Longitude of the second point
     *
     * @return distance in meters
     */
    static double distance(double lat1, double lon1, double lat2, double lon2);
};

#endif /* HELPER_H_ */
