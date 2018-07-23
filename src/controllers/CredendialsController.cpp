/*
 * CredendialsController.cpp
 *
 *  Created on: 5 kwi 2016
 *      Author: Kamil
 */

#include <src/controllers/CredendialsController.h>
#include <QSettings>
#include <QDebug>

CredendialsController::CredendialsController(QObject *parent) : QObject(parent)
{}

CredendialsController::~CredendialsController()
{} // nothign to destroy

void CredendialsController::save(QString key, QString value)
{
    qDebug() << "Saving credentials key = " << key << ", value = " << value;
    QSettings settings("rideNow", "PersistentStorage");
    settings.setValue(key, value);
}

QString CredendialsController::read(QString key)
{
    qDebug() << "Reading value for " << key;
    QSettings settings("rideNow", "PersistentStorage");
    qDebug() << "Returning " << settings.value(key).toString();
    return settings.value(key).toString();
}

void CredendialsController::clean()
{
    qDebug() << "Clearing setting";
    QSettings settings("rideNow", "PersistentStorage");
    settings.clear();
}
