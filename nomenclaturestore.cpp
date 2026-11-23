#include "nomenclaturestore.h"

#include "renamecore.h"

#include <QSettings>

NomenclatureStore::NomenclatureStore(QSettings &settings)
    : settings(settings)
{
}

QStringList NomenclatureStore::entries() const
{
    return settings.value(QStringLiteral("nomenclatures")).toStringList();
}

bool NomenclatureStore::add(const QString &nomenclature)
{
    QStringList saved = entries();
    if (!RenameCore::validateNomenclature(nomenclature).isEmpty() || saved.contains(nomenclature)) {
        return false;
    }

    saved.append(nomenclature);
    settings.setValue(QStringLiteral("nomenclatures"), saved);
    return true;
}

bool NomenclatureStore::remove(const QString &nomenclature)
{
    QStringList saved = entries();
    if (!saved.removeOne(nomenclature)) {
        return false;
    }

    settings.setValue(QStringLiteral("nomenclatures"), saved);
    return true;
}

QString NomenclatureStore::lastUsed() const
{
    return settings.value(QStringLiteral("lastNomenclature")).toString();
}

void NomenclatureStore::setLastUsed(const QString &nomenclature)
{
    settings.setValue(QStringLiteral("lastNomenclature"), nomenclature);
}
