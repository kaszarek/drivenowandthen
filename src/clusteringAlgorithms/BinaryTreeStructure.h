/*
 * BinaryTreeStructure.h
 *
 *  Created on: 2 gru 2015
 *      Author: Kamil
 */

#ifndef BINARYTREESTRUCTURE_H_
#define BINARYTREESTRUCTURE_H_

#include "src/interfaces/IClusterCreator.h"
#include <QObject>

class BinaryTreeStructure: public IClusterCreator
{
    Q_OBJECT
public:
    BinaryTreeStructure(QObject* parent = 0);
    virtual ~BinaryTreeStructure();
    Cluster* insertDataAndBuildClusters(QList<IPointOnMap*> list);
    QList<Cluster*> organizeClustersWithoutRoot(QList<Cluster*> listToOrganize, int howManyOnZeroLevel);
signals:
        void clustersCreated(Cluster* root);
private:
    QList<Cluster*> buildClustersWithRoot(QList<Cluster*> list);
};

#endif /* BINARYTREESTRUCTURE_H_ */
