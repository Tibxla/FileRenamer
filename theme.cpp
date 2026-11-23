#include "theme.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QHash>
#include <QIconEngine>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QRegularExpression>
#include <QSvgRenderer>

namespace {

bool darkScheme = false;

struct Token {
    const char *name;
    const char *light;
    const char *dark;
};

// Couleurs reprises des jetons Fluent de Windows 11 (aplats opaques équivalents).
const Token tokens[] = {
    {"scheme", "light", "dark"},
    {"window", "#F3F3F3", "#202020"},
    {"card", "#FFFFFF", "#2B2B2B"},
    {"cardBorder", "#E5E5E5", "#1D1D1D"},
    {"divider", "#EBEBEB", "#383838"},
    {"text", "#1B1B1B", "#FFFFFF"},
    {"secondary", "#5D5D5D", "#CFCFCF"},
    {"tertiary", "#757575", "#A0A0A0"},
    {"disabledText", "#A0A0A0", "#787878"},
    {"accent", "#0067C0", "#4CC2FF"},
    {"accentHover", "#1975C5", "#47B1E8"},
    {"accentPressed", "#3183CA", "#42A1D2"},
    {"accentDisabled", "#C5C5C5", "#3F3F3F"},
    {"accentText", "#005FB8", "#99EBFF"},
    {"onAccent", "#FFFFFF", "#000000"},
    {"onAccentPressed", "#C7DDF0", "#1A4256"},
    {"onAccentDisabled", "#FFFFFF", "#8A8A8A"},
    {"control", "#FBFBFB", "#373737"},
    {"controlHover", "#F6F6F6", "#3D3D3D"},
    {"controlPressed", "#F0F0F0", "#303030"},
    {"controlDisabled", "#F7F7F7", "#2F2F2F"},
    {"controlBorder", "#E3E3E3", "#434343"},
    {"controlBorderBottom", "#C7C7C7", "#3A3A3A"},
    {"input", "#FBFBFB", "#363636"},
    {"inputHover", "#F6F6F6", "#3B3B3B"},
    {"inputFocus", "#FFFFFF", "#1F1F1F"},
    {"inputBottom", "#8A8A8A", "#9A9A9A"},
    {"inset", "#F9F9F9", "#242424"},
    {"subtle", "#F0F0F0", "#383838"},
    {"popup", "#F9F9F9", "#2C2C2C"},
    {"popupBorder", "#DADADA", "#474747"},
    {"focus", "#1B1B1B", "#FFFFFF"},
    {"zoneDash", "#B8B8B8", "#5E5E5E"},
    {"zoneHover", "#FAFAFA", "#303030"},
    {"zoneDrop", "#EBF3FB", "#22394A"},
    {"track", "#E1E1E1", "#454545"},
    {"scrollHandle", "#C2C2C2", "#5E5E5E"},
    {"scrollHandleHover", "#9E9E9E", "#7E7E7E"},
    {"accentTint", "#E5EFF9", "#203A48"},
    {"success", "#0F7B0F", "#6CCB5F"},
    {"cautionTint", "#FFF4CE", "#433519"},
    {"caution", "#9D5D00", "#FCE100"},
    {"criticalTint", "#FDE7E9", "#442726"},
    {"critical", "#C42B1C", "#FF99A4"},
};

QString tokenValue(const QString &name, bool *found = nullptr)
{
    for (const Token &token : tokens) {
        if (name == QLatin1String(token.name)) {
            if (found) {
                *found = true;
            }
            return QString::fromLatin1(darkScheme ? token.dark : token.light);
        }
    }
    if (found) {
        *found = false;
    }
    return {};
}

QColor tokenColor(const char *name)
{
    return QColor(tokenValue(QLatin1String(name)));
}

const char *roleToken(Theme::Role role)
{
    switch (role) {
    case Theme::Role::Text:
        return "text";
    case Theme::Role::Secondary:
        return "secondary";
    case Theme::Role::Accent:
        return "accentText";
    case Theme::Role::Success:
        return "success";
    case Theme::Role::Caution:
        return "caution";
    case Theme::Role::Critical:
        return "critical";
    }
    Q_UNREACHABLE();
}

QString styleSheet()
{
    QFile file(QStringLiteral(":/theme.qss"));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Thème : feuille de style :/theme.qss introuvable.");
        return {};
    }
    const QString source = QString::fromUtf8(file.readAll());

