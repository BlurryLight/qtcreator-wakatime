#include "waka_options_widget.h"
#include "ui_waka_options.h"
#include "waka_options.h"

namespace QtCreatorWakatime {
namespace Internal {

WakaOptionsWidget::WakaOptionsWidget(const QSharedPointer<WakaOptions> &options, QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::WakaOptionsForm),
    _options(options)
{
    _ui->setupUi(this);

    connect(_ui->shToolButton, &QToolButton::pressed, this, [this]()
    {
        _ui->apiKeyLineEdit->setEchoMode(_ui->apiKeyLineEdit->echoMode() == QLineEdit::EchoMode::Password ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password);
        _ui->shToolButton->setText(_ui->apiKeyLineEdit->echoMode() == QLineEdit::EchoMode::Password ? "show" : "hide");
    });
}

WakaOptionsWidget::~WakaOptionsWidget()
{
    delete _ui;
}

void WakaOptionsWidget::restore()
{
   _ui->apiKeyLineEdit->setText(_options->apiKey());
   _ui->excludePaternLineEdit->setText(_options->ignorePatern());
   _ui->inStatusBarCheckBox->setChecked(_options->inStatusBar());
   _ui->debugCheckBox->setChecked(_options->isDebug());
   _ui->enabledCheckBox->setChecked(_options->isEnabled());
}

void WakaOptionsWidget::apply()
{
    _options->setApiKey(_ui->apiKeyLineEdit->text());
    _options->setIgnorePatern(_ui->excludePaternLineEdit->text());
    _options->setStatusBar(_ui->inStatusBarCheckBox->isChecked());
    _options->setDebug(_ui->debugCheckBox->isChecked());
    _options->setEnabled(_ui->enabledCheckBox->isChecked());
    _options->save();
}

} // namespace Internal
} // namespace QtCreatorWakatime
