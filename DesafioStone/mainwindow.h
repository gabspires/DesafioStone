#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOAuth2AuthorizationCodeFlow>

#include <QTableWidgetItem>
#include <QMediaPlaylist>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void authorizationStatusChanged (QAbstractOAuth::Status status);

private slots:
    void on_conexaoButton_clicked();

    void on_buscaButton_clicked();

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_2_itemDoubleClicked();

    void on_salveButton_clicked();

    void on_carregarButton_clicked();

private:
    Ui::MainWindow *ui;

    QOAuth2AuthorizationCodeFlow spotify;
    bool isGranted;
    QString userName;
    QString client_id = "0ab3ec194754471c8ee06d453d0bdf80";
    QString client_Key = "195a6dea50c84d199107fb5efdf3d783";
    QMediaPlayer *musicPlayer;
    QMediaPlaylist *playlist;
};
#endif // MAINWINDOW_H
