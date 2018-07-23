/*
 * Cluster.h
 *
 *  Created on: 30 lis 2015
 *      Author: Kamil
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <QObject>
#include "src/interfaces/IPointOnMap.h"

/*!
 * Container for a points on a map. It generated clusters with provided algorithm
 */
class Cluster: public IPointOnMap
{
    Q_OBJECT
public:
    Cluster(QObject* parent = 0);
    virtual ~Cluster();
    /*!
     * Adds cluster as a child to this
     *
     * \param child Cluster object to be added
     */
    void addChild(Cluster * child);
    /*!
     * Sets children clusters on this
     *
     * \param children List of clusters to be added
     */
    void setChildren(QList<Cluster*> &children);
    /*!
     * Gets content from a specified depth, NOT clusters
     *
     * \param depth Depth that content will be returned from
     * \return List of objects a the given depths
     */
    QList<IPointOnMap*> getContentAtDepth(int depth);
    /*!
     * Gets amount of content under this cluster
     *
     * \return Amount of content.
     */
    int getContentCountBelowCluster();
    /*!
     * Adds content to this cluster
     *
     * \param content QObject to be added
     */
    void addContent(IPointOnMap* content);
    /*!
     * Gets clusters at a given depth
     *
     * \param depth Depth clusters will be returned from. Starts from 0 - top/root cluster
     * \return List of clusters
     */
    QList<Cluster*> getClustersAtDepth(int depth);

    /*!
     * Gets lists of clusters without children which fall in the coordinates limits.
     *
     * \param east The most eastern coordinate to consider.
     * \param west The most western coordinate to consider.
     * \param south The most southern coordinate to consider.
     * \param north The most northern coordinate to consider.
     *
     * \return List of clusters
     */
    QList<Cluster*> getAllContentFromBoundriesBox(double westBorder, double eastBorder, double southBorder, double northBorder);

    /*!
     * Gets amount of clusters at a given level.
     *
     * \param depth Depth to consider.
     * \return Amount of clusters.
     */
    int getClustersCountAtLevel(int depth);

    /*!
     * Gets number of children.
     *
     * \return Number of children.
     */
    int getChildrenCount(){
        return m_clustersBelow.length();
    }

    /*!
     * Gets maximum available depth
     */
    int getMaxAvailableDepth();

private:
    double m_minLat;
    double m_maxLat;
    double m_minLon;
    double m_maxLon;
    QList<Cluster*> m_clustersBelow;
    QList<IPointOnMap*> m_pointsInCluster;
};

#endif /* CLUSTER_H_ */
