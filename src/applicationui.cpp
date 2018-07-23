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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Container>
#include <bb/cascades/maps/MapView>
#include <bb/cascades/QmlDocument>
#include <bb/platform/geo/Point.hpp>
#include <QtCore/qmath.h>
#include <bb/device/DisplayInfo>
#include "src/sdk/src/Countly.hpp"


using namespace bb::cascades;
using namespace bb::cascades::maps;
using namespace bb::platform::geo;
using namespace bb::device;

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_userLatitude = 52.5142847;
    m_userLongitude = 13.4036335;
    m_updatingCLusters = false;
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    qmlRegisterType<QTimer>("MyTimer", 1, 0, "QTimer");

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    activeFrame = new ActiveFrameQML();

    credentialsController = new CredendialsController();
    qml->setContextProperty("userData", credentialsController);
    qml->setContextProperty("activeFrame", activeFrame);
    qml->setContextProperty("_mapViewTest", this);
    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    pinsContainer = root->findChild<QObject*>("pinContainer");
    if (!pinsContainer){
        qWarning() << "PINS_CONTINER NOT FOUND";
    }
    // Set created root object as the application scene

    Application::instance()->setScene(root);
    driveNowApi = new DriveNowWebClient(this);
    cityController = new CityController(driveNowApi);
    qml->setContextProperty("_cityController", cityController);
    carsController = new CarsController(this);
    clusterController = new ClustersController(this);
    reservationController = new RevervationController(pinsContainer, driveNowApi, carsController);
    qml->setContextProperty("_reservationController", reservationController);

    if (QObject:: connect(Application::instance(), SIGNAL(thumbnail()), this, SLOT(onThumbnail()))){
        qWarning() << "Could not connect {thumbnail} to {onThumbnail}";
    }
    if (QObject:: connect(Application::instance(), SIGNAL(fullscreen), this, SLOT(onFullScreen()))){
        qWarning() << "Could not connect {fullscreen} to {onFullscreen}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(loggedIn(bool)), this, SLOT(onLogin(bool)))){
        qWarning() << "Could not connect {loggedIn} to {onLogin}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(haveCars(QList<Car*> &)), carsController, SLOT(receivedNewCars(QList<Car*> &)))){
        qWarning() << "Could not connect {haveCars} to {receivedNewCars}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(updateStatus(QString)), this, SLOT(showMessageToUser(QString)))){
        qWarning() << "Could not connect {updateStatus} to {showMessageToUser}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(reservedCar(QString, bool)), reservationController, SLOT(responseFromReservation(QString, bool)))){
        qWarning() << "Could not connect {reservedCar} to {responseFromReservation}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(reservationCanceled(bool)), reservationController, SLOT(responseFromCancellation(bool)))){
        qWarning() << "Could not connect {reservationCanceled} to {responseFromCancellation}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(loggedOut(bool)), this, SLOT(onLogout(bool)))){
        qWarning() << "Could not connect {loggedOut} to {loggedOut}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(problemWithApi(QString)), this, SLOT(notifyViewAboutProblem(QString)))){
        qWarning() << "Could not connect {problemWithApi} to {notifyViewAboutProblem}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(stopLoadingCars()), this, SLOT(stopLoadingCars()))){
        qWarning() << "Could not connect {stopLoadingCars} to {stopLoadingCars}";
    }
    if (!QObject::connect(driveNowApi, SIGNAL(startLoadingCars()), this, SLOT(startLoadingCars()))){
        qWarning() << "Could not connect {startLoadingCars} to {startLoadingCars}";
    }
    if (!QObject::connect(carsController, SIGNAL(updateView(QList<Car*> &)), this, SLOT(updateClustersWithCars(QList<Car*> &)))){
        qWarning() << "Could not connect {updateView} to {updateViewWithCars}";
    }
    if (!QObject::connect(clusterController, SIGNAL(updateView(QList<Cluster*> &)), this, SLOT(updateViewWithClusters(QList<Cluster*> &)))){
        qWarning() << "Could not connect {updateView} to {updateViewWithClusters}";
    }

    //TODO: Kamil -> Make loading cars after successful login
    QString user = credentialsController->read("user");
    QString pass = credentialsController->read("pass");

    QNetworkConfigurationManager networkManager;
    bool isOnline = networkManager.isOnline();

    if (isOnline){
        if (!user.isNull() && !user.isEmpty() && !pass.isNull() && !pass.isEmpty()){
            qDebug() << "Auto log in ENABLED";
            login(user, pass);
        } else {
            qDebug() << "Auto log in DISABLED";
            getCars();
        }
    } else {
        notifyViewAboutProblem("Please check you internet connection. Once back online restart the application.");
    }
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("DriveNowAndThen_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

QVariantList ApplicationUI::worldToPixelInvokable(QObject* mapObject, double latitude, double longitude) const
{
    MapView* mapview = qobject_cast<MapView*>(mapObject);
    const Point worldCoordinates = Point(latitude, longitude);
    const QPoint pixels = mapview->worldToWindow(worldCoordinates);
    return QVariantList() << pixels.x() << pixels.y();
}

void ApplicationUI::updateMarkers(QObject* mapObject, QObject* containerObject)
{
    mutex.lock();
    MapView* mapview = qobject_cast<MapView*>(mapObject);
    Container* container = qobject_cast<Container*>(containerObject);
    for (int i = 0; i < container->count(); i++) {
        const QPoint xy = worldToPixel(mapview,
                                       container->at(i)->property("lat").value<double>(),
                                       container->at(i)->property("lon").value<double>());
        container->at(i)->setProperty("x", xy.x());
        container->at(i)->setProperty("y", xy.y());
    }
    mutex.unlock();
}

QPoint ApplicationUI::worldToPixel(QObject* mapObject, double latitude, double longitude) const
{
    MapView* mapview = qobject_cast<MapView*>(mapObject);
    const Point worldCoordinates = Point(latitude, longitude);
    return mapview->worldToWindow(worldCoordinates);
}

void ApplicationUI::login(QString user, QString pass)
{
    driveNowApi->login(user, pass);
    countly::CountlyEvent event(this, "login", 1);
    event.send();
}

void ApplicationUI::updateClustersWithCars(QList<Car*> & cars)
{
    qDebug() << __FUNCTION__;
    if (cars.length() < 1){
        qWarning() << "No cars to update clusters with";
        delete &cars;
    }
    QList<IPointOnMap*> points;
    for (int i = 0; i < cars.length(); i++){
        points.append((IPointOnMap*)(cars.at(i)));
    }
    clusterController->insertPoints(points);
    adjustClusterToAltitude(cityController->getAltitude(), cityController->getLatitude(), cityController->getLongitude());
}

void ApplicationUI::showMessageToUser(QString message)
{
    QMetaObject::invokeMethod(pinsContainer, "showToast", Q_ARG(QVariant, message));
}

void ApplicationUI::stopLoadingCars()
{
    QMetaObject::invokeMethod(pinsContainer, "stopLoadingCars");
}

void ApplicationUI::startLoadingCars()
{
    QMetaObject::invokeMethod(pinsContainer, "startLoadingCars");
}

void ApplicationUI::updateViewWithClusters(QList<Cluster*>& clusters)
{
    mutex.lock();
//    QMetaObject::invokeMethod(pinsContainer, "removeAllPins");
    for (int i = 0; i < clusters.length(); i++){
        QList<IPointOnMap*> content = (clusters.at(i)->getContentAtDepth(0));
        if (content.length() != 0){ // if there are actual points with details to show
            for (int j = 0; j < content.length(); j++){
                Car* car = (qobject_cast<Car*>(content.at(j)));
                QString text = QString("%1, %2\n%3\n%4m    [%5]    Fuel: %6% ").arg(car->getModelName())
                                                                               .arg(car->getName())
                                                                               .arg(car->getLicencePlate())
                                                                               .arg(car->getDistanceFromUser())
                                                                               .arg(car->getTransmission())
                                                                               .arg(car->getPercentOfFuel());
                double lon = car->getLongitude();
                double lat = car->getLatitude();
                qDebug() << "Adding car " << car->getLicencePlate() << " at LON=" << lon << " LAT=" << lat;
                QString carId = car->getId();
                QMetaObject::invokeMethod(pinsContainer, "addFromModel", Q_ARG(QVariant, text),
                        Q_ARG(QVariant, lat),
                        Q_ARG(QVariant, lon),
                        Q_ARG(QVariant, carId),
                        Q_ARG(QVariant, car->getReservedStatus().compare("R") == 0 ? true : false),
                        Q_ARG(QVariant, 0));
            }
        }
        if (clusters.at(i)->getChildrenCount() != 0){ // if there are still points in the cluster
            double lon = clusters.at(i)->getLongitude();
            double lat = clusters.at(i)->getLatitude();
            qDebug() << "Cluster to show with " << clusters.at(i)->getContentCountBelowCluster() << " at LON=" << lon << " LAT=" << lat;
            QMetaObject::invokeMethod(pinsContainer, "addFromModel", Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, lat),
                    Q_ARG(QVariant, lon),
                    Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, clusters.at(i)->getContentCountBelowCluster()));
        }
    }
    mutex.unlock();
    m_updatingCLusters = false;
}

