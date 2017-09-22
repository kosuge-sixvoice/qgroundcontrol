#include "PointOfInterestController.h"
#include "RallyPoint.h"
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

QGC_LOGGING_CATEGORY(PointOfInterestController, "PointOfInterestControllerLog")

const char* PointOfInterestController::_jsonFileTypeValue =  "PointsOfInterest";
const char* PointOfInterestController::_jsonPointsKey =      "points";

PointOfInterestController::PointOfInterestController(PlanMasterController* masterController, QObject* parent)
    : PlanElementController(masterController, parent)
    , _currentRallyPoint(NULL)
{
    connect(&_points, &QmlObjectListModel::countChanged, this, &PointOfInterestController::_updateContainsItems);
}

PointOfInterestController::~PointOfInterestController()
{

}

bool PointOfInterestController::load(const QJsonObject& json, QString& errorString)
{
    QString errorStr;
    QString errorMessage = tr("Rally: %1");

    // Check for required keys
    QStringList requiredKeys = { _jsonPointsKey };
    if (!JsonHelper::validateRequiredKeys(json, requiredKeys, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return false;
    }

    QList<QGeoCoordinate> rgPoints;
    if (!JsonHelper::loadGeoCoordinateArray(json[_jsonPointsKey], true /* altitudeRequired */, rgPoints, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return false;
    }
    _points.clearAndDeleteContents();
    QObjectList pointList;
    for (int i=0; i<rgPoints.count(); i++) {
        pointList.append(new RallyPoint(rgPoints[i], this));
    }
    _points.swapObjectList(pointList);

    setDirty(false);
    _setFirstPointCurrent();

    return true;
}

void PointOfInterestController::save(QJsonObject& json)
{
    json[JsonHelper::jsonVersionKey] = 1;

    QJsonArray rgPoints;
    QJsonValue jsonPoint;
    for (int i=0; i<_points.count(); i++) {
        JsonHelper::saveGeoCoordinate(qobject_cast<RallyPoint*>(_points[i])->coordinate(), true /* writeAltitude */, jsonPoint);
        rgPoints.append(jsonPoint);
    }
    json[_jsonPointsKey] = QJsonValue(rgPoints);
}

void PointOfInterestController::removeAll(void)
{
    _points.clearAndDeleteContents();
    setCurrentPOI(NULL);
}

QString PointOfInterestController::editorQml(void) const
{
    return _rallyPointManager->editorQml();
}

void PointOfInterestController::addPoint(QGeoCoordinate point)
{
    double defaultAlt;
    if (_points.count()) {
        defaultAlt = qobject_cast<RallyPoint*>(_points[_points.count() - 1])->coordinate().altitude();
    } else {
        defaultAlt = qgcApp()->toolbox()->settingsManager()->appSettings()->defaultMissionItemAltitude()->rawValue().toDouble();
    }
    point.setAltitude(defaultAlt);
    RallyPoint* newPoint = new RallyPoint(point, this);
    _points.append(newPoint);
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
    if (_currentRallyPoint != rallyPoint) {
        _currentRallyPoint = rallyPoint;
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
