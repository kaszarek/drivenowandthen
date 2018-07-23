/*
 * RevervationController.h
 *
 *  Created on: 3 kwi 2016
 *      Author: Kamil
 */

#ifndef REVERVATIONCONTROLLER_H_
#define REVERVATIONCONTROLLER_H_


#include "../api/DriveNowWebClient.h"
#include "CarsController.h"
#include <QObject>


class RevervationController: public QObject
{
    Q_OBJECT
public:
    RevervationController(QObject* parent = 0, DriveNowWebClient *api = 0, CarsController *cars = 0);
    virtual ~RevervationController();

    Q_INVOKABLE void bookCar(QString id);
    Q_INVOKABLE void cancelReservation();
    void clean();

public slots:
    void responseFromReservation(QString reservationId, bool state);
    void responseFromCancellation(bool state);

private:
    QString reservationId;
    QString carId;
    QObject* view;
    DriveNowWebClient *api;
    CarsController *cars;
};

#endif /* REVERVATIONCONTROLLER_H_ */