void ApplicationUI::updateUserLatitude(double newLat)
{
    m_userLatitude = newLat;
    carsController->setUserLatitude(newLat);
}

void ApplicationUI::updateUserLongitude(double newLon)
{
    m_userLongitude = newLon;
    carsController->setUserLongitude(newLon);
}

void ApplicationUI::notifyViewAboutProblem(QString msg)
{
    if (msg.isNull() || msg.isEmpty()){
        qWarning() << "Notification message was empty or null";
        return;
    }
    QMetaObject::invokeMethod(pinsContainer, "showToastNotification", Q_ARG(QVariant, msg));
}

void ApplicationUI::onLogin(bool result)
{
    QMetaObject::invokeMethod(pinsContainer, "loggedIn", Q_ARG(QVariant, result));
    if (result){
        getCars();
    } else {
        credentialsController->clean();
    }
}

void ApplicationUI::setCurrentCity(QString city)
{
    driveNowApi->setCity(city);
    getCars();
    countly::CountlyEvent event(this, "changeCity", 1);
    event.set("city", city);
    event.send();
}

void ApplicationUI::onThumbnail()
{
    if (!Application::instance()->cover()){
        Application::instance()->setCover(activeFrame);
    }
}

void ApplicationUI::onFullScreen()
{
    if (!Application::instance()->cover()){
        Application::instance()->setCover(activeFrame);
    }
}

