#ifndef CUSTOMINPUTDIALOG_H
#define CUSTOMINPUTDIALOG_H


#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDesktopServices>
#include <QDialog>

class CustomInputDialog : public QDialog
{
    Q_OBJECT
public:
    CustomInputDialog(QWidget* parent = nullptr);
    ~CustomInputDialog();

    QString getText() { return mText; }
    int getResult() { return mResult; }

private:
    QLabel      *mTokenLabel;
    QLineEdit   *mTokenLineEdit;
    QPushButton *mOkButton;
    QPushButton *mCancelButton;
    QVBoxLayout *mLayout;

    QString mText;
    int     mResult;

private slots:
    void OnOk();
    void OnCancel();
    void OnTextChanged(const QString& text);
};

#endif // CUSTOMINPUTDIALOG_H
