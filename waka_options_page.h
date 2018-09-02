#ifndef WAKA_OPTIONS_PAGE_H
#define WAKA_OPTIONS_PAGE_H

#include "waka_options_widget.h"

#include <coreplugin/dialogs/ioptionspage.h>

#include <QPointer>
#include <QWidget>

namespace Wakatime {
namespace Internal {

class WakaOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT

public:
    explicit WakaOptionsPage(const QSharedPointer<WakaOptions> &options, QObject *parent = nullptr);
    virtual ~WakaOptionsPage() override = default;

private:
    QWidget *widget() override;
    void apply() override;
    void finish() override;

private:
    QPointer<WakaOptionsWidget> _widget;
    QSharedPointer<WakaOptions> _options;
};

} // namespace Internal
} // namespace QtCreatorWakatime

#endif // WAKA_OPTIONS_PAGE_H
