#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtNetworkAuth>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setSelectionBehavior(QTableView::SelectRows);

    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, NULL);

    musicPlayer = new QMediaPlayer();
    playlist = new QMediaPlaylist();

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
            ui->salveButton->setEnabled(true);
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





void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{

    auto itemRow = item->row();


    int currentIndex = 0;
    ui->tableWidget_2->insertRow ( ui->tableWidget_2->rowCount() );
    for(int i =0; i< ui->tableWidget->columnCount();++i)
    {
        auto itemSelected = ui->tableWidget->item(itemRow,i);
        ui->tableWidget_2->setItem   ( ui->tableWidget_2->rowCount()-1,
                                   currentIndex,
                                  new QTableWidgetItem(itemSelected->text()));
       currentIndex++;
       if(currentIndex>1)
           currentIndex=0;

    }

}


void MainWindow::on_tableWidget_2_itemDoubleClicked()
{
    if(ui->tableWidget_2->rowCount()==0)
    {
      QMessageBox::information(this, tr("A playlist está vazia!"),
      "Insira músicas na playlist para começar");
      return;
    }

    auto item = ui->tableWidget_2->selectedItems();

    //Remove por linha; Divide por dois pois o método selectedItems pega por célula. Caso tente remover um que não está lá, quebra.
    for(int i =0;i<= item.count()/2;i+=2)
    {
        ui->tableWidget_2->removeRow(item[i]->row());
    }
}

void MainWindow::on_salveButton_clicked()
{
    if(ui->tableWidget_2->rowCount()==0)
    {
        QMessageBox::information(this, tr("Dados não encontrados"),
            "Adicionar músicas a playlist");
        return;
    }
    else{
    QString fileName = QFileDialog::getSaveFileName(this,
         tr("Salvar Playlist"), "",
         tr("Arquivo de Texto (*.txt);;Todos os Arquivos (*)"));

    if (fileName.isEmpty())
          return;
      else {
          QFile file(fileName);
          if (!file.open(QIODevice::WriteOnly)) {
              QMessageBox::information(this, tr("Não é possível abrir o arquivo"),
                  file.errorString());
              return;
          }

          try{

              QTextStream out(&file);
              for(int i =0; i< ui->tableWidget_2->rowCount();++i)
              {
                  auto itemURL = ui->tableWidget_2->item(i,1);
                  auto itemName = ui->tableWidget_2->item(i,0);
                  out << QString(itemName->text() + ";" + itemURL->text()) << "\r\n"; //windows line terminator
              }
          }
          catch(QException e)
          {
              QMessageBox::information(this, tr("Erro ao escrever o arquivo"),
                 " Erro encontrado ao escrever o arquivo");
          }
      }
    }
}
