/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include <QStringList>
#include <QDebug>

#include "PointOfInterest.h"

const char* PointOfInterest::_longitudeFactName =    "Longitude";
const char* PointOfInterest::_latitudeFactName =     "Latitude";
const char* PointOfInterest::_altitudeFactName =     "Altitude";

QMap<QString, FactMetaData*> PointOfInterest::_metaDataMap;

PointOfInterest::PointOfInterest(const QGeoCoordinate& coordinate, const QString& pictureName, QObject* parent)
    : QObject(parent)
    , _dirty(false)
    , _longitudeFact(0, _longitudeFactName, FactMetaData::valueTypeDouble)
    , _latitudeFact(0, _latitudeFactName, FactMetaData::valueTypeDouble)
    , _altitudeFact(0, _altitudeFactName, FactMetaData::valueTypeDouble)
    , _pictureName(pictureName)
{
    setCoordinate(coordinate);

    _factSetup();
}

PointOfInterest::PointOfInterest(const RallyPoint& other, QObject* parent)
    : QObject(parent)
    , _dirty(false)
    , _longitudeFact(0, _longitudeFactName, FactMetaData::valueTypeDouble)
    , _latitudeFact(0, _latitudeFactName, FactMetaData::valueTypeDouble)
    , _altitudeFact(0, _altitudeFactName, FactMetaData::valueTypeDouble)
{
    _longitudeFact.setRawValue(other._longitudeFact.rawValue());
    _latitudeFact.setRawValue(other._latitudeFact.rawValue());
    _altitudeFact.setRawValue(other._altitudeFact.rawValue());

    _factSetup();
}

const PointOfInterest& PointOfInterest::operator=(const PointOfInterest& other)
{
    _longitudeFact.setRawValue(other._longitudeFact.rawValue());
    _latitudeFact.setRawValue(other._latitudeFact.rawValue());
    _altitudeFact.setRawValue(other._altitudeFact.rawValue());

    emit coordinateChanged(coordinate());

    return *this;
}

PointOfInterest::~PointOfInterest()
{    

}

void PointOfInterest::_factSetup(void)
{
    if (_metaDataMap.isEmpty()) {
        _metaDataMap = FactMetaData::createMapFromJsonFile(QStringLiteral(":/json/RallyPoint.FactMetaData.json"), NULL /* metaDataParent */);
    }

    _longitudeFact.setMetaData(_metaDataMap[_longitudeFactName]);
    _latitudeFact.setMetaData(_metaDataMap[_latitudeFactName]);
    _altitudeFact.setMetaData(_metaDataMap[_altitudeFactName]);

    _textFieldFacts.append(QVariant::fromValue(&_longitudeFact));
    _textFieldFacts.append(QVariant::fromValue(&_latitudeFact));
    _textFieldFacts.append(QVariant::fromValue(&_altitudeFact));

    connect(&_longitudeFact, &Fact::valueChanged, this, &RallyPoint::_sendCoordinateChanged);
    connect(&_latitudeFact, &Fact::valueChanged, this, &RallyPoint::_sendCoordinateChanged);
    connect(&_altitudeFact, &Fact::valueChanged, this, &RallyPoint::_sendCoordinateChanged);
}

void PointOfInterest::setCoordinate(const QGeoCoordinate& coordinate)
{
    if (coordinate != this->coordinate()) {
        _longitudeFact.setRawValue(coordinate.longitude());
        _latitudeFact.setRawValue(coordinate.latitude());
        _altitudeFact.setRawValue(coordinate.altitude());
        emit coordinateChanged(coordinate);
        setDirty(true);
    }
}

QGeoCoordinate PointOfInterest::coordinate(void) const
{
    return QGeoCoordinate(_latitudeFact.rawValue().toDouble(), _longitudeFact.rawValue().toDouble(), _altitudeFact.rawValue().toDouble());
}

void PointOfInterest::_sendCoordinateChanged(void)
{
    emit coordinateChanged(coordinate());
}
