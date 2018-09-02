#include "waka_options_page.h"
#include "waka_constants.h"
#include "waka_options.h"

#include <QCoreApplication>

namespace Wakatime {
namespace Internal {

WakaOptionsPage::WakaOptionsPage(const QSharedPointer<WakaOptions> &options, QObject *parent) : Core::IOptionsPage(parent), _options(options)
{
    setId(Constants::OPTION_ID);
    setDisplayName(tr("General"));
    setCategory(Constants::OPTION_CATEGORY);
    setDisplayCategory(QCoreApplication::translate("Wakatime", "Wakatime"));
    setCategoryIcon(Utils::Icon({{":/logo.png", Utils::Theme::PanelTextColorDark}}, Utils::Icon::Tint));
}

QWidget *WakaOptionsPage::widget()
{
    _options->read();

   if(!_widget)
   {
       _widget = new WakaOptionsWidget(_options);
   }
   _widget->restore();

   return _widget;
}

void WakaOptionsPage::apply()
{
   if(_widget)
   {
       _widget->apply();
   }
}

void WakaOptionsPage::finish()
{
    // Implement the ok button functionality
}

} // namespace Internal
} // namespace QtCreatorWakatime
