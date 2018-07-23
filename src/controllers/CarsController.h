/*
 * CarsController.h
 *
 *  Created on: 23 lis 2015
 *      Author: Kamil
 */

#ifndef CARSCONTROLLER_H_
#define CARSCONTROLLER_H_

#include <QObject>
#include "src/data/Cluster.h"
#include "src/data/Car.h"

class CarsController: public QObject
{
    Q_OBJECT
public:
    CarsController(QObject* parent = 0);
    virtual ~CarsController();

    void markAsReserved(QString id);
    void freeUp();

public slots:
    void receivedNewCars(QList<Car*> &cars);

    void setUserLatitude(double userLat)
    {
        m_userLat = userLat;
    }

    void setUserLongitude(double userLon)
    {
        m_userLon = userLon;
    }

    signals:
    void updateView(QList<Car*> &cars);

private:
    QList<Car*> m_cars;
    QString reservedCarId;
    double m_userLat; // TODO: extract to other controller
    double m_userLon; // TODO: extract to other controller
};

#endif /* CARSCONTROLLER_H_ */
