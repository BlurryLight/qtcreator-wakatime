#ifndef WAKATIME_OPTIONS_PAGE_H
#define WAKATIME_OPTIONS_PAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

namespace QtCreatorWakatime {
namespace Internal {

class WakatimeOptionsPage : public Core::IOptionsPage
{
public:
    WakatimeOptionsPage(QObject *parent = 0);

private:
    QWidget *createPage(QWidget *parent);
    void apply(void);
    void finish();
};

} // namespace Internal
} // namespace QtCreatorWakatime

#endif // WAKATIME_OPTIONS_PAGE_H
