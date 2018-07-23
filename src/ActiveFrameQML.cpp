/*
 * ActiveFrameQML.cpp
 *
 *  Created on: 29 lis 2015
 *      Author: Kamil
 */

#include <src/ActiveFrameQML.h>
#include <bb/cascades/SceneCover>
#include <bb/cascades/Container>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>

using namespace bb::cascades;

ActiveFrameQML::ActiveFrameQML(QObject *parent)
: SceneCover(parent)
{
    QmlDocument *qml = QmlDocument::create("asset:///AppCover.qml").parent(parent);
    Container *mainContainer = qml->createRootObject<Container>();
    setContent(mainContainer);

    m_coverLabelTimeLeft = mainContainer->findChild<Label*>("statusLabelActiveFrame");
    m_coverLabelTimeLeft->setParent(mainContainer);
    m_coverLabelName = mainContainer->findChild<Label*>("nameLabelActiveFrame");
    m_coverLabelName->setParent(mainContainer);
    m_containerTimer = mainContainer->findChild<Container*>("timerContainer");
    m_containerTimer->setParent(mainContainer);
}

ActiveFrameQML::~ActiveFrameQML()
{} // nothing to destroy

void ActiveFrameQML::updateTimeLeft(QString timeLeft) {
    if (timeLeft.isEmpty() || timeLeft.compare("0:00") == 0){
        m_containerTimer->setVisible(false);
        updateTitleLabel("");
    } else {
        m_containerTimer->setVisible(true);
        m_coverLabelTimeLeft->setText(timeLeft);
    }
}

void ActiveFrameQML::updateTitleLabel(QString carName)
{
    m_coverLabelName->setText(carName);
}