    static const QRegularExpression tokenPattern(QStringLiteral("@([A-Za-z]+)"));
    QString sheet;
    qsizetype copied = 0;
    auto matches = tokenPattern.globalMatch(source);
    while (matches.hasNext()) {
        const auto match = matches.next();
        sheet += QStringView(source).mid(copied, match.capturedStart() - copied);
        bool found = false;
        const QString value = tokenValue(match.captured(1), &found);
        if (!found) {
            qWarning("Thème : jeton inconnu @%s dans :/theme.qss.", qPrintable(match.captured(1)));
        }
        sheet += value;
        copied = match.capturedEnd();
    }
    sheet += QStringView(source).mid(copied);
    return sheet;
}

QPalette palette()
{
    const QColor window = tokenColor("window");
    const QColor card = tokenColor("card");
    const QColor text = tokenColor("text");
    const QColor disabled = tokenColor("disabledText");
    const QColor border = tokenColor("controlBorder");

    QPalette palette;
    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::WindowText, text);
    palette.setColor(QPalette::Base, card);
    palette.setColor(QPalette::AlternateBase, tokenColor("inset"));
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, tokenColor("control"));
    palette.setColor(QPalette::ButtonText, text);
    palette.setColor(QPalette::BrightText, tokenColor("critical"));
    palette.setColor(QPalette::Highlight, tokenColor("accent"));
    palette.setColor(QPalette::HighlightedText, tokenColor("onAccent"));
    palette.setColor(QPalette::ToolTipBase, tokenColor("popup"));
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::PlaceholderText, tokenColor("tertiary"));
    palette.setColor(QPalette::Link, tokenColor("accentText"));
    palette.setColor(QPalette::LinkVisited, tokenColor("accentText"));
    palette.setColor(QPalette::Light, card);
    palette.setColor(QPalette::Midlight, border);
    palette.setColor(QPalette::Mid, tokenColor("controlBorderBottom"));
    palette.setColor(QPalette::Dark, tokenColor("tertiary"));
    palette.setColor(QPalette::Shadow, darkScheme ? QColor(0, 0, 0) : QColor(0x80, 0x80, 0x80));
    for (const auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText}) {
        palette.setColor(QPalette::Disabled, role, disabled);
    }
    palette.setColor(QPalette::Disabled, QPalette::Highlight, tokenColor("accentDisabled"));
    return palette;
}

void applyFont()
{
    static bool done = false;
    if (done) {
        return;
    }
    done = true;

    QFont font = QApplication::font();
    QStringList families{QStringLiteral("Segoe UI Variable Text"), QStringLiteral("Segoe UI Variable"),
                         QStringLiteral("Segoe UI"), QStringLiteral("Inter")};
    if (!families.contains(font.family())) {
        families.append(font.family());
    }
    font.setFamilies(families);
    font.setPointSizeF(10.5);
    QApplication::setFont(font);
}

QByteArray iconSource(const QString &name)
{
    static QHash<QString, QByteArray> cache;
    auto found = cache.constFind(name);
    if (found == cache.constEnd()) {
        QFile file(QStringLiteral(":/icons/%1.svg").arg(name));
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("Thème : icône %s introuvable.", qPrintable(name));
        }
        found = cache.insert(name, file.readAll());
    }
    return *found;
}

class TintedSvgEngine final : public QIconEngine
{
public:
    TintedSvgEngine(const QString &iconName, Theme::Role iconRole)
        : name(iconName)
        , role(iconRole)
    {
    }

    QIconEngine *clone() const override
    {
        return new TintedSvgEngine(*this);
    }

    QString key() const override
    {
        return QStringLiteral("TintedSvgEngine");
    }

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State) override
    {
        const QColor color = mode == QIcon::Disabled ? tokenColor("disabledText") : Theme::color(role);
        QByteArray svg = iconSource(name);
        svg.replace("currentColor", color.name().toLatin1());
        QSvgRenderer renderer(svg);
        renderer.render(painter, QRectF(rect));
    }

    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override
    {
        return scaledPixmap(size, mode, state, 1.0);
    }

    QPixmap scaledPixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal scale) override
    {
        QPixmap pixmap(size * scale);
        pixmap.fill(Qt::transparent);
        {
            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            paint(&painter, QRect(QPoint(), pixmap.size()), mode, state);
        }
        pixmap.setDevicePixelRatio(scale);
        return pixmap;
    }

private:
    QString name;
    Theme::Role role;
};

}

namespace Theme {

void apply(bool dark)
{
    darkScheme = dark;
    applyFont();
    QApplication::setPalette(palette());
    qApp->setStyleSheet(styleSheet());
}

bool isDark()
{
    return darkScheme;
}

QColor color(Role role)
{
    return tokenColor(roleToken(role));
}

QIcon icon(const QString &name, Role role)
{
    return QIcon(new TintedSvgEngine(name, role));
}

}
