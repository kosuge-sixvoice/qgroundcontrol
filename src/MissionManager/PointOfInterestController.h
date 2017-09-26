#ifndef POINTOFINTERESTCONTROLLER_H
#define POINTOFINTERESTCONTROLLER_H

#include "RallyPointManager.h"
#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"
#include "QmlObjectListModel.h"

class PointOfInterestController : public QObject
{
    Q_OBJECT

public:
    PointOfInterestController(QObject* parent = NULL);
    ~PointOfInterestController();

    Q_PROPERTY(QmlObjectListModel*  points                  READ points                                             CONSTANT)
    Q_PROPERTY(QString              editorQml               READ editorQml                                          CONSTANT)
    Q_PROPERTY(QObject*             currentPOI              READ currentPOI      WRITE setCurrentPOI  NOTIFY currentPOIChanged)

    Q_INVOKABLE void addPoint(QGeoCoordinate point);
    Q_INVOKABLE void removePoint(QObject* point);

    Q_INVOKABLE void save                       (const QString& filename);
    Q_INVOKABLE void load                       (const QString& filename);
    Q_INVOKABLE void removeAll                  (void);

    bool loadJson(const QJsonObject& json, QString& errorString);


    bool containsItems              (void) const;

    QmlObjectListModel* points           (void) { return &_points; }
    QString             editorQml        (void) const { return QStringLiteral("qrc:/FirmwarePlugin/RallyPointEditor.qml"); }
    QObject*            currentPOI       (void) const { return _currentPOI; }

    void setCurrentPOI(QObject* rallyPoint);

signals:
     void containsItemsChanged(bool containsItems);
    void currentPOIChanged(QObject* rallyPoint);
    void loadComplete(void);

private slots:
    void _setFirstPointCurrent(void);
    void _updateContainsItems(void);

private:
    QmlObjectListModel  _points;
    QObject*            _currentPOI;

    static const char* _jsonFileTypeValue;
    static const char* _jsonPointsKey;
};

#endif // POINTOFINTERESTCONTROLLER_H
