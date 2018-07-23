/*
 * Car.cpp
 *
 *  Created on: 23 lis 2015
 *      Author: Kamil
 */

#include <src/data/Car.h>
#include <qdebug.h>

Car::Car(QObject* parent) : IPointOnMap(parent)
{
    // TODO Auto-generated constructor stub
    m_distanceFromUser = 100000;
    m_percentOfFuel = 0;
    m_reservedStatus = "";
}

Car::~Car()
{
    // TODO Auto-generated destructor stub
}

const QString Car::toString()
{
    QString string = QString("%1 %2 %3").arg(m_name).arg(m_licencePlate).arg(m_id);
    return string;
}
