#include "wakatime_options_page.h"
#include "wakatime_constants.h"

namespace QtCreatorWakatime {
namespace Internal {

WakatimeOptionsPage::WakatimeOptionsPage(QObject *parent) : Core::IOptionsPage(parent)
{
    setId("A.General");
    setDisplayName(tr("Wakatime"));
    setCategory(Core::Id::fromString(QString(Constants::SETTINGS_TR_CATEGORY)));
    setDisplayCategory("Wakatime");
    //setCategoryIcon(Utils::Icon(":/doxygen.png"));
}

QWidget *WakatimeOptionsPage::createPage(QWidget *parent)
{
   return nullptr;
}

void WakatimeOptionsPage::apply()
{
   // Implement the apply botton functionality
}

void WakatimeOptionsPage::finish()
{
   // Implement the ok button functionality
}
} // namespace Internal
} // namespace QtCreatorWakatime
