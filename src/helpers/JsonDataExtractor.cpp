/*
 * JsonDataExtractor.cpp
 *
 *  Created on: 11 gru 2015
 *      Author: Kamil
 */

#include <src/helpers/JsonDataExtractor.h>
#include <qdebug.h>

namespace driveNowJsonExtractor
{

    JsonDataExtractor::JsonDataExtractor()
    {
        // TODO Auto-generated constructor stub

    }

    JsonDataExtractor::~JsonDataExtractor()
    {
        // TODO Auto-generated destructor stub
    }

    QString JsonDataExtractor::extractStreet(QVariantMap var)
    {
        if (var.contains("address")){
            if (var["address"].toList().length() > 0){
                return var["address"].toList().at(0).toString();
            }
        }
        return QString::null;
    }

    QString JsonDataExtractor::extractZipCodeAndCity(QVariantMap var)
    {
        if (var.contains("address")){
            if (var["address"].toList().length() > 1){
                return var["address"].toList().at(1).toString();
            }
        }
        return QString::null;
    }

    double JsonDataExtractor::extractLatitide(QVariantMap var)// TODO: check correctness of conversion
    {
        if (var.contains("latitude")){
            return var["latitude"].toDouble();
        }
        return 0;
    }

    double JsonDataExtractor::extractLongitude(QVariantMap var)// TODO: check correctness of conversion
    {
        if (var.contains("longitude")){
            return var["longitude"].toDouble();
        }
        return 0;
    }

    QString JsonDataExtractor::extractLicencePlate(QVariantMap var)
    {
        if (var.contains("licensePlate")){
            return var["licensePlate"].toString();
        }
        return QString::null;
    }

    QString JsonDataExtractor::extractId(QVariantMap var)
    {
        if (var.contains("id")){
            return var["id"].toString();
        }
        return QString::null;
    }

    QString JsonDataExtractor::extractModelName(QVariantMap var)
    {
        if (var.contains("modelName")){
            return var["modelName"].toString();
        }
        return QString::null;
    }

    QString JsonDataExtractor::extractMaker(QVariantMap var)
    {
        if (var.contains("make")){
            return var["make"].toString();
        }
        return QString::null;
    }

    QString JsonDataExtractor::extractName(QVariantMap var)
    {
        if (var.contains("name")){
            return var["name"].toString();
        }
        return QString::null;
    }

    int JsonDataExtractor::extractFuelLevel(QVariantMap var)// TODO: check correctness of conversion
    {
        if (var.contains("fuelLevel")){
            return var["fuelLevel"].toFloat() * 100;
        }
        return -1;
    }

    QString JsonDataExtractor::extractTransmission(QVariantMap var)
    {
        if (var.contains("transmission")){
            return var["transmission"].toString();
        }
        return QString::null;
    }
}
