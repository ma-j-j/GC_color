#include "OfflineTest.h"
#include <QListView>


OffLineTest::OffLineTest(Ui::MainWindowView *ui1,QObject *parent)
    : QObject{parent}, ui(ui1)
{

    ui->comboBox_IC->addItem(u8"11.5x13");
    ui->comboBox_IC->addItem(u8"11x13");
    ui->comboBox_IC->setCurrentIndex(0);
    QAbstractItemView *abstractViewIC = ui->comboBox_IC->view();
    QListView *listViewIC = qobject_cast<QListView *>(abstractViewIC);
    listViewIC->setSpacing(6);

    // 连接信号与槽
    connect(ui->pushButton_OpenImg_2, &QPushButton::clicked, this, &OffLineTest::slotOffLineTestingView_On_PushButton_OpenImg_Clicked);//打开图片
    connect(ui->pushButton_StartTest,&QPushButton::clicked,this,&OffLineTest::slotOffLineTestingView_On_PushButton_StartTest_Clicked);//开始测试
    changeStyle();
}

void OffLineTest::setImg(QImage qImage)
{
    if(true == qImage.isNull())
    {
        qDebug()<<"OffLineTestingView::setImg 传入图像为NULL";
        return;
    }
    ui->graphicsView_Test->loadQPixmapImage(QPixmap::fromImage(qImage));
    this->ui->graphicsView->setVisible(false);
    this->ui->graphicsViewAnno->setVisible(false);
    this->ui->graphicsView_Test->setVisible(true);
    ui->label_ShowImage->setText("离线测试图片");
}


//选择图片
void OffLineTest::slotOffLineTestingView_On_PushButton_OpenImg_Clicked()
{
    QString file_path = QFileDialog::getOpenFileName(nullptr,tr("Open Image"),"15000_IMG",tr("Image Files(*.jpg *.bmp *.png)"));
    if(true == file_path.isEmpty())
    {
        // 取消了对话框
        qDebug()<<u8"取消选择图片";
        return ;
    }
    this->ui->graphicsView->setVisible(true);//显示原图
    this->ui->graphicsViewAnno->setVisible(false);
    this->ui->graphicsView_Test->setVisible(true);
    ui->label_ShowImage->setText("离线测试图片");

    // 将图像路径显示到界面的lineEdit
    ui->lineEdit_filePath_2->setText(file_path);

}

//开始测试
void OffLineTest::slotOffLineTestingView_On_PushButton_StartTest_Clicked()
{
    // 将界面设置的信息汇总发送给后端进行检测
    // 获取图像路径
    QString imgFile = ui->lineEdit_filePath_2->text();
    if(true == imgFile.isEmpty())
    {
        // 图像路径为空
        QMessageBox::warning(NULL,u8"Error",u8"图像路径为空");
        return;
    }

    // 获取芯片类型
    IC_Type type = IC_Type::A;
    QString strType = ui->comboBox_IC->currentText();
    if("11.5x13" == strType)
    {
        type = IC_Type::A;
    }
    else if("11x13" == strType)
    {
        type = IC_Type::B;
    }

    // 将信息发送到后端进行检测
    emit this->signalOffLineTestingView_Send_Detection(imgFile,type);

    //检测测试
    rect_detect img_detect;
    cv::Mat result;
    // img_detect.calibration();
    img_detect.ROI(imgFile,150,result);

}
