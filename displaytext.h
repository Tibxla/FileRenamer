#ifndef DISPLAYTEXT_H
#define DISPLAYTEXT_H

#include <QString>

// Neutralise les sauts de ligne d'un nom de fichier ou d'un chemin pour l'afficher sur une ligne,
// comme le fait RenameCore::previewText.
inline QString singleLineText(QString text)
{
    return text.replace(QLatin1Char('\n'), QStringLiteral("\\n"))
               .replace(QLatin1Char('\r'), QStringLiteral("\\r"))
               .replace(QChar::LineSeparator, QStringLiteral("\\u2028"))
               .replace(QChar::ParagraphSeparator, QStringLiteral("\\u2029"));
}

#endif
