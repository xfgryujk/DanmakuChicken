# -*- coding: utf-8 -*-

from threading import Thread
from time import sleep

import requests

# 获取弹幕的地址
DANMAKU_URL = 'http://buaaacg.org/wx/danmaku.php'


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


def get_danmaku_from_url_thread():
    """
    获取新弹幕线程
    """

    # 取最后弹幕ID
    res = requests.get(DANMAKU_URL + '?action=getlastid').json()
    last_danmaku_id = res['lastid']

    while True:
        res = requests.get('{}?action=getdanmaku&start={}'
                           .format(DANMAKU_URL, last_danmaku_id + 1)
                           ).json()

        for danmaku in res:
            print(danmaku['content'])
            if not add_danmaku(danmaku['content']):
                print('添加弹幕失败！')

            if danmaku['id'] > last_danmaku_id:
                last_danmaku_id = danmaku['id']

        sleep(2)


def main():
    Thread(target=get_danmaku_from_url_thread).start()


if __name__ == '__main__':
    main()
