# -*- coding: utf-8 -*-

"""
从指定URL获取弹幕（后端请自己实现）
"""

from time import sleep

import requests

from danmaku_chicken import add_danmaku

# 获取弹幕的地址
DANMAKU_URL = 'http://buaaacg.org/wx/danmaku.php'


def main():
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


if __name__ == '__main__':
    main()
