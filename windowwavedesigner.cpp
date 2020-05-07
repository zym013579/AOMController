#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

#include <QFileDialog>

#include "dialogparameter.h"
#include "command.h"

//以下为绘图时所用参数，大多常数为计算并尝试后获得
#define X_VAL (ui->widgetWave->xAxis->pixelToCoord(event->pos().x()))
#define Y_VAL (ui->widgetWave->yAxis->pixelToCoord(event->pos().y()))

#define X_RANGE ((ui->widgetWave->width()-34-15)/30.0*5/ui->widgetWave->xAxis->range().size()*11/POINT_CIRCLE_SIZE)
#define Y_RANGE ((ui->widgetWave->height()-14-24)/6.2/ui->widgetWave->yAxis->range().size()*11/POINT_CIRCLE_SIZE)

//节点圆大小
#define POINT_CIRCLE_SIZE 11

//新增点时，与上一点的间距
#define DEFAULT_X_DIS_P 5 //仅有一个点时的间距
#define DEFAULT_X_DIS ((ui->widgetWave->graph()->dataCount()-1)?(ui->widgetWave->graph()->data()->at(ui->widgetWave->graph()->dataCount()-1)->key/(ui->widgetWave->graph()->dataCount()-1)):DEFAULT_X_DIS_P)

//至此为止

//写入/读取 文本框/复选框
#define SETCHECKBOX_REALTIME_QUANTIFY(enabled) (ui->checkBoxRealTimeQuanti->setChecked(enabled))
#define SETTEXT_VOL_QUANTIFY_LEVEL(level) (ui->lineEditVolQuantiLevel->setText(numberToStr(level)))
#define SETTEXT_UNIT_TIME(time) (ui->lineEditMinDeltaTime->setText(numberToStr(time)))
#define SETTEXT_MAX_VOL(vol) (ui->lineEditMaxVol->setText(numberToStr(vol)))

#define CHECKBOX_REALTIME_QUANTIFY (ui->checkBoxRealTimeQuanti->isChecked())
#define TEXT_VOL_QUANTIFY_LEVEL (ui->lineEditVolQuantiLevel->text().toInt())
#define TEXT_UNIT_TIME (ui->lineEditMinDeltaTime->text().toDouble())
#define TEXT_MAX_VOL (ui->lineEditMaxVol->text().toDouble())

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner),
    edit(new WaveData),
    mouseHasMoved(false),
    saved(true),
    c_point(-1)    //选中已选择的点时改变值

{
    ui->setupUi(this);

    //设置可输入字符
    QRegExp regExpUDouble("(\\d*)(\\.\\d+)?");
    QRegExp regExpUInt("(\\d*)");
    ui->lineEditMinDeltaTime->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditMinDeltaTime));
    ui->lineEditPointTime->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditPointTime));
    ui->lineEditVolQuantiLevel->setValidator(new QRegExpValidator(regExpUInt, ui->lineEditVolQuantiLevel));
    ui->lineEditPointNumber->setValidator(new QRegExpValidator(regExpUInt, ui->lineEditPointNumber));
    ui->lineEditPointVoltage->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditPointVoltage));
    ui->lineEditMaxVol->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditMaxVol));

    emit initWaveGraph(ui->widgetWave);
    edit->setSaveMode(EDITOR_MODE);

    //以下为编辑器界面坐标图的参数设置
    ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, POINT_CIRCLE_SIZE));
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);// QCP::iSelectPlottables  QCP::iRangeZoom
    ui->widgetWave->graph()->setSelectable(QCP::SelectionType::stDataRange);   //仅可选择一个点QCP::SelectionType::stSingleData
    ui->widgetWave->axisRect()->setRangeDrag(Qt::Horizontal);   //设置仅水平拖动和缩放
    ui->widgetWave->axisRect()->setRangeZoom(Qt::Horizontal);

    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undoStep()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redoStep()));
    connect(parent, SIGNAL(sendSettings(WaveData*)), this, SLOT(recieveSettings(WaveData*)));
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    delete edit;
    delete ui;
}

double checkData(double input, double min, double max)
{
    return MAX(min, MIN(max, input));
}

void WindowWaveDesigner::recieveSettings(WaveData *data)
{
    edit->clear();
    edit->copyData(data);
    edit->quantify();
    SETCHECKBOX_REALTIME_QUANTIFY(edit->getRealTimeQuantify());
    SETTEXT_UNIT_TIME(edit->getUnitTime());
    SETTEXT_VOL_QUANTIFY_LEVEL(edit->getVolQuantiLevel());
    SETTEXT_MAX_VOL(edit->getMaxVol());
    emit freshUndoRedoButton();
    //设置坐标范围
    if (edit->count() > 1)
        ui->widgetWave->xAxis->setRange(-0.05*edit->xAt(edit->count()-1), 1.05*edit->xAt(edit->count()-1));
    else
        ui->widgetWave->xAxis->setRange(-0.25, 20.2);
    emit choosePoint(-1);
}

