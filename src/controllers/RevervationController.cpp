/*
 * RevervationController.cpp
 *
 *  Created on: 3 kwi 2016
 *      Author: Kamil
 */

#include <src/controllers/RevervationController.h>

RevervationController::RevervationController(QObject* parent, DriveNowWebClient *api, CarsController *cars) : QObject(parent)
{
    clean();
    view = parent;
    this->api = api;
    this->cars = cars;
}

RevervationController::~RevervationController()
{
    // TODO Auto-generated destructor stub
}

void RevervationController::bookCar(QString id)
{
    carId = id;
    api->reserveCar(id);
}

void RevervationController::cancelReservation()
{
    api->cancelReservation();
}

void RevervationController::responseFromReservation(QString reservationId, bool state)
{
    if (state){
        this->reservationId = reservationId;
        cars->markAsReserved(carId);
        QMetaObject::invokeMethod(view, "reservedOk");
    } else
    {
        carId = "";
        QMetaObject::invokeMethod(view, "reservedNok");
    }
}

void RevervationController::responseFromCancellation(bool state)
{
    if (state){
        carId = "";
        this->reservationId = "";
        cars->freeUp();
        QMetaObject::invokeMethod(view, "canceledOk");
    } else
    {
        QMetaObject::invokeMethod(view, "canceledNok");
    }
}

void RevervationController::clean()
{
    reservationId = "";
    carId = "";
    // TODO this has to be thread safe to perform safetly
//    if (cars){
//        cars->freeUp();
//    }
}
