/*
 * BinaryTreeStructure.cpp
 *
 *  Created on: 2 gru 2015
 *      Author: Kamil
 */

#include <src/clusteringAlgorithms/BinaryTreeStructure.h>
#include <src/helpers/Helper.h>
#include "QDebug.h"
#define NAME __FUNCTION__

BinaryTreeStructure::BinaryTreeStructure(QObject* parent) : IClusterCreator(parent)
{
}

BinaryTreeStructure::~BinaryTreeStructure()
{
}

Cluster* BinaryTreeStructure::insertDataAndBuildClusters(QList<IPointOnMap*> listOdPoints)
{
    qDebug() << NAME;
    if (listOdPoints.length() < 1){
        qWarning() << "List of points to build cluster is < 1 !!";
        return NULL;
    }
    if (listOdPoints.length() < 3){
        Cluster* cluster = new Cluster();
        double newLat = listOdPoints.at(0)->getLatitude();
        double newLon = listOdPoints.at(0)->getLongitude();
        cluster->setLatitude(newLat);
        cluster->setLongitude(newLon);
        for (int i = 0; i < listOdPoints.length(); i++){
            cluster->addContent(listOdPoints.at(i));
        }
        return cluster;
    }
    QList<Cluster*> initialList;
        Cluster* cluster;
        double ** distances = new double*[listOdPoints.length()];
        double MAX = 100000;
        double minDist = MAX;
        double foundIndex = -1;
        QList<int> usedIndexes;
        for (int i = 0; i < listOdPoints.length(); i++){
            distances[i] = new double[listOdPoints.length()];
            if (usedIndexes.contains(i)){
                continue;
            }
            for (int j = i + 1; j < listOdPoints.length(); j++){
                if (usedIndexes.contains(j)){
                    continue;
                }
                distances[i][j] = Helper::distance(listOdPoints.at(i)->getLatitude(), listOdPoints.at(i)->getLongitude(), listOdPoints.at(j)->getLatitude(), listOdPoints.at(j)->getLongitude());
    //            qDebug() << QString("i=%6 j=%7\nCluster 1\tlat=%1 lon=%2\nCluster2\tlat=%3 lon=%4\nDistance = %5\n").arg(cars.at(i)->getLatitude())
    //                                                                                                                .arg(cars.at(i)->getLongitude())
    //                                                                                                                .arg(cars.at(j)->getLatitude())
    //                                                                                                                .arg(cars.at(j)->getLongitude())
    //                                                                                                                .arg(distances[i][j])
    //                                                                                                                .arg(i)
    //                                                                                                                .arg(j);
                if (distances[i][j] < minDist){
                    minDist = distances[i][j];
                    foundIndex = j;
                }
            }
            if (foundIndex != -1){
                usedIndexes.append(foundIndex);
                cluster = new Cluster();
                double newLat = (listOdPoints.at(i)->getLatitude() + listOdPoints.at(foundIndex)->getLatitude()) / 2;
                double newLon = (listOdPoints.at(i)->getLongitude() + listOdPoints.at(foundIndex)->getLongitude()) / 2;
                cluster->setLatitude(newLat);
                cluster->setLongitude(newLon);
                cluster->addContent(listOdPoints.at(i));
                cluster->addContent(listOdPoints.at(foundIndex));
                initialList.append(cluster);
                minDist = MAX;
                foundIndex = -1;
            }
        }
        QList<Cluster*> listCl = buildClustersWithRoot(initialList);
        return listCl.first();
}

QList<Cluster*> BinaryTreeStructure::buildClustersWithRoot(QList<Cluster*> list)
{
    qDebug() << NAME;
    if (list.length() == 1){
        return list;
    }
    QList<Cluster*> listToReturn;;
    Cluster* cluster;
    double MAX = 100000; // 100km
    double minDist = MAX;
    double foundIndex = -1;
    QList<int> usedIndexes;
    for (int i = 0; i < list.length(); i++){
        if (usedIndexes.contains(i)){
            continue;
        }
        for (int j = i + 1; j < list.length(); j++){
            if (usedIndexes.contains(j)){
                continue;
            }
            double dist = Helper::distance(list.at(i)->getLatitude(), list.at(i)->getLongitude(), list.at(j)->getLatitude(), list.at(j)->getLongitude());
            if (dist < minDist){
                foundIndex = j;
                minDist = dist;
            }
        }
        if (foundIndex != -1){
            usedIndexes.append(foundIndex);
            cluster = new Cluster();
            cluster->addChild(list.at(i));
            cluster->addChild(list.at(foundIndex));
            listToReturn.append(cluster);
            minDist = MAX;
            foundIndex = -1;
        }
    }
    return buildClustersWithRoot(listToReturn);
}

QList<Cluster*> BinaryTreeStructure::organizeClustersWithoutRoot(QList<Cluster*> listToOrganize,
        int howManyOnZeroLevelAtMax)
{
    qDebug() << NAME;
    if (listToOrganize.length() < 2){
        qWarning() << "Try to arrange less than two elements into binary tree";
        return QList<Cluster*>();
    }
    if (howManyOnZeroLevelAtMax < 1){
        qWarning() << "Tries to arrange to less that 1 at zero level";
        return QList<Cluster*>();
    }
    int desiredNumberOfclustersAtZeroLevel = listToOrganize.length();
    while (desiredNumberOfclustersAtZeroLevel > howManyOnZeroLevelAtMax) {
        qDebug() << "Searching for a number of clusters to show";
        qDebug() << "Found number so far = " << desiredNumberOfclustersAtZeroLevel;
        qDebug() << "Expected max number = " << howManyOnZeroLevelAtMax;
        desiredNumberOfclustersAtZeroLevel /= 2;
    };
    qDebug() << "Going to have " << desiredNumberOfclustersAtZeroLevel << " elements on the zero level";
    int clustersInEachRoot = ceil(listToOrganize.length() / desiredNumberOfclustersAtZeroLevel);
    QList<Cluster*> listToReturn;
    for (int i = 0; i < listToOrganize.length(); i+=clustersInEachRoot){
        QList<Cluster*> subList;
        for (int j = 0; j < clustersInEachRoot && i + j < listToOrganize.length(); j++){
            subList.append(listToOrganize.at(i + j));
        }
        listToReturn.append(buildClustersWithRoot(subList).first());
    }
    return listToReturn;
}
