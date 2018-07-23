/*
 * IClusterCreator.h
 *
 *  Created on: 1 gru 2015
 *      Author: Kamil
 */

#ifndef ICLUSTERCREATOR_H_
#define ICLUSTERCREATOR_H_

#include "src/data/Cluster.h"
#include <QObject>

class IClusterCreator : public QObject
{

public:
    IClusterCreator(QObject* parent = 0) : QObject(parent){};
    virtual ~IClusterCreator(){};
    virtual QList<Cluster*> buildClustersWithRoot(QList<Cluster*> list) = 0;
    virtual Cluster* insertDataAndBuildClusters(QList<IPointOnMap*> list) = 0;
    virtual QList<Cluster*> organizeClustersWithoutRoot(QList<Cluster*> listToOrganize, int howManyOnZeroLevelAtMax) = 0;
};

Q_DECLARE_INTERFACE(IClusterCreator, "IClusterCreator")


#endif /* ICLUSTERCREATOR_H_ */
