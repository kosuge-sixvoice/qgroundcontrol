#include "PointOfInterestController.h"
#include "PointOfInterest.h"
#include "Vehicle.h"
#include "FirmwarePlugin.h"
#include "MAVLinkProtocol.h"
#include "QGCApplication.h"
#include "ParameterManager.h"
#include "JsonHelper.h"
#include "SimpleMissionItem.h"
#include "QGroundControlQmlGlobal.h"
#include "SettingsManager.h"
#include "AppSettings.h"
#include "PlanMasterController.h"

#ifndef __mobile__
#include "QGCQFileDialog.h"
#endif

#include <QJsonDocument>
#include <QJsonArray>

QGC_LOGGING_CATEGORY(PointOfInterestControllerLog, "PointOfInterestControllerLog")

const char* PointOfInterestController::_jsonFileTypeValue =  "PointsOfInterest";
const char* PointOfInterestController::_jsonPointsKey =      "points";

PointOfInterestController::PointOfInterestController(QObject* parent)
    : _currentPOI(NULL)
{
    connect(&_points, &QmlObjectListModel::countChanged, this, &PointOfInterestController::_updateContainsItems);
}

PointOfInterestController::~PointOfInterestController()
{

}

void PointOfInterestController::load(const QString& filename)
{
    QString errorString;
    QString errorMessage = tr("Error reading Plan file (%1). %2").arg(filename).arg("%1");

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorString = file.errorString() + QStringLiteral(" ") + filename;
        qgcApp()->showMessage(errorMessage.arg(errorString));
        return;
    }

    QString fileExtension(".%1");
        QJsonDocument   jsonDoc;
        QByteArray      bytes = file.readAll();

        if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorString)) {
            qgcApp()->showMessage(errorMessage.arg(errorString));
            return;
        }

        int version;
        QJsonObject json = jsonDoc.object();
//        if (!JsonHelper::validateQGCJsonFile(json, _planFileType, _planFileVersion, _planFileVersion, version, errorString)) {
//            qgcApp()->showMessage(errorMessage.arg(errorString));
//            return;
//        }

//        QList<JsonHelper::KeyValidateInfo> rgKeyInfo = { _jsonPOIObjectKey, QJsonValue::Object, true };

//        if (!JsonHelper::validateKeys(json, rgKeyInfo, errorString)) {
//            qgcApp()->showMessage(errorMessage.arg(errorString));
//            return;
//        }

        if (!loadJson(json, errorString)) {
            qgcApp()->showMessage(errorMessage.arg(errorString));
        }
}

bool PointOfInterestController::loadJson(const QJsonObject& json, QString& errorString)
{
    QString errorStr;
    QString errorMessage = tr("Rally: %1");

    // Check for required keys
    QStringList requiredKeys = { _jsonPointsKey };
    if (!JsonHelper::validateRequiredKeys(json, requiredKeys, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return false;
    }

    const QJsonArray rgMissionItems(json[_jsonPointsKey].toArray());



//    QList<QGeoCoordinate> rgPoints;
//    if (!JsonHelper::loadGeoCoordinateArray(json[_jsonPointsKey], true /* altitudeRequired */, rgPoints, errorStr)) {
//        errorString = errorMessage.arg(errorStr);
//        return false;
//    }
//    _points.clearAndDeleteContents();
//    QObjectList pointList;
//    for (int i=0; i<rgPoints.count(); i++) {
//        pointList.append(new PointOfInterest(rgPoints[i], this));
//    }
//    _points.swapObjectList(pointList);

//    _setFirstPointCurrent();

    return true;
}

void PointOfInterestController::save(const QString& filename)
{
    if (filename.isEmpty()) {
        return;
    }

    QString planFilename = filename;
//    if (!QFileInfo(filename).fileName().contains(".")) {
//        planFilename += QString(".%1").arg(fileExtension());
//    }

    QFile file(planFilename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qgcApp()->showMessage(tr("Plan save error %1 : %2").arg(filename).arg(file.errorString()));
    } else {
        QJsonObject json;


        //JsonHelper::saveQGCJsonFileHeader(planJson, _planFileType, _planFileVersion);


        json[JsonHelper::jsonVersionKey] = 1;

        QJsonArray rgPoints;
        QJsonValue jsonPoint;
        for (int i=0; i<_points.count(); i++) {
            JsonHelper::saveGeoCoordinate(qobject_cast<PointOfInterest*>(_points[i])->coordinate(), true /* writeAltitude */, jsonPoint);
            rgPoints.append(jsonPoint);
        }
        json[_jsonPointsKey] = QJsonValue(rgPoints);

        QJsonDocument saveDoc(json);
        file.write(saveDoc.toJson());
    }
}

void PointOfInterestController::removeAll(void)
{
    _points.clearAndDeleteContents();
    setCurrentPOI(NULL);
}

void PointOfInterestController::addPoint(QGeoCoordinate point, QString& pictureName)
{
    qCDebug(PointOfInterestControllerLog) << "addPoint:" << point;

    double defaultAlt;
    if (_points.count()) {
        defaultAlt = qobject_cast<PointOfInterest*>(_points[_points.count() - 1])->coordinate().altitude();
    } else {
        defaultAlt = qgcApp()->toolbox()->settingsManager()->appSettings()->defaultMissionItemAltitude()->rawValue().toDouble();
    }
    point.setAltitude(defaultAlt);
    PointOfInterest* newPoint = new PointOfInterest(point, pictureName, this);
    _points.append(newPoint);
    qCDebug(PointOfInterestControllerLog) << "_points:" << _points.count();
    setCurrentPOI(newPoint);
}

void PointOfInterestController::removePoint(QObject* rallyPoint)
{
    int foundIndex = 0;
    for (foundIndex=0; foundIndex<_points.count(); foundIndex++) {
        if (_points[foundIndex] == rallyPoint) {
            _points.removeOne(rallyPoint);
            rallyPoint->deleteLater();
        }
    }

    if (_points.count()) {
        int newIndex = qMin(foundIndex, _points.count() - 1);
        newIndex = qMax(newIndex, 0);
        setCurrentPOI(_points[newIndex]);
    } else {
        setCurrentPOI(NULL);
    }
}

void PointOfInterestController::setCurrentPOI(QObject* rallyPoint)
{
    if (_currentPOI != rallyPoint) {
        _currentPOI = rallyPoint;
        emit currentPOIChanged(rallyPoint);
    }
}

void PointOfInterestController::_setFirstPointCurrent(void)
{
    setCurrentPOI(_points.count() ? _points[0] : NULL);
}

bool PointOfInterestController::containsItems(void) const
{
    return _points.count() > 0;
}

void PointOfInterestController::_updateContainsItems(void)
{
    emit containsItemsChanged(containsItems());
}
