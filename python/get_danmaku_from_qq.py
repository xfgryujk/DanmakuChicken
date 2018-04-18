# -*- coding: utf-8 -*-

"""
QQBot插件
https://github.com/pandolia/qqbot
"""

from danmaku_chicken import add_danmaku


def onQQMessage(bot, contact, member, content):
    if contact.uin == '389947752':
        add_danmaku(content)