void WindowWaveDesigner::updateGraph()
{
    emit updateWaveGraph(ui->widgetWave, edit->x(), edit->y(), edit->getMaxVol());
    SETTEXT_UNIT_TIME(edit->getUnitTime());
    SETTEXT_VOL_QUANTIFY_LEVEL(edit->getVolQuantiLevel());
    SETCHECKBOX_REALTIME_QUANTIFY(edit->getRealTimeQuantify());
    SETTEXT_MAX_VOL(edit->getMaxVol());
}

void WindowWaveDesigner::updateLineEditText()
{
    int i = witchPointclicked();
    if (i == -1)
    {
        ui->lineEditPointNumber->setText("");
        ui->lineEditPointTime->setText("");
        ui->lineEditPointVoltage->setText("");
    }
    else
    {
        ui->lineEditPointNumber->setText(numberToStr(i));
        ui->lineEditPointTime->setText(numberToStr(edit->xAt(i)));
        ui->lineEditPointVoltage->setText(numberToStr(edit->getMaxVol()*edit->yAt(i)));
    }
}

void WindowWaveDesigner::saveStep()
{
    edit->save();
    saved = false;
    emit freshUndoRedoButton();
}

void WindowWaveDesigner::dropStep()
{
    edit->drop();
    emit updateGraph();
    emit updateLineEditText();
}

void WindowWaveDesigner::undoStep()
{
    if (edit->countUnDo() < 1)
        return;
    edit->unDo();
    saved = false;
    emit freshUndoRedoButton();
    emit choosePoint(-1);
}

void WindowWaveDesigner::redoStep()
{
    if (edit->countReDo() < 1)
        return;
    edit->reDo();
    saved = false;
    emit freshUndoRedoButton();
    emit choosePoint(-1);
}

void WindowWaveDesigner::freshUndoRedoButton()
{
    if (edit->countUnDo() > 0)
        ui->actionUndo->setEnabled(true);
    else
        ui->actionUndo->setEnabled(false);

    if (edit->countReDo() > 0)
        ui->actionRedo->setEnabled(true);
    else
        ui->actionRedo->setEnabled(false);
}

int WindowWaveDesigner::witchPointclicking(QMouseEvent *event)
{
    for (int i=0; i<edit->count(); i++)
    {
        if ((X_VAL-edit->xAt(i))*X_RANGE > 1)
            continue;
        else if ((X_VAL-edit->xAt(i))*X_RANGE*(X_VAL-edit->xAt(i))*X_RANGE + (Y_VAL-edit->yAt(i))*Y_RANGE*(Y_VAL-edit->yAt(i))*Y_RANGE <= 1)
            return i;
        else if ((X_VAL-edit->xAt(i))*X_RANGE < -1)
            break;
    }
    return -1;
}

int WindowWaveDesigner::witchPointclicked()
{
    if (ui->widgetWave->graph()->selection() == QCPDataSelection())
        return -1;
    return ui->widgetWave->graph()->selection().dataRange().begin();
}

void WindowWaveDesigner::choosePoint(int i, int j)
{
    if (i <= -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection());
    else if (j == -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, i+1)));
    else
        //目前暂未用到此项
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, j+1)));
    emit updateGraph();
    emit updateLineEditText();
}

void WindowWaveDesigner::on_widgetWave_mousePress(QMouseEvent *event)
{
    mouseHasMoved = false;
    c_point = -1;
    if (witchPointclicking(event) == witchPointclicked())
        c_point = witchPointclicked();
}

void WindowWaveDesigner::on_widgetWave_mouseRelease(QMouseEvent *event)
{
    if (!mouseHasMoved)
    {
        emit choosePoint(witchPointclicking(event));
    }
    else if (c_point != -1)
        emit saveStep();
    c_point = -1;
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
}

void WindowWaveDesigner::on_widgetWave_mouseMove(QMouseEvent *event)
{
    mouseHasMoved = true;
    if (c_point == -1)
        return;
    ui->widgetWave->setInteractions(QCP::iRangeZoom);

    double x_val;
    if (c_point == 0)
        x_val = 0;
    else if (c_point != edit->count()-1)
        x_val = checkData(X_VAL, edit->xAt(c_point-1)+edit->getUnitTime(), edit->xAt(c_point+1)-edit->getUnitTime());
    else
        x_val = MAX(edit->xAt(c_point-1)+edit->getUnitTime(), X_VAL);
    edit->set(c_point, x_val, checkData(Y_VAL, 0, 1));
    emit updateGraph();
    emit updateLineEditText();
}

