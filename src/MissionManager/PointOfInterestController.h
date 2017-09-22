#ifndef POINTOFINTERESTCONTROLLER_H
#define POINTOFINTERESTCONTROLLER_H
#include "PlanElementController.h"
#include "RallyPointManager.h"
#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"
#include "QmlObjectListModel.h"

class PointOfInterestController : public PlanElementController
{
    Q_OBJECT

public:
    PointOfInterestController(PlanMasterController* masterController, QObject* parent = NULL);
    ~PointOfInterestController();

    Q_PROPERTY(QmlObjectListModel*  points                  READ points                                             CONSTANT)
    Q_PROPERTY(QString              editorQml               READ editorQml                                          CONSTANT)
    Q_PROPERTY(QObject*             currentPOI              READ currentP      WRITE setCurrentPOI  NOTIFY currentPOIChanged)

    Q_INVOKABLE void addPoint(QGeoCoordinate point);
    Q_INVOKABLE void removePoint(QObject* point);

    void save                       (QJsonObject& json) final;
    bool load                       (const QJsonObject& json, QString& errorString) final;
    void removeAll                  (void) final;

    bool containsItems              (void) const final;

    QmlObjectListModel* points           (void) { return &_points; }
    QString             editorQml        (void) const;
    QObject*            currentPOI       (void) const { return _currentRallyPoint; }

    void setCurrentPOI(QObject* rallyPoint);

signals:
    void rallyPointsSupportedChanged(bool rallyPointsSupported);
    void currentPOIChanged(QObject* rallyPoint);
    void loadComplete(void);

private slots:
    void _managerLoadComplete(const QList<QGeoCoordinate> rgPoints);
    void _managerSendComplete(bool error);
    void _managerRemoveAllComplete(bool error);
    void _setFirstPointCurrent(void);
    void _updateContainsItems(void);

private:
    RallyPointManager*  _rallyPointManager;
    bool                _dirty;
    QmlObjectListModel  _points;
    QObject*            _currentRallyPoint;
    bool                _itemsRequested;

    static const char* _jsonFileTypeValue;
    static const char* _jsonPointsKey;
};

#endif // POINTOFINTERESTCONTROLLER_H
