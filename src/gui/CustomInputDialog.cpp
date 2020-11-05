#include "CustomInputDialog.h"
#include <QApplication>

CustomInputDialog::CustomInputDialog(QWidget* parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    mTokenLabel(new QLabel(this)),
    mTokenLineEdit(new QLineEdit(this)),
    mOkButton(new QPushButton(this)),
    mCancelButton(new QPushButton(this)),
    mLayout(new QVBoxLayout(this)),
    mResult(QDialog::Rejected)
{
    connect(mOkButton, &QPushButton::clicked, this, &CustomInputDialog::OnOk);
    connect(mCancelButton, &QPushButton::clicked, this, &CustomInputDialog::OnCancel);

    mTokenLineEdit->setPlaceholderText(tr("Token"));
    connect(mTokenLineEdit, &QLineEdit::textChanged, this, &CustomInputDialog::OnTextChanged);

    QString link =( "<a href=\"https://ushare.phonexa.com/\">link</a> ");
    QString ll =  QString("To login please, use the following %1, copy token and paste in field below").arg(link);
    mTokenLabel->setText(ll);
    mTokenLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    mTokenLabel->setOpenExternalLinks(true);

    mOkButton->setText("Ok");
    mOkButton->setAutoDefault(false);
    mCancelButton->setText("Cancel");
    mCancelButton->setAutoDefault(true);
    mCancelButton->setDefault(true);

    auto buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(mOkButton);
    buttonLayout->addWidget(mCancelButton);
    mLayout->addWidget(mTokenLabel);
    mLayout->addWidget(mTokenLineEdit);
    mLayout->addLayout(buttonLayout);
    setLayout(mLayout);
    setTabOrder(mTokenLineEdit, mOkButton);
    setTabOrder(mOkButton, mCancelButton);

    setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("Enter token"));

}

CustomInputDialog::~CustomInputDialog()
{

}

void CustomInputDialog::OnOk()
{
    setResult(QDialog::Accepted);
    mResult = QDialog::Accepted;
    close();
}

void CustomInputDialog::OnCancel()
{
    setResult(QDialog::Rejected);
    close();
}

void CustomInputDialog::OnTextChanged(const QString &text)
{
    mText = text;
}

