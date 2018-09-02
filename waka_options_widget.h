#ifndef WAKA_OPTIONS_WIDGET_H
#define WAKA_OPTIONS_WIDGET_H

#include <coreplugin/dialogs/ioptionspage.h>

#include <QPointer>
#include <QWidget>
#include <QPointer>

namespace Ui {
class WakaOptionsForm;
}

namespace Wakatime {
namespace Internal {

class WakaOptions;
class WakaOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WakaOptionsWidget(const QSharedPointer<WakaOptions> &options, QWidget *parent = nullptr);
    ~WakaOptionsWidget();

    void restore();
    void apply();

private:
    Ui::WakaOptionsForm *_ui;
    QSharedPointer<WakaOptions> _options;
};

} // namespace Internal
} // namespace QtCreatorWakatime

#endif // WAKA_OPTIONS_WIDGET_H
