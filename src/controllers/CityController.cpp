/*
 * CityController.cpp
 *
 *  Created on: 28 sty 2016
 *      Author: Kamil
 */

#include <src/controllers/CityController.h>
#include <QDebug>
#define NAME __FUNCTION__

CityController::CityController(QObject *object) : QObject(object)
{
    altitude = 2000;
    latitude = 52.521386;
    longitude = 13.411460;
}

CityController::~CityController()
{} //nothing to destroy

void CityController::changeCityTo(QString cityCode, double altitude, double latitude, double longitude)
{
    qDebug() << NAME;
    qDebug() << "Changing city to code = " + cityCode;
    this->altitude = altitude;
    this->longitude = longitude;
    this->latitude = latitude;
}
