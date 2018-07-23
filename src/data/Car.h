/*
 * Car.h
 *
 *  Created on: 23 lis 2015
 *      Author: Kamil
 */

#ifndef CAR_H_
#define CAR_H_

#include <QObject>
#include "src/interfaces/IPointOnMap.h"

class Car: public IPointOnMap
{
    Q_OBJECT
public:
    Car(QObject* parent = 0);
    virtual ~Car();

    const QString& getAddressStreet() const
    {
        return m_addressStreet;
    }

    void setAddressStreet(const QString& addressStreet)
    {
        m_addressStreet = addressStreet;
    }

    const QString& getLicencePlate() const
    {
        return m_licencePlate;
    }

    void setLicencePlate(const QString& licencePlate)
    {
        m_licencePlate = licencePlate;
    }

    const QString toString();

    const QString& getId() const
    {
        return m_id;
    }

    void setId(const QString& id)
    {
        m_id = id;
    }

    const QString& getModelName() const
    {
        return m_modelName;
    }

    void setModelName(const QString& modelName)
    {
        m_modelName = modelName;
    }

    const QString& getProducer() const
    {
        return m_producer;
    }

    void setProducer(const QString& producer)
    {
        m_producer = producer;
    }

    const QString& getAddressCodeAndCity() const
    {
        return m_addressCodeAndCity;
    }

    void setAddressCodeAndCity(const QString& addressCodeAndCity)
    {
        m_addressCodeAndCity = addressCodeAndCity;
    }

    const QString& getName() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

    int getPercentOfFuel() const
    {
        return m_percentOfFuel;
    }

    void setPercentOfFuel(int percentOfFuel)
    {
        m_percentOfFuel = percentOfFuel;
    }

    const QString& getTransmission() const
    {
        return m_transmission;
    }

    void setTransmission(const QString& transmission)
    {
        m_transmission = transmission;
    }

    int getDistanceFromUser() const
    {
        return m_distanceFromUser;
    }

    void setDistanceFromUser(int distanceFromUser)
    {
        m_distanceFromUser = distanceFromUser;
    }

    const QString& getReservedStatus() const
    {
        return m_reservedStatus;
    }

    void setReservedStatus(const QString& reservedStatus)
    {
        m_reservedStatus = reservedStatus;
    }

private:
    QString m_addressStreet;
    QString m_addressCodeAndCity;
    int m_distanceFromUser; // meters
    int m_percentOfFuel;
    QString m_name;
    QString m_producer; // BMW, Mini
    QString m_modelName;
    QString m_transmission;
    QString m_licencePlate;
    QString m_id;
    QString m_reservedStatus;
};

#endif /* CAR_H_ */