void WindowWaveDesigner::closeEvent(QCloseEvent *event)
{
    if (saved)
        return;
    QMessageBox::StandardButton result = QMessageBox::information(this, "提示", "波形尚未应用，是否应用波形？", QMessageBox::Yes|QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Cancel);
    //event->accept();
    //event->ignore();
    if (result == QMessageBox::Yes)
        emit on_pushButtonSave_clicked();
    else if (result == QMessageBox::Save)
    {
        emit on_actionSaveAsFile_triggered();
        event->ignore();
    }
    //else if (result == QMessageBox::Discard) ;
    else if (result == QMessageBox::Cancel)
        event->ignore();
}

void WindowWaveDesigner::on_pushButtonNew_clicked()
{
    edit->add(edit->xAt(edit->count()-1)+DEFAULT_X_DIS, edit->yAt(edit->count()-1));
    emit saveStep();
    emit updateGraph();
}

void WindowWaveDesigner::on_pushButtonInsert_clicked()
{
    int i = witchPointclicked();
    if (i == -1 || i == edit->count()-1)
        emit on_pushButtonNew_clicked();
    else if (edit->xAt(i+1)-edit->xAt(i) >= 2.0*edit->getUnitTime())
        edit->insert(i+1, (edit->xAt(i+1)+edit->xAt(i))/2.0, edit->yAt(i));
    else
        MSG_WARNING("两点间距过小，不能插入");
    emit saveStep();
    emit updateGraph();
}

void WindowWaveDesigner::on_pushButtonSave_clicked()
{
    emit sendWaveData(edit);
    saved = true;
}

void WindowWaveDesigner::on_pushButtonDelete_clicked()
{
    if (witchPointclicked() == 0)
        MSG_WARNING("暂不支持删除起始点"); //不能删除初始点，或者可改写为删除后重排列
    else if (witchPointclicked() != -1)
    {
        edit->del(witchPointclicked());
        emit saveStep();
        c_point = -1;
        emit choosePoint(-1);
    }
    emit updateGraph();
}

void WindowWaveDesigner::on_lineEditPointNumber_textEdited(const QString &arg1)
{
    if (arg1 != "")
    {
        if (arg1.toInt() < edit->count() && arg1.toInt() >= 0)
            emit choosePoint(arg1.toInt());
        else
            emit choosePoint(-1);
    }
    else
        emit choosePoint(-1);
}

void WindowWaveDesigner::on_lineEditPointTime_textEdited(const QString &arg1)
{
    int i = witchPointclicked();
    if (i == -1 || arg1 == "" || i == 0)
        return;
    if (i == edit->count()-1)
        edit->setX(i, MAX(edit->xAt(i-1), arg1.toDouble()));
    else
        edit->setX(i, checkData(arg1.toDouble(), edit->xAt(i-1)+edit->getUnitTime(), edit->xAt(i+1)-edit->getUnitTime()));
    emit updateGraph();
}

void WindowWaveDesigner::on_lineEditPointVoltage_textEdited(const QString &arg1)
{
    int i = witchPointclicked();
    if (i == -1 || arg1 == "")
        return;
    edit->setY(i, checkData(arg1.toDouble()/edit->getMaxVol(), 0, 1));
    emit updateGraph();
}

void WindowWaveDesigner::on_lineEditPointNumber_editingFinished()
{
    if (ui->lineEditPointNumber->text().toInt() >= edit->count())
    {
        emit choosePoint(-1);
        MSG_WARNING("点序号超出范围");
        return;
    }
    emit saveStep();
}

void WindowWaveDesigner::on_lineEditPointTime_editingFinished()
{
    if (ui->lineEditPointTime->text() != "" && ui->lineEditPointNumber->text() == "")
    {
        emit dropStep();
        MSG_WARNING("没有选点");
        return;
    }
    int i = ui->lineEditPointNumber->text().toInt();
    if (i == 0)
    {
        if (ui->lineEditPointTime->text().toDouble() != 0)
        {
            emit dropStep();
            MSG_WARNING("暂不能更改起始点的时间");
        }
        return; //由于起始点时间总是为0，不需要保存，直接返回
    }
    if (ui->lineEditPointTime->text().toDouble() < edit->xAt(i-1)+edit->getUnitTime())
    {
        emit dropStep();
        MSG_WARNING("点时间过小");
        return;
    }
    if (i < edit->count()-1 && ui->lineEditPointTime->text().toDouble() > edit->xAt(i+1)-edit->getUnitTime())
    {
        emit dropStep();
        MSG_WARNING("点时间过大");
        return;
    }
    emit updateLineEditText();
    emit saveStep();
}

