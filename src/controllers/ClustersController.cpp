/*
 * ClustersController.cpp
 *
 *  Created on: 1 gru 2015
 *      Author: Kamil
 */

#include <src/controllers/ClustersController.h>
#include "QDebug.h"
#include "src/clusteringAlgorithms/BinaryTreeStructure.h"
#define NAME __FUNCTION__

ClustersController::ClustersController(QObject* parent) : QObject(parent)
{
    m_userLat = 0;
    m_userLon = 0;
    m_radiusToConsider = 2000; //[m]
    m_root = NULL;
    m_clusterCreator = NULL;
    setClusterCreator(new BinaryTreeStructure());
}

ClustersController::~ClustersController()
{
    // TODO Auto-generated destructor stub
}

void ClustersController::setClusterCreator(IClusterCreator* creator)
{
    if (!m_clusterCreator){
        delete m_clusterCreator;
        m_clusterCreator = NULL;
    }

    m_clusterCreator = creator;
}

void ClustersController::insertPoints(QList<IPointOnMap*> points)
{
    qDebug() << NAME;
    if (m_clusterCreator == NULL){
        qWarning() << "No Cluster Creator was set";
        return;
    }
    if (points.length() < 1){
        qWarning() << "No points to insert";
        return;
    }
    m_root = m_clusterCreator->insertDataAndBuildClusters(points);
    int maxLevel = m_root->getMaxAvailableDepth() - 1;
    int level = (maxLevel >= 7) ? 7 : maxLevel;
    qDebug() << "Going to show clusters from level " << level;
    QList<Cluster*> clustersToShow  = m_root->getClustersAtDepth(level);
    qDebug() << "At level [" << level << "] is " << m_root->getClustersCountAtLevel(level) << "object to show";
}

void ClustersController::adjustClusters(double bboxW, double bboxE, double bboxS, double bboxN)
{
    qDebug() << NAME;
    if (!m_root){
        qWarning() << "root cluster is NULL";
        emit updateView(*(new QList<Cluster*>()));
        return;
    }
    QList<Cluster*> clustersToShow  = m_root->getAllContentFromBoundriesBox(bboxW, bboxE, bboxS, bboxN);
    qDebug() << "Clusters to show has " << QString::number(clustersToShow.length()) << " elements";

    // each cluster holds at max 2 points
    // limit is 15 hence -> it will show at max 30 cars
    const int POINTS_LIMIT = 15;
    if (clustersToShow.length() > POINTS_LIMIT) {// TODO: extract to parameter
        clustersToShow = m_clusterCreator->organizeClustersWithoutRoot(clustersToShow, POINTS_LIMIT);
        qDebug() << "Clusters over limit. After reduction = " << clustersToShow.length() << " elements";
    }
    emit updateView(clustersToShow);
}
