/*
 * DriveNowInterfaceProxy.cpp
 *
 *  Created on: 22 lis 2015
 *      Author: Kamil
 */

#include "DriveNowWebClient.h"
#include "src/helpers/JsonDataExtractor.h"
#include <QDebug>
#include <bb/data/JsonDataAccess>
#include <QStringList>
#include <QDateTime>
#include <zlib.h>
#define NAME __FUNCTION__

using namespace bb::data;
using namespace driveNowJsonExtractor;

const QString DriveNowWebClient::LOGIN_BASE = "https://metrows.drive-now.com/php/drivenowws/v1/user/login?language=en";
const QString DriveNowWebClient::USER_STATUS = "https://metrows.drive-now.com/php/drivenowws/v1/user/status?language=en&auth=%1"; // authorization code from login
const QString DriveNowWebClient::CAR_LIST = "https://api2.drive-now.com/cities/%1?authToken=%2&expand=full"; // city (Berlin) = 6099, authorization code from login
const QString DriveNowWebClient::X_API_KEY = "adf51226795afbc4e7575ccc124face7";
const QString DriveNowWebClient::BOOK_CAR = "https://metrows.drive-now.com/php/drivenowws/v1/reservation/request?language=en&auth=%1";
const QString DriveNowWebClient::CANCEL_RESERVATION = "https://metrows.drive-now.com/php/drivenowws/v1/reservation/cancel?language=en&auth=%1";
const QString DriveNowWebClient::LOGOUT = "https://api2.drive-now.com/logout";
const bool DriveNowWebClient::USE_MOCK_FOR_COMMUNICATION = false;

DriveNowWebClient::DriveNowWebClient(QObject* parent) : IDriveNowClient(parent)
{
    m_cityCode = "6099"; // default Berlin
}

DriveNowWebClient::~DriveNowWebClient()
{
    // TODO Auto-generated destructor stub
}

