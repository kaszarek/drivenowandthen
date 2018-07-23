/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QMutex>
#include "api/DriveNowWebClient.h"
#include "controllers/CityController.h"
#include "controllers/CarsController.h"
#include "controllers/ClustersController.h"
#include "controllers/RevervationController.h"
#include "controllers/CredendialsController.h"
#include "ActiveFrameQML.h"

namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI() {}

    Q_INVOKABLE QVariantList worldToPixelInvokable(QObject* mapObject, double latitude, double longitude) const;
    Q_INVOKABLE void updateMarkers(QObject* mapObject, QObject* containerObject);
    Q_INVOKABLE void login(QString user, QString pass);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void updateUserLatitude(double newLat);
    Q_INVOKABLE void updateUserLongitude(double newLon);
    Q_INVOKABLE void setCurrentCity(QString city);
    Q_INVOKABLE void getCars();
    Q_INVOKABLE void adjustClusterToAltitude(double altitude, double latitude, double longitude);

    double getUserLatitude() const
    {
        return m_userLatitude;
    }

    double getUserLongitude() const
    {
        return m_userLongitude;
    }

private slots:
    void onSystemLanguageChanged();
    void updateClustersWithCars(QList<Car*> & cars);
    void updateViewWithClusters(QList<Cluster*> & clusters);
    void showMessageToUser(QString message);
    void notifyViewAboutProblem(QString msg);
    void onLogin(bool result);
    void onLogout(bool result);
    void onThumbnail();
    void onFullScreen();
    void stopLoadingCars();
    void startLoadingCars();

private:
    bool m_updatingCLusters;
    QMutex mutex;
    QObject* pinsContainer;
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    DriveNowWebClient* driveNowApi;
    CityController * cityController;
    CarsController *carsController;
    ClustersController *clusterController;
    RevervationController *reservationController;
    CredendialsController *credentialsController;
    QPoint worldToPixel(QObject* mapObject, double latitude, double longitude) const;
    double m_userLatitude;
    double m_userLongitude;
    ActiveFrameQML *activeFrame;
};

#endif /* ApplicationUI_HPP_ */
