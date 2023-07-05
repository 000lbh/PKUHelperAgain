#ifndef COURSETABLE_HPP
#define COURSETABLE_HPP

#include <QVector>
#include <QString>
#include <QObject>
#include <QNetworkAccessManager>

const auto query_course_type = {
    "0", // Default
    "1-08", // 思想政治
    "1-09", // 大学英语
    "1-11", // 体育
    "2-通识核心课",
    "2-通选课",
    "3-英文",
    "2-与中国有关课程",
    "1-07", // 全校公选课
};

// Edge User-Agent Header
const char user_agent[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Edg/114.0.1823.43";

// It can be better designed, what are your opinions?
struct CourseTime {
    int week_start;
    int week_stop; // Included
    int day_in_week;
    int time_in_day;
    int step;
    QString classroom; // Course info from dean does not include this

    CourseTime() = default;
    CourseTime(const QString &s);
    CourseTime(int w1, int w2, int d, int t, int step, QString c) :
        week_start(w1), week_stop(w2), day_in_week(d),
        time_in_day(t), step(step), classroom(c) {}
    QString repr();
    // Should I put an iterator here?

    friend QDataStream &operator<<(QDataStream &stream, const CourseTime &time);
    friend QDataStream &operator>>(QDataStream &stream, CourseTime &time);
};

struct CourseEntry {
    QString id;  // Can include non-digit characters
    QString course_name;
    QString eng_name;
    QString type_name;
    QString college_name;
    int class_no;
    double credit; // Can be non-integer, such as 2.5
    QString execute_plan_id; // Internal use
    QVector<CourseTime> time; // May have multiple time schedules
    QVector<QPair<QString, QString>> teachers; // teacher name and its title
    QString remarks;
    QString grade;
    double grade_point;

    enum JsonSource {
        Dean,
        PortalScore,
        PortalTable,
    };

    CourseEntry() = default;
    // Helper constructor function
    CourseEntry(const QJsonObject &entry, JsonSource source = Dean);

    bool is_same(const CourseEntry &other) const;
    bool operator==(const CourseEntry &other) const ;
};

struct QueryData {
    QString course_name;
    QString teacher_name;
    QString xndxq;
    QString course_type;
    QString college;
    int startrow;
};

class CourseTable : public QObject {
    Q_OBJECT
private:
    QVector<CourseEntry> _course_data;
    bool available;
    QNetworkAccessManager qnam;
    QueryData temp_req; // for network use
public:
    explicit CourseTable(QObject *parent = nullptr);
    // asynchronize get data from web, emit signal void ready() when finished,
    // or void fail() when failed
    void online_get(const QueryData &req = QueryData());
    // synchronize get data from database
    bool cache_get(QString path);
    // synchronize save data to database
    bool cache_store(QString path);
    QVector<CourseEntry> search(const QueryData &condition);
private slots:
    void online_get_slot(QueryData req, int start);
    void qnam_request_finished(QNetworkReply *response);
signals:
    void ready(); // When online get becomes ready, emit this signal;
    void fail(QString reason); // When online get fails, emit this signal;
    void online_get_signal(QueryData req, int start); // For internal use only
    void progress_update(int total, int current);
};

#endif // COURSETABLE_HPP