void DriveNowWebClient::login(QString user, QString password)
{
    qDebug() << NAME;
    //     for mocking purposes
    if (USE_MOCK_FOR_COMMUNICATION){
        qDebug() << "TEMP MOCK with user = " << user << " and pass = " << password;
        emit updateStatus("MOCK - Logged in");
        emit loggedIn(true);
        return;
    }

    if (user.isNull() || password.isNull() || user.isEmpty() || password.isEmpty()){
        qWarning() << "User or password problem !!";
        return;
    }
    QNetworkRequest loginReq;
    QVariantMap userV;
    userV.insert("current_tenant", "DE");
    userV.insert("password", password);
    userV.insert("user", user);
    QByteArray jsonToSend;
    JsonDataAccess jsonAccess;
    jsonAccess.saveToBuffer(userV, &jsonToSend);
    loginReq.setUrl(LOGIN_BASE);
    loginReq.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    loginReq.setRawHeader("Accept-Encoding", "gzip, deflate");
    loginReq.setRawHeader("Accept-Language", "en-US,en;q=0.5");
    loginReq.setRawHeader("Content-Length", QByteArray::number(jsonToSend.size()));
    loginReq.setRawHeader("Content-Type", "application/json; charset=UTF-8");
    loginReq.setRawHeader("Host", "metrows.drive-now.com");
    loginReq.setRawHeader("Origin", "https://de.drive-now.com");
    loginReq.setRawHeader("Referer", "https://de.drive-now.com/en/");

    emit updateStatus("About to send request to login");
    QNetworkReply *reply =  m_networkMan.post(loginReq, jsonToSend);
    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(loginRequestFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
//    Q_UNUSED(reply);
}

void DriveNowWebClient::loginRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << NAME;
    if (reply != NULL &&
        reply->bytesAvailable() > 0 &&
        reply->error() == QNetworkReply::NoError){
        if (reply->isFinished()){
            QByteArray encoding = reply->rawHeader("content-encoding");
            QByteArray cookie = reply->rawHeader("Set-Cookie");
            if ((QString) encoding == "gzip"){
                QString data = (QString) inflateData(reply->readAll());
                JsonDataAccess jda;
                m_loginReceivedData = jda.loadFromBuffer(data);
                m_token = m_loginReceivedData.toMap()["auth"].toString();
                QMap<QString, QVariant> reservationMap =
                            m_loginReceivedData.toMap()["reservation"].toMap();
                QString reservationStatus = reservationMap["status"].toString();
                if (!reservationStatus.isEmpty() && reservationStatus.contains("reserved")){
                    QString systemTimeStr = reservationMap["systemTime"].toString();
                    QString untilTimeStr = reservationMap["reservedUntil"].toString();
                    m_reservationId = reservationMap["reservationId"].toString();
                    m_reservationTime = reservationMap["reservationTime"].toString();
                    qDebug() << systemTimeStr;
                    qDebug() << untilTimeStr;
                    const QString timeFormat = "yyyy-MM-ddTHH:mm:ss+00:00";
                    QDateTime systemTime = QDateTime::fromString(systemTimeStr, timeFormat);
                    QDateTime untilTime = QDateTime::fromString(untilTimeStr, timeFormat);
                    qDebug() << systemTime;
                    qDebug() << untilTime;
                    int secondsToExpire = systemTime.secsTo(untilTime);
                    qDebug() << "User has " << secondsToExpire << "seconds to get to the car";
                    QVariant carsList = reservationMap["car"];
                    QVariantMap carMap = carsList.toMap();
                    Car *car = extractCar(carMap);

                    QList<Car*> foundCars;
                    foundCars.append(car);
                    qDebug() << QString("Extracted car, no %1 - %2").arg(foundCars.length()).arg(car->toString());
//                    emit haveCars(foundCars);
//                    emit reservedCar(m_reservationId, true);

                } else {
                    qDebug() << "No reservation is ongoing";
                    // When user logged in, wait for preffered city code
                    m_cityCode = "";
                    getUserStatus();
                }

                qDebug() << "Logged in. Token = " << m_token;
                emit loggedIn(true);
            }
        }
    } else {
        QString data = (QString) reply->readAll();
        if (reply->error() == QNetworkReply::HostNotFoundError){
            emit loggedIn(false);
            emit problemWithApi("Please check your Internet connection.");
            return;
        }
        JsonDataAccess jda;
        QVariant json = jda.loadFromBuffer(data);
        QString msg = json.toMap()["message"].toString();
        qWarning() << "EXTRACTED MESSAGE = " << msg;
        emit loggedIn(false);
        emit problemWithApi(msg);
        qWarning() << QString("ERROR logging in!! Code = %1\nerrorString = %2\ndata = %3").arg(reply->error()).arg(reply->errorString()).arg(data);
        // extract codeDetail and message
    }
    reply->deleteLater();
}

