/*
 * ClustersController.h
 *
 *  Created on: 1 gru 2015
 *      Author: Kamil
 */

#ifndef CLUSTERSCONTROLLER_H_
#define CLUSTERSCONTROLLER_H_

#include "src/interfaces/IClusterCreator.h"
#include "src/data/Cluster.h"

#include <QObject>

class ClustersController: public QObject
{
    Q_OBJECT
public:
    ClustersController(QObject* parent = 0);
    virtual ~ClustersController();

    /*!
     * Sets algorithm for clustering - cluster creator
     *
     * \param Algorithm implementing IClusterCreator which will create clusters
     */
    void setClusterCreator(IClusterCreator* creator);
    /*!
     * Inserts points to Cluster and create clustering by provided IClusterCreator algorithm
     *
     * \param points List of points to be showed on the map
     */
    void insertPoints(QList<IPointOnMap*> points);

    void adjustClusters(double bboxW, double bboxE, double bboxS, double bboxN);

    signals:
    void updateView(QList<Cluster*> &clusters);

private:
    IClusterCreator* m_clusterCreator;
    double m_userLat; // TODO: extract to other controller
    double m_userLon; // TODO: extract to other controller
    double m_radiusToConsider; // TODO: extract to other controller
    Cluster* m_root;
};

#endif /* CLUSTERSCONTROLLER_H_ */
