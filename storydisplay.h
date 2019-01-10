#ifndef STORYDISPLAY_H
#define STORYDISPLAY_H

#include <QPainter>
#include <QScrollArea>
#include <QDebug>

//定义绘制图形线条宽度
#define EventWidth 20.0
//定义图元标签字号
#define CharWidth 16
#define LineWidth 2

/**
 * @brief Component命名空间用于放置自定义的UI组件，其下每个组件可能拥有以“__”开头的同名命名空间
 */
namespace Component {

namespace __storydisplay {

//基本时间点符号
class TimePoint{
public:
    /**
     * @brief 构建一个时间点
     * @param time 时间整数表示
     * @param nextRef 指向下一个时间点的指针，通常是开始时间点指向结束时间点
     */
    explicit TimePoint(qlonglong time, TimePoint *nextRef = nullptr);
    virtual ~TimePoint();

    /**
     * @brief 获取时间点的整数表示
     * @return 整数表示
     */
    qlonglong time() const;
    /**
     * @brief 获取本时间点的下一个
     * @return 指针
     */
    TimePoint * nextRef();

private:
    qlonglong tTime;
    TimePoint *nextPoint;
};

//事件符号、包含开始与结束时间点
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

    /**
     * @brief 获得事件名称
     * @return 名称
     */
    QString name();

    /**
     * @brief 事件开始时间点符号
     * @return 指针
     */
    TimePoint * startTime() const;

    /**
     * @brief 事件结束时间点符号
     * @return 指针
     */
    TimePoint * endTime() const;

private:
    QString tName;
    TimePoint * start;
    TimePoint * end;
};

/**
 * @brief 本控件用于显示事件之间的跨度和交互关系
 */
class StoryCanvas:public QWidget{
    Q_OBJECT
public:
    /**
     * @brief 构建一个基本控件，显示事件之间的跨度
     * @param parent 绑定父组件
     */
    explicit StoryCanvas(QWidget *parent = nullptr);
    virtual ~StoryCanvas() override;

    /**
     * @brief 添加事件符号实例
     * @param idNum 事件指定id，为了便于内外一致，推荐采用数据库主键
     * @param symbo 事件符号实例
     */
    void addEvent(const qlonglong idNum, EventSymbo *const symbo);

    /**
     * @brief 删除内部存在的事件符号
     * @param idNum 事件绑定id
     */
    void removeEvent(const qlonglong idNum);

    /**
     * @brief 获取数据，描述能够容纳此插件的最小空间
     * @return 空间矩形
     */
    QSize getFitSize();

    /**
     * @brief 清空内部所有内容
     */
    void clear();

signals:
    /**
     * @brief 当控件显示空间不足的时候，发送该信号，外部控件将重新调整本控件大小
     */
    void resizeRequest();

    /**
     * @brief 控件焦点转移到另一个事件
     * @param idNum id
     */
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

    static bool compareEvSymbo(EventSymbo* ev1, EventSymbo*ev2);
    static bool compareTimePoint(TimePoint* p1, TimePoint* p2);


};
}

/**
     * @brief 专用容器，承载StoryCanvas，二者结合才是完整控件
     */
class StoryDisplay:public QScrollArea
{
    Q_OBJECT
public:
    explicit StoryDisplay(QWidget *parent = nullptr);
    virtual ~StoryDisplay() override;


    /**
     * @brief 向控件中添加事件
     * @warning 控件不检查idNum，ID重合会直接替换
     * @param idNum 事件id
     * @param name 事件名称
     * @param startT 起始时间
     * @param endT 终止时间
     */
    void addEvent(const qlonglong idNum, QString name, qlonglong startT, qlonglong endT);

    /**
         * @brief 移除指定事件
         * @param idNum 事件绑定id
         */
    void removeEvent(const qlonglong idNum);

    /**
         * @brief 清空控件内容
         */
    void clear();

signals:
    /**
         * @brief 当前焦点位于idNum
         * @param idNum 事件id
         */
    void focuse(qlonglong idNum);
    void deFocuse(qlonglong idNum);


    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
   __storydisplay::StoryCanvas *const canvas;

   /**
        * @brief 向控件中添加事件符号
        * @warning 控件不会对事件id进行检查，如果发现重合id，前一个事件符号将会被后一个替代
        * @param idNum 绑定事件id
        * @param symbo 事件
        */
   void addEvent(const qlonglong idNum, __storydisplay::EventSymbo *const symbo);

private slots:
    void slot_recieveFocuse(qlonglong inNum);
    void slot_recieveDeFocuse(qlonglong inNum);

    void slot_recieveResizeRequest();
};

}

#endif // STORYDISPLAY_H