void ApplicationUI::adjustClusterToAltitude(double altitude, double latitude, double longitude)
{
    DisplayInfo display;
    qDebug() << "Alt = " << altitude << endl;
    qDebug() << "Lat = " << latitude << endl;
    qDebug() << "Lon = " << longitude << endl;

    double kmInDegree = 111.0;
    double ratio = altitude / 1000.0 / kmInDegree / 1.7;

    qDebug() << "Display W = " <<  display.pixelSize().width() << endl;
    qDebug() << "Display H = " << display.pixelSize().height() << endl;

    qDebug() << "New NORTH = " << latitude + ratio << endl;
    qDebug() << "New SOUTH = " << latitude - ratio << endl;
    qDebug() << "New WEST = " << longitude - ratio << endl;
    qDebug() << "New EAST = " << longitude + ratio << endl;

    double s = latitude - ratio;
    double n = latitude + ratio;
    double e = longitude - ratio;
    double w = longitude + ratio;
    if (!m_updatingCLusters){
        m_updatingCLusters = true;
        clusterController->adjustClusters(w, e, s, n);

        if (false){
            QMetaObject::invokeMethod(pinsContainer, "addFromModel", Q_ARG(QVariant, "NE"),
                    Q_ARG(QVariant, n),
                    Q_ARG(QVariant, e),
                    Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, "DEBUG"),
                    Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(pinsContainer, "addFromModel", Q_ARG(QVariant, "NW"),
                    Q_ARG(QVariant, n),
                    Q_ARG(QVariant, w),
                    Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, "DEBUG"),
                    Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(pinsContainer, "addFromModel", Q_ARG(QVariant, "SE"),
                    Q_ARG(QVariant, s),
                    Q_ARG(QVariant, e),
                    Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, "DEBUG"),
                    Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(pinsContainer, "addFromModel", Q_ARG(QVariant, "SW"),
                    Q_ARG(QVariant, s),
                    Q_ARG(QVariant, w),
                    Q_ARG(QVariant, ""),
                    Q_ARG(QVariant, "DEBUG"),
                    Q_ARG(QVariant, 0));
        }
    }
}

void ApplicationUI::getCars()
{
    driveNowApi->getCars();
}

void ApplicationUI::onLogout(bool result)
{
    QMetaObject::invokeMethod(pinsContainer, "loggedOut", Q_ARG(QVariant, result));
}

void ApplicationUI::logout()
{
    reservationController->clean();
    driveNowApi->logout();
    countly::CountlyEvent event(this, "logout", 1);
    event.send();
}
