#ifndef SOUNDBOARDUPDATER_H
#define SOUNDBOARDUPDATER_H

#include <QObject>
#include <QTimer>
#include <QUrl>

class SoundboardUpdater : public QObject {
  Q_OBJECT
public:
  explicit SoundboardUpdater(QUrl updateMetaUrl, QObject* parent = nullptr);

  void checkForUpdates();

  /*!
   * \brief Compares two version strings using semantic versioning rules.
   * \param v1 First version string.
   * \param v2 Second version string.
   * \return -1 if v1 < v2, 0 if v1 == v2, 1 if v1 > v2.
   */
  int compareVersions(const QString& v1, const QString& v2) const;

  QUrl getUpdateMetaUrl() const { return updateMetaUrl; }
  void setUpdateMetaUrl(const QUrl& url) { updateMetaUrl = url; }

public slots:
  void startPeriodicUpdateChecks(int intervalHours);
  void stopPeriodicUpdateChecks();

signals:
  void updateAvailable(const QString& latestVersion);
  void error(const QString& msg);

private:
  QTimer* updateCheckTimer;
  QUrl updateMetaUrl;
  QString currentVersionStr = "0.0.0-dev";
};

#endif // SOUNDBOARDUPDATER_H
