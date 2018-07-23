/*
 * CredendialsController.h
 *
 *  Created on: 5 kwi 2016
 *      Author: Kamil
 */

#ifndef CREDENDIALSCONTROLLER_H_
#define CREDENDIALSCONTROLLER_H_
#include <QObject>

class CredendialsController: public QObject
{

    Q_OBJECT
public:
    CredendialsController(QObject *parent = 0);
    virtual ~CredendialsController();

    Q_INVOKABLE void save(QString key, QString value);
    Q_INVOKABLE QString read(QString key);
    Q_INVOKABLE void clean();

private:

};

#endif /* CREDENDIALSCONTROLLER_H_ */
