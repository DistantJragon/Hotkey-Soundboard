#include "ui/soundboard/soundboardupdater.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QSettings>
#include <QVersionNumber>

SoundboardUpdater::SoundboardUpdater(QUrl updateMetaUrl, QObject* parent)
    : QObject(parent), updateCheckTimer(new QTimer(this)),
      updateMetaUrl(updateMetaUrl) {
  connect(updateCheckTimer, &QTimer::timeout, this,
          [this]() { checkForUpdates(); });
  QSettings settings;
  currentVersionStr = settings.value("appVersion", "0.0.0-dev").toString();
}

void SoundboardUpdater::checkForUpdates() {
  QNetworkAccessManager* manager = new QNetworkAccessManager(this);
  QNetworkRequest request(updateMetaUrl);
  QNetworkReply* reply = manager->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    if (reply->error() != QNetworkReply::NoError) {
      emit error("Network error: " + reply->errorString());
      reply->deleteLater();
      return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
      emit error("Invalid JSON response.");
      reply->deleteLater();
      return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    if (!jsonObj.contains("hotkeysoundboard") ||
        !jsonObj["hotkeysoundboard"].isString()) {
      emit error("Missing fields in update metadata.");
      reply->deleteLater();
      return;
    }
    QString latestVersionStr = jsonObj["hotkeysoundboard"].toString();

    if (latestVersionStr.isEmpty()) {
      emit error("Missing fields in update metadata.");
      reply->deleteLater();
      return;
    }

    int cmpResult = compareVersions(currentVersionStr, latestVersionStr);
    if (cmpResult < 0) {
      emit updateAvailable(latestVersionStr);
    } else {
      qDebug("No update available. Current version (%s) is up to date.",
             qPrintable(currentVersionStr));
    }

    reply->deleteLater();
  });
}

void SoundboardUpdater::startPeriodicUpdateChecks(int intervalHours) {
  if (intervalHours <= 0) {
    emit error("Interval must be greater than zero.");
    return;
  }
#ifdef HKSBNDEBUG
  int intervalMs = intervalHours * 60 * 60 * 1000;
#else
  int intervalMs = intervalHours * 60 * 1000;
#endif // HKSBNDEBUG
  updateCheckTimer->start(intervalMs);
}

void SoundboardUpdater::stopPeriodicUpdateChecks() { updateCheckTimer->stop(); }

int SoundboardUpdater::compareVersions(const QString& v1Str,
                                       const QString& v2Str) const {
  qsizetype hyphenIndex1 = 0;
  QVersionNumber ver1 = QVersionNumber::fromString(v1Str, &hyphenIndex1);
  qsizetype hyphenIndex2 = 0;
  QVersionNumber ver2 = QVersionNumber::fromString(v2Str, &hyphenIndex2);
  int cmp = QVersionNumber::compare(ver1, ver2);
  if (cmp != 0) {
    return cmp;
  } else {
    // Versions are equal barring suffix, compare suffixes if present
    if (hyphenIndex1 == v1Str.size() && hyphenIndex2 == v2Str.size()) {
      return 0;
    } else if (hyphenIndex1 == v1Str.size()) {
      // v1 has no suffix, considered greater
      return 1;
    } else if (hyphenIndex2 == v2Str.size()) {
      // v2 has no suffix, considered greater
      return -1;
    }
    QString suffix1 = (hyphenIndex1 != -1) ? v1Str.mid(hyphenIndex1 + 1) : "";
    QString suffix2 = (hyphenIndex2 != -1) ? v2Str.mid(hyphenIndex2 + 1) : "";
    qDebug("Comparing suffixes: '%s' vs '%s'", qPrintable(suffix1),
           qPrintable(suffix2));
    QStringList identifiers1 = suffix1.split('.');
    QStringList identifiers2 = suffix2.split('.');
    int len = qMin(identifiers1.size(), identifiers2.size());

    // Compare each identifier in the suffix
    for (int i = 0; i < len; ++i) {
      bool isInt1, isInt2;
      int part1 = identifiers1[i].toInt(&isInt1);
      int part2 = identifiers2[i].toInt(&isInt2);
      if (isInt1 && isInt2) {
        if (part1 < part2) {
          return -1;
        } else if (part1 > part2) {
          return 1;
        } else {
          continue;
        }
      } else if (isInt1) {
        return -1;
      } else if (isInt2) {
        return 1;
      } else {
        int strCmp = QString::compare(identifiers1[i], identifiers2[i]);
        if (strCmp != 0) {
          return strCmp;
        } else {
          continue;
        }
      }
    }

    // Each pair of identifiers compared equal
    if (identifiers1.size() < identifiers2.size()) {
      return -1;
    } else if (identifiers1.size() > identifiers2.size()) {
      return 1;
    } else {
      return 0;
    }
  }
}
