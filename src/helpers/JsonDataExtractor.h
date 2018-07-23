/*
 * JsonDataExtractor.h
 *
 *  Created on: 11 gru 2015
 *      Author: Kamil
 */

#ifndef JSONDATAEXTRACTOR_H_
#define JSONDATAEXTRACTOR_H_

namespace driveNowJsonExtractor
{
    class JsonDataExtractor
    {
    public:
        JsonDataExtractor();
        virtual ~JsonDataExtractor();

        static QString extractStreet(QVariantMap var);
        static QString extractZipCodeAndCity(QVariantMap var);
        static double extractLatitide(QVariantMap var);
        static double extractLongitude(QVariantMap var);
        static QString extractLicencePlate(QVariantMap var);
        static QString extractId (QVariantMap var);
        static QString extractModelName (QVariantMap var);
        static QString extractMaker (QVariantMap var);
        static QString extractName (QVariantMap var);
        static int extractFuelLevel(QVariantMap var);
        static QString extractTransmission (QVariantMap var);
    };

}
#endif /* JSONDATAEXTRACTOR_H_ */
