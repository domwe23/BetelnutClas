#include "CApplication.h"
#include "MainWindow.h"
#include "VpMessageBox.h"
#include "VpDialog.h"
//#include "FrmMachineInfo.h"


int main(int argc, char *argv[]) {
    CApplication a(argc, argv);
      if (!a.InitState()) {
          return -1;
     }
    //VpDialog::ShowDialog<FrmMachineInfo>();


    MainWindow w;
    w.showMaximized();

    return a.exec();
}

