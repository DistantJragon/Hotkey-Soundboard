#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QMap<QString, QVariant> initialSettings,
                             QWidget* parent)
    : QDialog(parent), optionsMap(initialSettings), ui(new Ui::OptionsDialog) {
  ui->setupUi(this);

  initializeMapAndUI();
}

OptionsDialog::~OptionsDialog() { delete ui; }

void OptionsDialog::initializeMapAndUI() {
  // Update Check Settings Map
  // checkUpdateStartup
  if (!optionsMap.contains("checkUpdate/atStartup")) {
    optionsMap["checkUpdate/atStartup"] = false;
  }
  ui->updateStartupCheckBox->setChecked(
      optionsMap["checkUpdate/atStartup"].toBool());

  // checkUpdateInterval
  if (!optionsMap.contains("checkUpdate/interval")) {
    optionsMap["checkUpdate/interval"] = false;
  }
  ui->updateIntervalCheckBox->setChecked(
      optionsMap["checkUpdate/interval"].toBool());
  ui->updateIntervalHoursLabel->setEnabled(
      optionsMap["checkUpdate/interval"].toBool());
  ui->updateIntervalSlider->setEnabled(
      optionsMap["checkUpdate/interval"].toBool());

  // checkUpdateIntervalHours
  if (!optionsMap.contains("checkUpdate/intervalHours")) {
    optionsMap["checkUpdate/intervalHours"] = 6;
  }
  int intervalHours = optionsMap["checkUpdate/intervalHours"].toInt();
  ui->updateIntervalSlider->setValue(intervalHours);
  // Set text to "Every X Hours"
  QString labelText = "Every " + QString::number(intervalHours) + " hours";
  ui->updateIntervalHoursLabel->setText(labelText);

  // notifyErrors
  if (!optionsMap.contains("checkUpdate/notifyErrors")) {
    optionsMap["checkUpdate/notifyErrors"] = true;
  }
  ui->notifyErrorCheckBox->setChecked(
      optionsMap["checkUpdate/notifyErrors"].toBool());

  // Page buttons
  connect(ui->updatePageButton, &QPushButton::clicked, this, [this]() {
    unFlattenPageButtons();
    ui->updatePageButton->setFlat(true);
    ui->stackedWidget->setCurrentWidget(ui->updatePage);
  });

  // Update Page
  connect(
      ui->updateStartupCheckBox, &QCheckBox::toggled, this,
      [this](bool checked) { optionsMap["checkUpdate/atStartup"] = checked; });

  connect(ui->updateIntervalCheckBox, &QCheckBox::toggled, this,
          [this](bool checked) {
            optionsMap["checkUpdate/interval"] = checked;
            ui->updateIntervalHoursLabel->setEnabled(checked);
            ui->updateIntervalSlider->setEnabled(checked);
          });

  connect(ui->updateIntervalSlider, &QSlider::valueChanged, this,
          [this](int value) {
            optionsMap["checkUpdate/intervalHours"] = value;
            QString labelText = "Every " + QString::number(value) + " hours";
            ui->updateIntervalHoursLabel->setText(labelText);
          });

  connect(ui->notifyErrorCheckBox, &QCheckBox::toggled, this,
          [this](bool checked) {
            optionsMap["checkUpdate/notifyErrors"] = checked;
          });

  // Button Box
  QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(applyButton, &QPushButton::clicked, this,
          [this]() { emit applyRequested(optionsMap); });
}

void OptionsDialog::unFlattenPageButtons() {
  ui->updatePageButton->setFlat(false);
}
