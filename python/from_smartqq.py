# -*- coding: utf-8 -*-

"""
QQBot插件
https://github.com/pandolia/qqbot
"""

import sys
from os.path import realpath, dirname

from qqbot import _bot as bot

from danmaku_chicken import add_danmaku

# 获取弹幕的群名
GROUP_NAME = 'BUAA-飞梦ACG联盟'

FILE_DIR = dirname(realpath(__file__))


def onQQMessage(bot, contact, member, content):
    # print(contact.__dict__)
    # 由于腾讯关闭接口，QQ号已无法获取
    if contact.name == GROUP_NAME:
        print(content)
        if not add_danmaku(content):
            print('添加弹幕失败！')


def main():
    # 参数 -q <QQ号> 可以指定使用之前登录过的账号
    argv = ['-pp', FILE_DIR, '-pl', 'from_smartqq'] + sys.argv[1:]
    bot.Login(argv)
    bot.Run()


if __name__ == '__main__':
    main()
