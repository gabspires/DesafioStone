#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QMessageBox>

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

    QString s;
        if (status == QAbstractOAuth::Status::Granted){
            s = "Conectado com Spotify";
            ui->conexaoButton->setEnabled(false);
            ui->buscaButton->setEnabled(true);
        }
        if (status == QAbstractOAuth::Status::NotAuthenticated){
            s = "Conexão Perdida";
            ui->conexaoButton->setEnabled(true);
        }


        if (status == QAbstractOAuth::Status::TemporaryCredentialsReceived) {
            s = "Conectado temporariamente";

        }

        ui->label->setText("Status da Conexão: " + s +  "\n");

}


void MainWindow::on_buscaButton_clicked()
{

    if(ui->buscaLine->text()== NULL || ui->buscaLine->text()=="")
        {
            QMessageBox::information(this, tr("Nenhuma Informação"),
            "Digite o que deseja buscar");
            return;
        }


}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_conexaoButton_clicked()
{
    spotify.grant();
}


