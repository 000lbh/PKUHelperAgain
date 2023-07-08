# PKUHelperAgain

Make PKU Helper Great Again!!!

## 1 程序功能介绍

**PKU Helper Again**是仿照旧版PKU Helper的功能设计出的软件，旨在帮助同学们安排自己的课程规划、个人成绩、日常安排等。其主要功能包括在线查询成绩、安排日程、远程联网查询课表及课程安排等。

## 1.1 成绩查询

程序设计了登录模块。本程序通过一种隐私安全的方式调用IAAA接口实现远端登录，并可以向服务器在线请求个人的成绩单。查询成功后，成绩单将显示在表格控件当中，并能够以多种方式进行成绩着色，同时可以以可变间隔刷新并在通知栏进行提示，方便可视化查看个人成绩情况。

## 1.2 日程安排

程序保存了一个以日期为索引的日程安排，每一天可以保存多条不同时间的日程安排，可以精确到具体时间，日历的每一天都可以添加多条带有标题，内容和具体时间的日程。有日程的时间在日历上会高亮显示，点击该天将显示出当天的所有待办事项。
同时从课程查询得到的课程可以直接添加到这里，进行编辑。准备实现课程直接添加到课表，但是时间不够，没有完成。

## 1.3 课程查询

程序设有数据库，可以存储以学年度学期为索引的课表。用户首次查询时，可以通过向Dean请求课程表的方式更新课表缓存。基于用户提供的索引，程序将返回对应的课程。课程的各具体参数如授课教师、上课时间等将显示在一个表格控件上，用户可以通过所提供的超链接快捷跳转至课程详情页面。在一次查询后，该学期课表将自动被添加到数据库缓存当中，并可以快捷地迅速查找所需的课程。

## 1.4 其他功能

程序对以上个人数据按照用户进行持久化，每次启动程序时即可自动加载。程序支持开机启动（仅Windows，Linux用户请自行写脚本完成），支持托盘组件，方便长期在后台工作，进行成绩查询等工作。目前掉线自动重登还没有做，希望以后有机会能完成。

## 2 项目各模块与类设计细节

## 2.1 前端

本项目的主页面和其他页面均有单独的类，主要页面使用单例调用模式控制。面板控件均在类内设置有对应的槽函数，并通过良好的封装实现了前后端的高效交互。根据需要我们派生了自定义的类，实现了更加强大的功能，比如可点击的QLabel，比如可存储更多数据的ItemWidget等等。

## 2.2 后端

### 2.2.1 统一数据库

我们将所有数据持久化（除了简单的设置选项），全部通过这个类进行。不过这个类并没有必要有实例，因此或许把他变成命名空间会更好。目前我们只使用这个类的一个实例。这个类包括全校课表的SQLite数据库接口，包括个人成绩数据，课程和ddl数据的接口，后两个接口直接将Qt实例使用QDataStream进行序列化存储，而SQLite中部分复合数据也使用此法进行存储。

### 2.2.2 课表查询

我们将课表查询类封装，通过QNetworkAccessManager进行数据爬取。由于Dean一次只返回10条，因此爬取速度较慢。我们使用了SQLite数据库对全校课表进行缓存。首次查询前建议先缓存课表，大学期约2分钟，暑假小学期约10秒。QNetworkAccessManager是异步返回结果，因此我们将结果处理放在了一个槽函数，同时通过锁定界面等待的方式确保状态合法。由于这里需要不定次数的请求，因此我们的结果处理会再次发射信号调用数据获取，数据获取完成之后会自动调用结果处理来完成。由于这种异步处理较为麻烦，也不能通过简单地等待信号来达成（由于Qt是事件驱动，因此函数内部基本可以保证原子性，因此如果在函数内部等待本线程的一个信号，那就相当于把中断关掉了等中断，陷入死锁）。这期间我们考虑过其他http库，但是考虑到在Windows下配置环境是一件极其痛苦的事情，最后我们没有打算这么做。相关尝试可以在backend_deprecated分支中看到。当初次缓存全校课表后，再次查询会直接走数据库接口获取信息。

### 2.2.3 IAAA登录

IAAA登录我们只使用一个实例。考虑到以后的多用户支持，IAAA也有公开的构造函数。IAAA负责存储用户名和密码（密码只写不读），并进行登录，获得token。token为一次性使用，PKUPortal类是IAAA的友元类，他会使用这个token，同时将IAAA变为无效登录状态，再次使用时需要重新登录。我们使用这个类判断当前用户，但不判断其在线状态。我们提供了离线登录模式，此时不需要联网获取的数据仍可以使用。以后我们打算接入扫码登录。

### 2.2.4 Portal登录

Portal需要从IAAA获取token，然后进行一次get访问，全程Cookie要放入CookieJar中，以保存登陆状态。后续成绩单获取依赖这个CookieJar登陆状态，因此ScoreSheet类是其友元类。网络访问的流程和刚刚类似，不再赘述。

### 2.2.5 成绩单

成绩单同样是从Portal特定位置获取，需要Portal登录类提供的Cookie。这里需要发送一个空白post请求即可获得成绩单。然后我们按学期解析成绩单，美学期的课程以列表放置，课程本身用一个结构体描述，和课表查询使用的一样，但是填充数据不尽相同。