void DriveNowWebClient::getCars()
{
    qDebug() << NAME;
    if (m_token.isNull() || m_token.isEmpty()){
        qWarning() << "Token is null or empty, will get cars anyway";
    }
    if (m_cityCode.isEmpty()){
        qWarning() << "No city code yet";
        return;
    }
    m_mutex.lock();
    QString cityToUse = m_cityCode;
    m_mutex.unlock();
    QNetworkRequest getCarsReq;
    QString uri = CAR_LIST.arg(cityToUse).arg(m_token);
    getCarsReq.setUrl(uri);
    getCarsReq.setRawHeader("Accept", "application/json,v=1.6");
    getCarsReq.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    getCarsReq.setRawHeader("Accept-Language", "en-US,en;q=0.5");
    getCarsReq.setRawHeader("Host", "api2.drive-now.com");
    getCarsReq.setRawHeader("Origin", "https://de.drive-now.com");
    getCarsReq.setRawHeader("Referer", "https://de.drive-now.com/en/");
    getCarsReq.setRawHeader("X-Language", "en");
    getCarsReq.setRawHeader("X-Api-Key", X_API_KEY.toUtf8());
    // X-Auth-Token:c33a30b33d7c3a3a4f3f109de85f4bcf

    emit updateStatus("About to send request to get all cars");
    emit startLoadingCars();
    QNetworkReply *reply =  m_networkMan.get(getCarsReq);
    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(getCarsRequestFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void DriveNowWebClient::getCarsRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << NAME;
    if (reply != NULL &&
        reply->bytesAvailable() > 0 &&
        reply->error() == QNetworkReply::NoError){
        if (reply->isFinished()){
            QByteArray encoding = reply->rawHeader("content-encoding");
            QByteArray cookie = reply->rawHeader("Set-Cookie");
            if ((QString) encoding == "gzip"){
                QString data = (QString) inflateData(reply->readAll());
                if (data.isEmpty()){
                    qWarning() << "Got empty buffer";
                    emit updateStatus("Api: Got no cars");
                    return;
                }
                JsonDataAccess jda;
                QVariant carsVariant = jda.loadFromBuffer(data);
                QMap<QString, QVariant> jsonMap = carsVariant.toMap();
                QMap<QString, QVariant> carsModule = jsonMap["cars"].toMap();
                QVariantList carsList = carsModule["items"].toList();
                qDebug() << "There is " << carsList.length() << " cars in the reply. Processing...";
                if (carsList.length() == 0){
                    qDebug() << "No cars to extract. Stop processing !";
                    emit updateStatus("No cars to process.");
                    return;
                }
                QList<Car*> foundCars;
                for (int i = 0; i < carsList.length(); i++){
                    QVariantMap carMap = carsList.at(i).toMap();
                    Car* c = extractCar(carMap);
                    if (c == NULL){
                        continue;
                    }
                    foundCars.append(c);
                    qDebug() << QString("Extracted car, no %1 - %2").arg(foundCars.length()).arg(c->toString());
                }

                emit updateStatus("Got all cars, filtering now...");
                emit haveCars(foundCars);
            }
        }
    } else {
        QString data = (QString) reply->readAll();
        JsonDataAccess jda;
        QVariant json = jda.loadFromBuffer(data);
        QString msg = json.toMap()["message"].toString();
        qWarning() << "EXTRACTED MESSAGE = " << msg;
        qWarning() << QString("ERROR getting cars!! Code = %1\nerrorString = %2\ndata = %3").arg(reply->error()).arg(reply->errorString()).arg(data);
        emit updateStatus(QString("Api error code: %1, msg: %2").arg(reply->error()).arg(reply->errorString()));
        emit problemWithApi("Could not talk to the server. Please try again later");
    }
    emit stopLoadingCars();
    reply->deleteLater();
}

void DriveNowWebClient::reserveCar(QString id)
{
    qDebug() << NAME;
    //         for mocking purposes
    if (USE_MOCK_FOR_COMMUNICATION){
        qDebug() << "TEMP MOCK with id = " << id;
        emit updateStatus("MOCK - Reservation successful");
        emit reservedCar("FakeReservationId123", true);
        return;
    }

    if (id.isNull() || id.isEmpty()){
        qWarning() << "Trying to reserve a car with null or empty ID";
        return;
    }
    if (m_token.isNull() || m_token.isEmpty()){
        qWarning() << "Token is null or empty, cannot call get cars";
        return;
    }
    QNetworkRequest reserveCarReq;
    QVariantMap userV;
    userV.insert("carId", id);
    QByteArray jsonToSend;
    JsonDataAccess jsonAccess;
    jsonAccess.saveToBuffer(userV, &jsonToSend);
    QString uri = BOOK_CAR.arg(m_token);
    reserveCarReq.setUrl(uri);
    reserveCarReq.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    reserveCarReq.setRawHeader("Accept-Encoding", "gzip, deflate");
    reserveCarReq.setRawHeader("Accept-Language", "en-US,en;q=0.5");
    reserveCarReq.setRawHeader("Content-Length", QByteArray::number(jsonToSend.size()));
    reserveCarReq.setRawHeader("Content-Type", "application/json; charset=UTF-8");
    reserveCarReq.setRawHeader("Host", "metrows.drive-now.com");
    reserveCarReq.setRawHeader("Origin", "https://de.drive-now.com");
    reserveCarReq.setRawHeader("Referer", "https://de.drive-now.com/en/");

    emit updateStatus("About to reserve the car id = " + id);
    QNetworkReply *reply =  m_networkMan.post(reserveCarReq, jsonToSend);
    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(reserveCarFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void DriveNowWebClient::reserveCarFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << NAME;
    if (reply != NULL &&
        reply->bytesAvailable() > 0 &&
        reply->error() == QNetworkReply::NoError){
        if (reply->isFinished()){
            QByteArray encoding = reply->rawHeader("content-encoding");
            QByteArray cookie = reply->rawHeader("Set-Cookie");
            if ((QString) encoding == "gzip"){
                QString data = (QString) inflateData(reply->readAll());
                JsonDataAccess jda;
                QVariant receivedDataJson = jda.loadFromBuffer(data);
                // extract car as in loginRequestFinished
                // and propagate to the UI and controller and ActiveFrame
                m_reservationId = receivedDataJson.toMap()["reservationId"].toString();
                m_reservationTime = receivedDataJson.toMap()["reservationTime"].toString();
                qDebug() << "Reservation successful, reservation id = " << m_reservationId ;
                emit updateStatus("Reservation successful, resId = " + m_reservationId);
                emit reservedCar(m_reservationId, true);
            }
        }
    } else {
        QString data = (QString) reply->readAll();
        JsonDataAccess jda;
        QVariant json = jda.loadFromBuffer(data);
        QString msg = json.toMap()["message"].toString();
        qWarning() << "EXTRACTED MESSAGE = " << msg;
        qWarning() << QString("ERROR reserving car!! Code = %1\nerrorString = %2\ndata = %3").arg(reply->error()).arg(reply->errorString()).arg(data);
        emit reservedCar("Could not reserve that car. Try another one.", false);
        // extract codeDetail and message
    }
    reply->deleteLater();
}

void DriveNowWebClient::cancelReservation()
{
    qDebug() << NAME;
    // for mocking purposes
    if (USE_MOCK_FOR_COMMUNICATION){
        qDebug() <<"TEMP MOCK";
        emit updateStatus("MOCK - Reservation canceled");
        emit reservationCanceled(true);
        return;
    }

    if (m_token.isNull() || m_token.isEmpty()){
        qWarning() << "Token is null or empty, cannot cancel reservation";
        return;
    }
    QNetworkRequest cancelReservReq;
    QVariantMap userV;
    QByteArray jsonToSend;
    JsonDataAccess jsonAccess;
    jsonAccess.saveToBuffer(userV, &jsonToSend);
    QString uri = CANCEL_RESERVATION.arg(m_token);
    cancelReservReq.setUrl(uri);
    cancelReservReq.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    cancelReservReq.setRawHeader("Accept-Encoding", "gzip, deflate");
    cancelReservReq.setRawHeader("Accept-Language", "en-US,en;q=0.5");
    cancelReservReq.setRawHeader("Content-Length", QByteArray::number(jsonToSend.size()));
    cancelReservReq.setRawHeader("Content-Type", "application/json; charset=UTF-8");
    cancelReservReq.setRawHeader("Host", "metrows.drive-now.com");
    cancelReservReq.setRawHeader("Origin", "https://de.drive-now.com");
    cancelReservReq.setRawHeader("Referer", "https://de.drive-now.com/en/");

    emit updateStatus("About to cancel reservation");
    QNetworkReply *reply =  m_networkMan.post(cancelReservReq, jsonToSend);
    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(cancelReservationFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void DriveNowWebClient::cancelReservationFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << NAME;
    if (reply != NULL &&
        reply->bytesAvailable() > 0 &&
        reply->error() == QNetworkReply::NoError){
        if (reply->isFinished()){
            QByteArray encoding = reply->rawHeader("content-encoding");
            if ((QString) encoding == "gzip"){
                m_reservationId = "";
                m_reservationTime = "";
                qDebug() << "Reservation CANCELED";
                emit updateStatus("Reservation canceled");
                emit reservationCanceled(true);
            }
        }
    } else {
        QString data = (QString) reply->readAll();
        JsonDataAccess jda;
        QVariant json = jda.loadFromBuffer(data);
        QString msg = json.toMap()["message"].toString();
        qWarning() << "EXTRACTED MESSAGE = " << msg;
        qWarning() << QString("ERROR canceling the reservation!! Code = %1\nerrorString = %2\ndata = %3").arg(reply->error()).arg(reply->errorString()).arg(data);
        emit updateStatus("Cacelation sucessfull.");
        emit reservationCanceled(false);
    }
    reply->deleteLater();
}

QByteArray DriveNowWebClient::inflateData(QByteArray deftaledData)
{
    if (deftaledData.size() <= 4) {
        qWarning("inflateData: Input data is truncated");
        return QByteArray();
    }

    QByteArray result;
    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = deftaledData.size();
    strm.next_in = (Bytef*)(deftaledData.data());
    ret = inflateInit2(&strm, 15 +  32); // gzip decoding
    if (ret != Z_OK){
        return QByteArray();
    }
    // run inflate()
    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);  // state not clobbered

        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     // and fall through
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return QByteArray();
        }

        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);
    return result;
}

