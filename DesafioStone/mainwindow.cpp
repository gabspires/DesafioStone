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

     ui->tableWidget->setRowCount(0);
      QUrl u ("https://api.spotify.com/v1/search?q="+ui->buscaLine->text() +"&type=track");
      auto reply = spotify.get(u);

      connect (reply, &QNetworkReply::finished, [=]() {
          if (reply->error() != QNetworkReply::NoError) {
              qWarning () << reply->errorString();
              return;
          }

          const auto data = reply->readAll();

          const auto document = QJsonDocument::fromJson(data);
          const auto root = document.object();
          QString rootValue = root.value("id").toString();

          QVariantMap root_map = root.toVariantMap();
          QVariantMap tracks_map = root_map["tracks"].toMap();
          QVariantList track_list = tracks_map["items"].toList();

          for(int i =0; i< track_list.count();i++)
          {
              QVariantMap itemMap = track_list[i].toMap();
              auto ID  = itemMap["id"];
              auto name = itemMap["name"];
              auto href = itemMap["href"];
              auto previewUrl = itemMap["preview_url"];

              QTableWidgetItem *nameItem = new QTableWidgetItem(name.toString());
              nameItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
              ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
              ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1, 0, nameItem);

              QTableWidgetItem *previewItem = new QTableWidgetItem(previewUrl.toString());
              previewItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
              ui->tableWidget->setItem   ( ui->tableWidget->rowCount()-1,
                                       1,
                                       previewItem);


          }


          reply->deleteLater();
      });
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_conexaoButton_clicked()
{
    spotify.grant();
}