void WindowWaveDesigner::on_lineEditPointVoltage_editingFinished()
{
    if (ui->lineEditPointVoltage->text() != "" && ui->lineEditPointNumber->text() == "")
    {
        emit dropStep();
        MSG_WARNING("没有选点");
        return;
    }
    if (ui->lineEditPointVoltage->text().toDouble() > edit->getMaxVol())
    {
        emit dropStep();
        MSG_WARNING("点电压超出范围");
        return;
    }
    emit updateLineEditText();
    emit saveStep();
}

void WindowWaveDesigner::on_pushButtonApply_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::warning(this, "警告", "请确保您十分了解这些参数的意义，否则请不要更改！\n是否应用设置？", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel);
    if (result == QMessageBox::Cancel)
    {
        emit updateGraph();
        return;
    }
    edit->setRealTimeQuantify(CHECKBOX_REALTIME_QUANTIFY);
    edit->quantify(TEXT_UNIT_TIME, TEXT_VOL_QUANTIFY_LEVEL);
    edit->setMaxVol(TEXT_MAX_VOL);
    emit saveStep();
    emit choosePoint(-1);
    emit updateGraph();
}

void WindowWaveDesigner::on_actionSaveAsFile_triggered()
{
    QString aFileName=QFileDialog::getSaveFileName(this,"打开波形文件","","文本文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty())
        return;
    freopen(aFileName.toLatin1(), "w", stdout);
    for (int i=0; i<edit->count(); i++)
        if (printf("   %le", edit->xAt(i))<0)
        {
            MSG_WARNING("文件写入失败");
            fclose(stdout);
            return;
        }
    printf("\n");
    for (int i=0; i<edit->count(); i++)
        if (printf("   %le", edit->getMaxVol()*edit->yAt(i))<0)
        {
            MSG_WARNING("文件写入失败");
            fclose(stdout);
            return;
        }
    fclose(stdout);
}

void WindowWaveDesigner::on_actionOpenFile_triggered()
{
    QString aFileName=QFileDialog::getOpenFileName(this,"打开波形文件","","文本文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty())
        return;
    DialogParameter *wPara = new DialogParameter;
    wPara->exec();
    if (wPara->canceled)
    {
        delete wPara;
        return;
    }
    freopen(aFileName.toLatin1(), "r", stdin);
    char t;
    double n;
    double maxY = wPara->maxVol;
    double dx = 0;
    WaveData data;
    data.clear();
    data.setRealTimeQuantify(wPara->realTimeQuantify);
    data.quantify(wPara->unitTime, wPara->VolQuantiLevel);
    delete wPara;
    QList<double> x, y;
    int err = scanf("%c", &t);
    while (true)
    {
        if (err == EOF)
        {
            MSG_WARNING("文件读取失败");
            fclose(stdin);
            return;
        }
        if (t == '\n')
        {
            scanf("%c", &t);
            break;
        }
        if (t != ' ')
        {
            MSG_WARNING("文件格式错误");
            fclose(stdin);
            return;
        }
        scanf("%lf", &n);
        if (x.count() == 0)
            dx = n;
        else if (n < x.last()+data.getUnitTime()+dx)
        {
            MSG_WARNING("请按时间顺序排列，并且注意间距要大于最小量化时间");
            fclose(stdin);
            return;
        }
        x.append(n-dx);
        err = scanf("%c", &t);
    }
    while (true)
    {
        if (err == EOF || t == '\n')
            break;
        if (t != ' ')
        {
            MSG_WARNING("文件格式错误");
            fclose(stdin);
            return;
        }
        scanf("%lf", &n);
        if (n < 0 || n > maxY)
        {
            MSG_WARNING("电压值超出范围");
            fclose(stdin);
            return;
        }
        y.append(n/maxY);
        err = scanf("%c", &t);
    }
    fclose(stdin);
    if (x.count() != y.count())
    {
        MSG_WARNING("数据数目不对等");
        return;
    }
    data.add(x, y);
    edit->copyData(&data);
    emit saveStep();
    emit choosePoint(-1);
}

void WindowWaveDesigner::on_actionClear_triggered()
{
    WaveData tmp;
    tmp.clear();
    tmp.setRealTimeQuantify(edit->getRealTimeQuantify());
    tmp.setVolQuantiLevel(edit->getVolQuantiLevel());
    tmp.setUnitTime(edit->getUnitTime());
    tmp.setMaxVol(edit->getMaxVol());
    tmp.add(0, 0);
    edit->copyData(&tmp);
    choosePoint(-1);
}
