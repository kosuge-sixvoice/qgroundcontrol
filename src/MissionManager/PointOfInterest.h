/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef PointOfInterest_H
#define PointOfInterest_H

#include <QObject>
#include <QGeoCoordinate>

#include "FactSystem.h"

/// This class is used to encapsulate the QGeoCoordinate associated with a Rally Point into a QObject such
/// that it can be used in a QmlObjectListMode for Qml.
class PointOfInterest : public QObject
{
    Q_OBJECT
    
public:
    PointOfInterest(const QGeoCoordinate& coordinate, const QString& pictureName, QObject* parent = NULL);
    PointOfInterest(const PointOfInterest& other, QObject* parent = NULL);

    ~PointOfInterest();

    const PointOfInterest& operator=(const PointOfInterest& other);
    
    Q_PROPERTY(QGeoCoordinate   coordinate      READ coordinate     WRITE setCoordinate     NOTIFY coordinateChanged)
    Q_PROPERTY(bool             dirty           READ dirty          WRITE setDirty          NOTIFY dirtyChanged)
    Q_PROPERTY(QVariantList     textFieldFacts  MEMBER _textFieldFacts                      CONSTANT)
    Q_PROPERTY(QString          pictureName     READ pictureName    WRITE setPictureName    NOTIFY pictureNameChanged)

    QGeoCoordinate coordinate(void) const;
    void setCoordinate(const QGeoCoordinate& coordinate);
    QString pictureName(void) const;

    bool dirty(void) const { return _dirty; }
    void setDirty(bool dirty);
    void setPictureName(const QString& pictureName);

signals:
    void coordinateChanged      (const QGeoCoordinate& coordinate);
    void dirtyChanged           (bool dirty);
    void pictureNameChanged     (const QString& pictureName);

private slots:
    void _sendCoordinateChanged(void);

private:
    void _factSetup(void);

    bool _dirty;
    Fact _longitudeFact;
    Fact _latitudeFact;
    Fact _altitudeFact;

    QString _pictureName;

    QVariantList _textFieldFacts;

    static QMap<QString, FactMetaData*> _metaDataMap;

    static const char* _longitudeFactName;
    static const char* _latitudeFactName;
    static const char* _altitudeFactName;
};

#endif
