DanmakuChicken
=========

弹幕鸡，一个在屏幕上显示弹幕的小程序。目前实现了从指定URL获取弹幕（后端请自己实现），和从QQ群获取弹幕（需要[QQBot](https://github.com/pandolia/qqbot)）

![screenshot](https://github.com/xfgryujk/DanmakuChicken/blob/master/screenshot.png)


使用方法
---------

编译后先运行`DanmakuChicken.exe`，然后根据需要运行`python`目录下的`danmaku_chicken.py`或`run_qqbot.bat`


主要源码文件
---------

```
.
├─DanmakuChicken                 显示弹幕的exe项目
│  ├─DanmakuChickenDlg.cpp       程序主窗口（设置窗口）、HTTP API
│  ├─OverlayDlg.cpp              显示弹幕的窗口
│  └─Danmaku.cpp                 弹幕逻辑
├─python                         获取弹幕的python程序
│  ├─danmaku_chicken.py          从指定URL获取弹幕
│  ├─get_danmaku_from_qq.py      QQBot插件，从QQ群获取弹幕
│  └─run_qqbot.bat               启动QQBot并加载插件的脚本
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
