#ifndef STORYDISPLAY_H
#define STORYDISPLAY_H

#include <QPainter>
#include <QScrollArea>
#include <QDebug>

//定义绘制图形线条宽度
#define EventWidth 20.0
//定义图元标签字号
#define CharWidth 18
#define LineWidth 2

namespace UIComp {
    class TimePoint{
    public:
        explicit TimePoint(qlonglong time, TimePoint *nextRef = nullptr);
        virtual ~TimePoint();

        qlonglong time() const;
        TimePoint * nextRef();

    private:
        qlonglong tTime;
        TimePoint *nextPoint;
    };

    class EventSymbo{
    public:
        /**
         * @brief 新建一个事件阶段符号
         * @param name 名称
         * @param startT 起始时间
         * @param endT 终止时间
         */
        explicit EventSymbo(QString name, qlonglong startT, qlonglong endT);
        virtual ~EventSymbo();

        QString name();
        TimePoint * startTime() const;
        TimePoint * endTime() const;

    private:
        QString tName;
        TimePoint * start;
        TimePoint * end;
    };

    class StoryCanvas:public QWidget{
        Q_OBJECT
    public:
        explicit StoryCanvas(QWidget *parent = nullptr);
        virtual ~StoryCanvas() override;

        void addEvent(const qlonglong idNum, EventSymbo *const symbo);
        void removeEvent(const qlonglong idNum);
        QSize getFitSize();
        void clear();

    signals:
        void resizeRequest();

        void focuse(qlonglong idNum);
        void deFocuse(qlonglong idNum);


        // QWidget interface
    protected:
        virtual void paintEvent(QPaintEvent *event) override;
        virtual void mousePressEvent(QMouseEvent *event) override;

    private:
        qlonglong focuseID;
        //事件集合:idNum      eventSymbo
        QHash<qlonglong, EventSymbo*> evCon;
        //     列及列内事件阶段队列
        QList<QList<EventSymbo*>*> colsLayout;
        QList<TimePoint *> timeLine;
        //绘制过程中聚焦控制
        QHash<int, EventSymbo*> paintCtrl;
        double fitWidth;
        QRectF focuseRect;

        void eventSymboReLayout();


    };

    class StoryDisplay:public QScrollArea
    {
        Q_OBJECT
    public:
        explicit StoryDisplay(QWidget *parent = nullptr);
        virtual ~StoryDisplay() override;

        void addEvent(const qlonglong idNum, EventSymbo *const symbo);
        void removeEvent(const qlonglong idNum);
        void clear();

    signals:
        void focuse(qlonglong idNum);
        void deFocuse(qlonglong idNum);


        // QWidget interface
    protected:
        virtual void paintEvent(QPaintEvent *event) override;

    private:
        StoryCanvas *const canvas;

    private slots:
        void slot_recieveFocuse(qlonglong inNum);
        void slot_recieveDeFocuse(qlonglong inNum);

        void slot_recieveResizeRequest();
    };

}

#endif // STORYDISPLAY_H
