DanmakuChicken
=========

弹幕鸡，一个在屏幕上显示弹幕的小程序。目前实现了从指定URL获取弹幕（后端请自己实现），和从QQ群获取弹幕（需要[QQBot](https://github.com/pandolia/qqbot)或[酷Q](https://cqp.cc/)）

![screenshot](https://github.com/xfgryujk/DanmakuChicken/blob/master/screenshot.png)


使用方法
---------

编译后先运行`DanmakuChicken.exe`，然后根据需要运行`python`目录下的脚本


主要源码文件
---------

```
.
├─DanmakuChicken                 显示弹幕的exe项目
│  ├─DanmakuChickenDlg.cpp       程序主窗口（设置窗口）、HTTP API
│  ├─OverlayDlg.cpp              显示弹幕的窗口
│  └─Danmaku.cpp                 弹幕逻辑
├─python                         获取弹幕的Python程序
│  ├─danmaku_chicken.py          弹幕鸡Python API
│  ├─from_coolq.py               从QQ群获取弹幕，安卓版QQ协议，需要酷Q
│  ├─from_smartqq.py             从QQ群获取弹幕，SmartQQ协议，需要QQBot
│  └─from_url.py                 从指定URL获取弹幕
└─Simple-Web-Server              C++ HTTP服务器库
```


API
---------

默认HOST为`localhost:12450`

### 添加弹幕

URL：`/danmaku`  
方式：POST  
内容：`{"content": "弹幕内容"}`  
返回：成功则状态码为200，失败则状态码为400，内容为`{"error": "错误信息"}`
