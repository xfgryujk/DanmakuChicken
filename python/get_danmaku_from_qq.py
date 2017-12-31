# -*- coding: utf-8 -*-

"""
QQBot插件
https://github.com/pandolia/qqbot
"""

from danmaku_chicken import add_danmaku


def onQQMessage(bot, contact, member, content):
    if contact.qq == '414754582':
        add_danmaku(content)
