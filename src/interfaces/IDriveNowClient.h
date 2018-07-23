/*
 * IDrtiveNowClient.h
 *
 *  Created on: 14 maj 2017
 *      Author: Kamil
 */

#ifndef IDRTIVENOWCLIENT_H_
#define IDRTIVENOWCLIENT_H_

#include <QObject>

class IDriveNowClient : public QObject{

public:
    IDriveNowClient(QObject* parent = 0) : QObject(parent){};
    virtual ~IDriveNowClient(){};
};

Q_DECLARE_INTERFACE(IDriveNowClient, "IDriveNowClient")

#endif /* IDRTIVENOWCLIENT_H_ */