void DriveNowWebClient::logout()
{
    qDebug() << NAME;
    // for mocking purposes
    if (USE_MOCK_FOR_COMMUNICATION){
        qDebug() <<"TEMP MOCK";
        emit updateStatus("MOCK - Logged OUT");
        emit loggedOut(true);
        return;
    }

    if (m_token.isNull() || m_token.isEmpty()){
        qWarning() << "Token is null or empty, cannot logout";
        emit loggedOut(false);
        return;
    }
    QNetworkRequest logoutReq;
    QVariantMap userV;
    userV.insert("authToken", m_token);
    QByteArray jsonToSend;
    JsonDataAccess jsonAccess;
    jsonAccess.saveToBuffer(userV, &jsonToSend);
    logoutReq.setUrl(LOGOUT);
    logoutReq.setRawHeader("Accept", "application/json");
    logoutReq.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    logoutReq.setRawHeader("Accept-Language", "en-US,en;q=0.5");
    logoutReq.setRawHeader("Content-Length", QByteArray::number(jsonToSend.size()));
    logoutReq.setRawHeader("Content-Type", "application/json");
    logoutReq.setRawHeader("Host", "api2.drive-now.com");
    logoutReq.setRawHeader("Origin", "https://de.drive-now.com");
    logoutReq.setRawHeader("Referer", "https://de.drive-now.com/en/");
    logoutReq.setRawHeader("X-Auth-Token", m_token.toUtf8());
    logoutReq.setRawHeader("X-Api-Key", X_API_KEY.toUtf8());

    emit updateStatus("About to logout");
    QNetworkReply *reply =  m_networkMan.post(logoutReq, jsonToSend);
    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(logoutFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void DriveNowWebClient::logoutFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << NAME;
    if (reply != NULL &&
        reply->bytesAvailable() > 0 &&
        reply->error() == QNetworkReply::NoError){
        if (reply->isFinished()){
            QByteArray encoding = reply->rawHeader("content-encoding");
            if ((QString) encoding == "gzip"){
                m_reservationId = "";
                m_reservationTime = "";
                qDebug() << "Logged out";
                emit updateStatus("Logged out");
                emit loggedOut(true);
            }
        }
    } else {
        QString data = (QString) reply->readAll();
        JsonDataAccess jda;
        QVariant json = jda.loadFromBuffer(data);
        QString msg = json.toMap()["message"].toString();
        qWarning() << "EXTRACTED MESSAGE = " << msg;
        qWarning() << QString("ERROR Logging out!! Code = %1\nerrorString = %2\ndata = %3").arg(reply->error()).arg(reply->errorString()).arg(data);
        emit updateStatus("Problem logging out.");
        emit loggedOut(false);
    }
    reply->deleteLater();
}


void DriveNowWebClient::getUserStatus()
{
    qDebug() << NAME;
    // for mocking purposes
    if (USE_MOCK_FOR_COMMUNICATION){
        qDebug() <<"TEMP MOCK" << " getUserStatus";
        return;
    }

    if (m_token.isNull() || m_token.isEmpty()){
        qWarning() << "Token is null or empty, cannot logout";
        return;
    }

    QNetworkRequest getUserStatusReq;
    QString uri = USER_STATUS.arg(m_token);
    getUserStatusReq.setUrl(uri);
    getUserStatusReq.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    getUserStatusReq.setRawHeader("Accept-Encoding", "gzip, deflate, br");
    getUserStatusReq.setRawHeader("Accept-Language", "en-US,en;q=0.5");
    getUserStatusReq.setRawHeader("Origin", "https://de.drive-now.com");
    getUserStatusReq.setRawHeader("Referer", "https://de.drive-now.com/en/");

    emit updateStatus("About to send request to get all cars");
    QNetworkReply *reply =  m_networkMan.get(getUserStatusReq);
    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(getUserStatusFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);

}
void DriveNowWebClient::getUserStatusFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << NAME;
    if (reply != NULL &&
        reply->bytesAvailable() > 0 &&
        reply->error() == QNetworkReply::NoError){
        if (reply->isFinished()){
            QByteArray encoding = reply->rawHeader("content-encoding");
            if ((QString) encoding == "gzip"){

                QString data = (QString) inflateData(reply->readAll());
                JsonDataAccess jda;
                m_loginReceivedData = jda.loadFromBuffer(data);
                QString preferredCity = m_loginReceivedData.toMap()["preferredCityId"].toString();
                m_mutex.lock();
                m_cityCode = preferredCity;
                m_mutex.unlock();
                qDebug() << "Got preffered city " << preferredCity;
                if (m_reservationId.isEmpty()){
                    getCars();
                }
            }
        }
    } else {
        QString data = (QString) reply->readAll();
        JsonDataAccess jda;
        QVariant json = jda.loadFromBuffer(data);
        QString msg = json.toMap()["message"].toString();
        qWarning() << "EXTRACTED MESSAGE = " << msg;
        qWarning() << QString("ERROR getting user status!! Code = %1\nerrorString = %2\ndata = %3").arg(reply->error()).arg(reply->errorString()).arg(data);
    }
    reply->deleteLater();
}

