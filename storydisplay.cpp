#include "storydisplay.h"

#include <QMouseEvent>

using namespace Component;
using namespace Component::__storydisplay;

TimePoint::TimePoint(qlonglong time, TimePoint *nextRef):
    tTime(time),
    nextPoint(nextRef){}

TimePoint::~TimePoint(){}

qlonglong TimePoint::time() const {
    return tTime;
}

TimePoint *TimePoint::nextRef(){
    return this->nextPoint;
}

EventSymbo::EventSymbo(QString name, qlonglong startT, qlonglong endT){
    this->tName = name;
    this->end = new TimePoint(endT);
    this->start = new TimePoint(startT, end);
}

EventSymbo::~EventSymbo(){}

QString EventSymbo::name(){return this->tName;}

TimePoint *EventSymbo::startTime() const {return this->start;}

TimePoint *EventSymbo::endTime() const {return this->end;}

StoryCanvas::StoryCanvas(QWidget *parent):
    QWidget (parent),
    focuseID(-1),
    fitWidth(20),
    focuseRect(QRectF(0.0,0.0,0.0,0.0))
{

}

StoryCanvas::~StoryCanvas()
{
    this->clear();
}

void StoryCanvas::addEvent(const qlonglong idNum, EventSymbo * const symbo)
{
    this->evCon.insert(idNum, symbo);
    this->eventSymboReLayout();
}

void StoryCanvas::removeEvent(const qlonglong idNum)
{
    this->evCon.remove(idNum);
    this->eventSymboReLayout();
}

QSize StoryCanvas::getFitSize()
{
    double h = this->timeLine.size() * EventWidth;
    return QSize((int)this->fitWidth, (int)h + 4);
}

void StoryCanvas::clear()
{
    for(int i=0; i<colsLayout.size();){
        delete colsLayout.takeAt(i);
    }

    this->focuseID = -1;
    this->timeLine.clear();
    this->paintCtrl.clear();

    auto values = this->evCon.values();
    this->evCon.clear();
    for(int i=0; i<values.size();){
        delete values.takeAt(0);
    }

    this->eventSymboReLayout();
}

void StoryCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    int count = this->timeLine.size();
    if(count <= 1){
        return;
    }
    auto startPoint = this->timeLine.first();
    auto lastPoint = this->timeLine.last();
    auto colorScale = 255*255*255.0;
    auto colorStep = colorScale/(lastPoint->time() - startPoint->time());



    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRect(0,0, width(), height());

    QFont font = painter.font();
    font.setPixelSize(CharWidth);
    painter.setFont(font);

    painter.translate(3,2);



    //以节点截断为基础，渐次绘制
    for(int i=0; i<this->timeLine.size()-1 ; ++i){

        int colIndex=0;
        for(;colIndex < colsLayout.size(); ++colIndex){
            auto targetEvent = paintCtrl.value(colIndex);

            if(targetEvent == nullptr){
                targetEvent = colsLayout.at(colIndex)->at(0);
                paintCtrl.insert(colIndex, targetEvent);
            }

            int tStartIndex = this->timeLine.indexOf(targetEvent->startTime());
            int tEndIndex = this->timeLine.indexOf(targetEvent->endTime());
            if(tStartIndex > i || tEndIndex < i+1){
                continue;
            }

            QPainterPath path;
            path.setFillRule(Qt::WindingFill);

            QPointF firstP1(colIndex * (EventWidth+3) , EventWidth * i);
            QPointF firstP2(firstP1.x() + EventWidth , firstP1.y() + EventWidth);
            QPointF middleP1(colIndex * (EventWidth+3), EventWidth *(0.5 + i));
            QPointF middleP2(middleP1.x() + EventWidth, middleP1.y() + EventWidth);
            QPointF secondP1(colIndex * (EventWidth+3) , EventWidth * (i+1));
            QPointF secondP2(secondP1.x() + EventWidth ,secondP1.y() + EventWidth);


            //如果是事件开始则绘制圆头
            if(i == tStartIndex){
                path.addEllipse(QRectF(firstP1, firstP2));

                QPen pen(Qt::black, LineWidth, Qt::DotLine);
                painter.setPen(pen);

                QPointF headPoint(firstP1.x() + EventWidth/2, firstP1.y());
                QRectF temp1 = QRectF(QPointF(colsLayout.size()*(EventWidth+3)+30, i * EventWidth), QSizeF(CharWidth, 5*CharWidth));
                QRectF realRect = painter.boundingRect(temp1, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, targetEvent->name());
                QPointF trailPoint(colsLayout.size()*(EventWidth+3)+30, (i + 1)*realRect.height());

                double widthNeed = colsLayout.size()*(EventWidth+3)+30 + realRect.width();
                if(this->fitWidth < widthNeed){
                    this->fitWidth = widthNeed + 10;
                    emit this->resizeRequest();
                }

                painter.drawLine(headPoint, trailPoint);
                painter.drawText(QPointF(colsLayout.size()*(EventWidth+3)+30, (i + 1)*realRect.height()), targetEvent->name());
            }else {
                path.addRect(QRectF(firstP1, firstP2));
            }

            path.addRect(QRectF(middleP1, middleP2));

            //如果是事件结束则绘制圆头
            if(i+1 == tEndIndex){
                path.addEllipse(QRectF(secondP1, secondP2));

                auto colTarget = colsLayout.at(colIndex);
                auto index = colTarget->indexOf(targetEvent);

                EventSymbo * val = nullptr;
                if(index < colTarget->size()-1){
                    val = colTarget->at(index + 1);
                }
                paintCtrl.insert(colIndex, val);

            }else {
                path.addRect(QRectF(secondP1, secondP2));
            }

            auto timeSpan = targetEvent->endTime()->time() - targetEvent->startTime()->time();
            auto colorNum = colorStep * timeSpan;

            auto valR =(int) (colorNum) / (255*255);
            auto valG =(int) (colorNum - valR*255*255) / 255;
            auto valB =(int) (colorNum - valR*255*255 - valG*255) / 255;

            QColor c(valR,valG,valB);

            painter.setBrush(c);
            QPen pen(c);
            painter.setPen(pen);
            painter.drawPath(path);

            if(this->focuseRect.width() > 1.0){
                painter.setPen(Qt::gray);
                painter.setBrush(QColor(0,0,0,0));
                painter.drawRect(this->focuseRect);
            }

        }
    }
}

