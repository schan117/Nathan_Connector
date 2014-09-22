#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   Q_UNUSED(context);

   QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
   QString txt = QString("[%1] ").arg(dt);

   switch (type)
   {
      case QtDebugMsg:
         txt += QString("{Debug} %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("{Warning} %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("{Critical} %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("{Fatal} %1").arg(msg);
         abort();
         break;
   }

   QFile outFile(QString("Log.txt"));
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);

   QTextStream textStream(&outFile);
   textStream << txt << endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
