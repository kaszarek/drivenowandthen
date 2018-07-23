/*
 * Cluster.cpp
 *
 *  Created on: 30 lis 2015
 *      Author: Kamil
 */

#include <src/data/Cluster.h>
#include <qdebug.h>
#include <QtCore/qmath.h>
#define NAME __FUNCTION__

Cluster::Cluster(QObject* parent) : IPointOnMap(parent)
{
    m_clustersBelow = QList<Cluster*>();
    m_pointsInCluster = QList<IPointOnMap*>();
    m_minLat = 100; // max is 90
    m_maxLat = -100; // min is -90
    m_minLon = 190; // max is 180
    m_maxLon = -190; // min is -180
}

Cluster::~Cluster()
{
    // TODO Auto-generated destructor stub
}

void Cluster::addChild(Cluster* child)
{
    qDebug() << NAME;
    m_clustersBelow.append(child);
    if (latitude == 0 || longitude == 0){
        latitude = child->getLatitude();
        longitude = child->getLongitude();
    } else {
        latitude = (latitude  + child->getLatitude()) / 2;
        longitude = (longitude + child->getLongitude()) / 2;
    }
    // TODO: fix below to handle universal place on earth
    if (latitude < m_minLat){
        m_minLat = latitude;
    }
    if (latitude > m_maxLat){
        m_maxLat = latitude;
    }
    if (longitude < m_minLon){
        m_minLon = longitude;
    }
    if (longitude > m_maxLon){
        m_maxLon = longitude;
    }
}

QList<IPointOnMap*> Cluster::getContentAtDepth(int depth)
{
    qDebug() << NAME;
    QList<IPointOnMap*> fullContent;
    if (depth == 0){
        fullContent.append(m_pointsInCluster);
        return fullContent;
    }
    depth--;
    for (int i = 0; i < m_clustersBelow.length(); i++){
        fullContent.append(m_clustersBelow.at(i)->getContentAtDepth(depth));
    }
    return fullContent;
}

void Cluster::addContent(IPointOnMap* content)
{
    qDebug() << NAME;
    m_pointsInCluster.append(content);
}

int Cluster::getContentCountBelowCluster()
{
    qDebug() << NAME;
    int allPoints = 0;
    for (int i = 0; i < m_clustersBelow.length(); i++){
        allPoints += m_clustersBelow.at(i)->getContentCountBelowCluster();
    }
    allPoints += m_pointsInCluster.length();
    //    qDebug() << "All cars in node = " << allPoints;
    return allPoints;
}

void Cluster::setChildren(QList<Cluster*> &children)
{
    qDebug() << NAME;
    m_clustersBelow = children;
    for (int i = 0; i < children.length(); i++){
        addChild(children.at(i));
    }
}

QList<Cluster*> Cluster::getClustersAtDepth(int depth)
{
    qDebug() << NAME;
    QList<Cluster*> desiredClusters;
    if (depth == 0){
        desiredClusters.append(this);
        return desiredClusters;
    }
    depth--;
    for (int i = 0; i < m_clustersBelow.length(); i++){
        desiredClusters.append(m_clustersBelow.at(i)->getClustersAtDepth(depth));
    }
    return desiredClusters;
}

QList<Cluster*> Cluster::getAllContentFromBoundriesBox(double westBorder, double eastBorder, double southBorder,
        double northBorder)
{
    qDebug() << NAME;
    QList<Cluster*> desiredClusters;
    // make an offset so coordinates are always positive
    int offset = 200;
    double safeLeftBoundry = offset + westBorder;
    double safeRightBoundry = offset + eastBorder;
    double safeTopBoundry = offset + northBorder;
    double safeBottomBoundry = offset + southBorder;

    //TODO if there is no clusters below and there is content (cars) then check the cars' coordinated within the boundaries
    double latitideToConsider = 0.0d;
    double longifuteToConsider = 0.0d;
    if (m_clustersBelow.length() == 0){
        for (int i = 0; i < m_pointsInCluster.length(); i++){
            latitideToConsider = offset + m_pointsInCluster.at(i)->getLatitude();
            longifuteToConsider = offset + m_pointsInCluster.at(i)->getLongitude();
            if (safeRightBoundry < longifuteToConsider && longifuteToConsider < safeLeftBoundry &&
                    safeBottomBoundry < latitideToConsider && latitideToConsider < safeTopBoundry){
                desiredClusters.append(this);
                return desiredClusters;
            }
        }
    }
    for (int i = 0; i < m_clustersBelow.length(); i++){
        desiredClusters.append(m_clustersBelow.at(i)->getAllContentFromBoundriesBox(westBorder, eastBorder, southBorder, northBorder));
    }
    return desiredClusters;
}

int Cluster::getClustersCountAtLevel(int depth)
{
    qDebug() << NAME;
    int numberOfCLusters = 0;
    if (depth == 0){
        return (m_clustersBelow.length() != 0) ? (m_pointsInCluster.length() + 1) : m_pointsInCluster.length(); // all points it contains
    }
    depth--;
    for (int i = 0; i < m_clustersBelow.length(); i++){
        numberOfCLusters += m_clustersBelow.at(i)->getClustersCountAtLevel(depth);
    }
    //    qDebug() << "Clusters at [" << depth << "] level = " << numberOfCLusters;
    return numberOfCLusters;
}

int Cluster::getMaxAvailableDepth()
{
    qDebug() << NAME;
    int depth = 1;
    if (m_clustersBelow.length() == 0){
        return depth;
    }
    int deepestSoFar = 0;
    for (int i = 0; i < m_clustersBelow.length(); i++){
        int returnedDepth = m_clustersBelow.at(i)->getMaxAvailableDepth();
        if (returnedDepth > deepestSoFar){
            deepestSoFar = returnedDepth;
        }
    }
    depth += deepestSoFar;
    return depth;
}
