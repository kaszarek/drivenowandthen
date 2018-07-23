/*
 * ActiveFrameQML.h
 *
 *  Created on: 29 lis 2015
 *      Author: Kamil
 */

#ifndef ACTIVEFRAMEQML_H_
#define ACTIVEFRAMEQML_H_

#include <QObject>
#include <bb/cascades/Label>
#include <bb/cascades/SceneCover>

using namespace ::bb::cascades;

/**
 * Implementation of showing elapsing time for picking up the car on an active frame
 */
class ActiveFrameQML: public SceneCover {
    Q_OBJECT
public:
    ActiveFrameQML(QObject *parent=0);
    virtual ~ActiveFrameQML();

    public slots:
        Q_INVOKABLE void updateTimeLeft(QString appText);
        Q_INVOKABLE void updateTitleLabel(QString appText);

    private:
        bb::cascades::Label *m_coverLabelTimeLeft;
        bb::cascades::Label *m_coverLabelName;
        bb::cascades::Container *m_containerTimer;
};

#endif /* ACTIVEFRAMEQML_H_ */
