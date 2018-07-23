/*
 * IPointOnMap.h
 *
 *  Created on: 1 gru 2015
 *      Author: Kamil
 */

#ifndef IPOINTONMAP_H_
#define IPOINTONMAP_H_

#include <QObject>

class IPointOnMap : public QObject
{
public:
    IPointOnMap(QObject* parent = 0): QObject(parent) {latitude = 0; longitude = 0;};
    virtual ~IPointOnMap(){};

    double getLatitude() const
    {
        return latitude;
    }

    void setLatitude(double latituide)
    {
        this->latitude = latituide;
    }

    double getLongitude() const
    {
        return longitude;
    }

    void setLongitude(double longitude)
    {
        this->longitude = longitude;
    }
protected:
    double longitude;
    double latitude;
};

Q_DECLARE_INTERFACE(IPointOnMap, "IPointOnMap")

#endif /* IPOINTONMAP_H_ */