void StoryCanvas::mousePressEvent(QMouseEvent *event)
{
    auto point = event->pos();

    auto colIndex =(int) (point.x() / (EventWidth+3));
    auto rowIndex =(int) (point.y() / EventWidth);

    if(colIndex < this->colsLayout.size()){
        auto colist = this->colsLayout.at(colIndex);

        for(int i=0; i<colist->size(); ++i){
            auto targetEvent = colist->at(i);
            auto startIndex = timeLine.indexOf(targetEvent->startTime());
            auto endIndex = timeLine.indexOf(targetEvent->endTime());

            if(startIndex <= rowIndex && endIndex >= rowIndex){
                auto x = this->evCon.keys(targetEvent);
                this->focuseID = x.at(0);
                auto nodeSpan = endIndex - startIndex + 1;
                QPointF ltop(colIndex * (EventWidth + 3) -1, this->timeLine.indexOf(targetEvent->startTime()) * EventWidth);
                this->focuseRect = QRectF(ltop, QSizeF(EventWidth + 2, nodeSpan * EventWidth));

                emit this->focuse(this->focuseID);
                this->repaint();
                return;
            }
        }
    }

    this->focuseRect = QRectF(QPointF(0,0), QSizeF(0,0));
    if(this->focuseID != -1){
        emit this->deFocuse(this->focuseID);
        this->focuseID = -1;
    }

    this->repaint();
}

void StoryCanvas::eventSymboReLayout()
{
    for(int i=0; i<colsLayout.size();){
        delete colsLayout.takeAt(i);
    }

    auto vals = this->evCon.values();
    std::sort(vals.begin(), vals.end(), StoryCanvas::compareEvSymbo);
    this->timeLine.clear();

    for(int i=0; i<vals.size(); ++i){
        auto target = vals.at(i);
        this->timeLine.append(target->startTime());
        this->timeLine.append(target->endTime());

        int col=0;
        for(; col<colsLayout.size(); ++col){
            auto colTarget = colsLayout.at(col);

            auto last = colTarget->last();
            if(StoryCanvas::compareTimePoint(last->endTime(), target->startTime())){
                colTarget->append(target);
                break;
            }
        }
        if(col == colsLayout.size()){
            auto newCol = new QList<EventSymbo*>();
            newCol->append(target);
            this->colsLayout.append(newCol);
            this->paintCtrl.insert(col, target);
        }
    }
    std::sort(this->timeLine.begin(), this->timeLine.end(), StoryCanvas::compareTimePoint);
}

bool StoryCanvas::compareEvSymbo(EventSymbo *ev1, EventSymbo *ev2)
{
    return StoryCanvas::compareTimePoint(ev1->startTime(), ev2->startTime());
}

bool StoryCanvas::compareTimePoint(TimePoint *p1, TimePoint *p2)
{
    if(p1->time() < p2->time())
        return true;
    return false;
}



StoryDisplay::StoryDisplay(QWidget *parent):
    QScrollArea(parent),
    canvas(new StoryCanvas(this))
{
    this->setWidget(canvas);
    this->connect(this->canvas, &StoryCanvas::focuse,
                  this,         &StoryDisplay::slot_recieveFocuse);
    this->connect(this->canvas, &StoryCanvas::deFocuse,
                  this,         &StoryDisplay::slot_recieveDeFocuse);
    this->connect(this->canvas, &StoryCanvas::resizeRequest,
                  this,         &StoryDisplay::slot_recieveResizeRequest);
}

StoryDisplay::~StoryDisplay() {
    delete this->canvas;
}

void StoryDisplay::addEvent(const qlonglong idNum, EventSymbo * const symbo){
    this->canvas->addEvent(idNum, symbo);

    auto size = this->canvas->getFitSize();
    this->canvas->resize(size.width(), size.height());
}

void StoryDisplay::addEvent(const qlonglong idNum, QString name, qlonglong startT, qlonglong endT)
{
    auto symbo = new EventSymbo(name, startT, endT);
    this->addEvent(idNum, symbo);
}

void StoryDisplay::removeEvent(const qlonglong idNum){
    this->canvas->removeEvent(idNum);

    auto size = this->canvas->getFitSize();
    this->canvas->resize(size.width(), size.height());
}

void StoryDisplay::clear()
{
    this->canvas->clear();

    auto size = this->canvas->getFitSize();
    this->canvas->resize(size.width(), size.height());
}

void StoryDisplay::paintEvent(QPaintEvent *event){
    QScrollArea::paintEvent(event);
    this->canvas->repaint();
}

void StoryDisplay::slot_recieveFocuse(qlonglong inNum)
{
    emit this->focuse(inNum);
}

void StoryDisplay::slot_recieveDeFocuse(qlonglong inNum)
{
    emit this->deFocuse(inNum);
}

void StoryDisplay::slot_recieveResizeRequest()
{
    auto s = this->canvas->getFitSize();
    this->canvas->resize(s.width(),s.height());
}
