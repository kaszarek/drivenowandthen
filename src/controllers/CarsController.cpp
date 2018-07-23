/*
 * CarsController.cpp
 *
 *  Created on: 23 lis 2015
 *      Author: Kamil
 */

#include <src/controllers/CarsController.h>
#include <qdebug.h>
#include <src/helpers/Helper.h>
#include <src/applicationui.hpp>
#define NAME __FUNCTION__

CarsController::CarsController(QObject* parent) : QObject(parent)
{
    // TODO: these need to be passed to here
    ApplicationUI* app = (ApplicationUI*)parent;
    reservedCarId = "";
    m_userLat = app->getUserLatitude();
    m_userLon = app->getUserLongitude();
}

CarsController::~CarsController()
{
    // TODO Auto-generated destructor stub
}

bool sortByLatitudeCoordsinate(const Car* c1, const Car* c2){
    if (c1->getLatitude() >= c2->getLatitude()){
        return true;
    }
    return false;
}

bool sortByLongitudeCoordsinate(const Car* c1, const Car* c2){
    if (c1->getLongitude() >= c2->getLongitude()){
        return true;
    }
    return false;
}

void CarsController::receivedNewCars(QList<Car*> &cars)
{
    qDebug() << NAME;
    m_cars = cars;
    for (int i = 0; i < m_cars.length(); i++){
        int calculatedDistance = (int) Helper::distance(m_userLat, m_userLon, m_cars.at(i)->getLatitude(), m_cars.at(i)->getLongitude());
        m_cars.at(i)->setDistanceFromUser(calculatedDistance);
        if (m_cars.at(i)->getId().compare(reservedCarId) == 0){
            m_cars.at(i)->setReservedStatus("R");
        }
    }
    emit updateView(cars);
}

void CarsController::markAsReserved(QString id)
{
    qDebug() << NAME;
    reservedCarId = id;
    for (int i = 0; i < m_cars.length(); i++){
        if (m_cars.at(i)->getId().compare(reservedCarId) == 0){
            m_cars.at(i)->setReservedStatus("R");
        }
    }
}

void CarsController::freeUp()
{
    qDebug() << NAME;
    reservedCarId = "";
    for (int i = 0; i < m_cars.length(); i++){
        m_cars.at(i)->setReservedStatus("");
    }
}