Car* DriveNowWebClient::extractCar(QVariantMap variantMapCar){
    qDebug() << NAME;
    Car* c = new Car();
    QString street = JsonDataExtractor::extractStreet(variantMapCar);
    if (street.isNull() || street.isEmpty()){
        qDebug() << "Not adding car, street is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setAddressStreet(street);

    QString zipCodeAndCity = JsonDataExtractor::extractZipCodeAndCity(variantMapCar);
    if (zipCodeAndCity.isNull() || zipCodeAndCity.isEmpty()){
        qDebug() << "Not adding car, zipCodeAndCity is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setAddressCodeAndCity(zipCodeAndCity);

    double latitude = JsonDataExtractor::extractLatitide(variantMapCar);
    if (latitude == 0){
        qDebug() << "Not adding car, latitude = 0";
        delete c;
        c = NULL;
        return c;
    }
    c->setLatitude(latitude);

    double longitude = JsonDataExtractor::extractLongitude(variantMapCar);
    if (longitude == 0){
        qDebug() << "Not adding car, longitude = 0";
        delete c;
        c = NULL;
        return c;
    }
    c->setLongitude(longitude);

    QString licencePlate = JsonDataExtractor::extractLicencePlate(variantMapCar);
    if (licencePlate.isNull() || licencePlate.isEmpty()){
        qDebug() << "Not adding car, licencePlate is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setLicencePlate(licencePlate);

    QString id = JsonDataExtractor::extractId(variantMapCar);
    if (id.isNull() || id.isEmpty()){
        qDebug() << "Not adding car, id is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setId(id);

    QString model = JsonDataExtractor::extractModelName(variantMapCar);
    if (model.isNull() || model.isEmpty()){
        qDebug() << "Not adding car, model is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setModelName(model);

    QString producer = JsonDataExtractor::extractMaker(variantMapCar);
    if (producer.isNull() || producer.isEmpty()){
        qDebug() << "Not adding car, producer is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setProducer(producer);

    QString name = JsonDataExtractor::extractName(variantMapCar);
    if (name.isNull() || name.isEmpty()){
        qDebug() << "Not adding car, name is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setName(name);

    int fuelLevel = JsonDataExtractor::extractFuelLevel(variantMapCar);
    if (fuelLevel == -1){
        qDebug() << "Not adding car, fuelLevel = -1";
        delete c;
        c = NULL;
        return c;
    }
    c->setPercentOfFuel(fuelLevel);

    QString transmission = JsonDataExtractor::extractTransmission(variantMapCar);
    if (transmission.isNull() || transmission.isEmpty()){
        qDebug() << "Not adding car, transmission is null or empty";
        delete c;
        c = NULL;
        return c;
    }
    c->setTransmission(transmission);

    return c;
}
