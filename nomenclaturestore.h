#ifndef NOMENCLATURESTORE_H
#define NOMENCLATURESTORE_H

#include <QStringList>

class QSettings;

class NomenclatureStore
{
public:
    explicit NomenclatureStore(QSettings &settings);

    QStringList entries() const;
    bool add(const QString &nomenclature);
    bool remove(const QString &nomenclature);

    QString lastUsed() const;
    void setLastUsed(const QString &nomenclature);

private:
    QSettings &settings;
};

#endif
