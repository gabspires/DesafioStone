#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, NULL);

    spotify.setReplyHandler(replyHandler);
    spotify.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    spotify.setClientIdentifier(client_id);
    spotify.setClientIdentifierSharedKey(client_Key);

    connect(&spotify, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
                 &QDesktopServices::openUrl);

        connect(&spotify, &QOAuth2AuthorizationCodeFlow::statusChanged,
                this, &MainWindow::authorizationStatusChanged);
}

void MainWindow::authorizationStatusChanged(QAbstractOAuth::Status status)
{

    ui->label->setText("Conectado com Spotify");

}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_conexaoButton_clicked()
{
    spotify.grant();
}
