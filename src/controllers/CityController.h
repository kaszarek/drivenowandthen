/*
 * CityController.h
 *
 *  Created on: 28 sty 2016
 *      Author: Kamil
 */

#ifndef CITYCONTROLLER_H_
#define CITYCONTROLLER_H_

#include <QObject>
#include "src/api/DriveNowWebClient.h"

class CityController: public QObject
{
    Q_OBJECT
public:
    CityController(QObject *object = 0);
    virtual ~CityController();

    Q_INVOKABLE void changeCityTo(QString cityCode, double altitude, double latitude, double longitude);


    double getAltitude() const
    {
        return altitude;
    }

    double getLatitude() const
    {
        return latitude;
    }

    double getLongitude() const
    {
        return longitude;
    }

    Q_INVOKABLE void setAltitude(double altitude)
    {
        this->altitude = altitude;
    }

    Q_INVOKABLE void setLatitude(double latitude)
    {
        this->latitude = latitude;
    }

    Q_INVOKABLE void setLongitude(double longitude)
    {
        this->longitude = longitude;
    }

private:
    double altitude;
    double latitude;
    double longitude;
};


#endif /* CITYCONTROLLER_H_ */
