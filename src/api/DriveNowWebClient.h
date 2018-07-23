/*
 * DriveNowInterfaceProxy.h
 *
 *  Created on: 22 lis 2015
 *      Author: Kamil
 */

#ifndef DRIVENOWINTERFACEPROXY_H_
#define DRIVENOWINTERFACEPROXY_H_

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QObject>
#include <QMutex>
#include "src/data/Car.h"
#include "src/interfaces/IDriveNowClient.h"

/**
 * Implementation of a web client DriveNow interface
 */
class DriveNowWebClient: public IDriveNowClient
{
    Q_OBJECT

public:
    DriveNowWebClient(QObject* parent = 0);
    // TODO: extract all public, interface functions to interface
    // TODO: update all depend classes to use abstraction
    void login(QString user, QString password);
    void getCars();
    void reserveCar(QString id);
    void cancelReservation();
    void logout();
    virtual ~DriveNowWebClient();

    void setCity(const QString& cityCode)
    {
        if (cityCode.isNull() || cityCode.isEmpty()){
            return;
        }
        m_cityCode = cityCode;
    }

signals:
    void loggedIn(bool result);
    void haveCars(QList<Car*> &list);
    void problemWithApi(QString message);
    void updateStatus(QString status);
    void reservedCar(QString reservationId, bool state);
    void reservationCanceled(bool state);
    void loggedOut(bool result);
    void stopLoadingCars();
    void startLoadingCars();

private:
    static const QString LOGIN_BASE;
    static const QString USER_STATUS;
    static const QString CAR_LIST;
    static const QString X_API_KEY;
    static const QString BOOK_CAR;
    static const QString CANCEL_RESERVATION;
    static const QString LOGOUT;
    static const bool USE_MOCK_FOR_COMMUNICATION;

    QString m_cityCode;
    QString m_token;
    QString m_reservationId;
    QString m_reservationTime;
    QVariant m_loginReceivedData;
    QNetworkAccessManager m_networkMan;
    QMutex m_mutex;
    QByteArray inflateData(QByteArray deftaledData);
    Car* extractCar(QVariantMap variantMapCar);
    void getUserStatus();

private slots:
    void loginRequestFinished();
    void getCarsRequestFinished();
    void reserveCarFinished();
    void cancelReservationFinished();
    void logoutFinished();
    void getUserStatusFinished();
};

#endif /* DRIVENOWINTERFACEPROXY_H_ */
