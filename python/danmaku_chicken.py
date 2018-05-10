# -*- coding: utf-8 -*-

import requests


def add_danmaku(content):
    """
    添加弹幕
    :param content: 弹幕内容
    :return: 是否成功
    """

    try:
        res = requests.post('http://localhost:12450/danmaku',
                            json={'content': content})
    except:
        return False
    return res.status_code == 200
