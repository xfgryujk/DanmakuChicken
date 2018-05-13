# -*- coding: utf-8 -*-

"""
从B站直播获取弹幕
https://github.com/lyyyuna/bilibili_danmu
"""

import asyncio
import json
import random
import xml.dom.minidom
from struct import pack, unpack

import requests

from danmaku_chicken import add_danmaku

# 获取弹幕的房间号，URL后面跟的那个
ROOM_ID = 139


class bilibiliClient():
    def __init__(self, room_id):
        self._CIDInfoUrl = 'http://live.bilibili.com/api/player?id=cid:'
        self._roomId = room_id
        self._ChatPort = 788
        self._protocolversion = 1
        self._reader = None
        self._writer = None
        self.connected = False
        self._UserCount = 0
        self._ChatHost = 'livecmt-1.bilibili.com'

    async def connectServer(self):
        print ('正在进入房间。。。。。')
        r = requests.get('https://api.live.bilibili.com/room/v1/Room/room_init?id=' + str(self._roomId))
        self._roomId = r.json()['data']['room_id']
        r = requests.get(self._CIDInfoUrl + str(self._roomId))
        if r.status_code == 200:
            xml_string = '<root>' + r.text + '</root>'
            dom = xml.dom.minidom.parseString(xml_string)
            root = dom.documentElement
            server = root.getElementsByTagName('server')
            self._ChatHost = server[0].firstChild.data

        reader, writer = await asyncio.open_connection(self._ChatHost, self._ChatPort)
        self._reader = reader
        self._writer = writer
        print ('链接弹幕中。。。。。')
        if (await self.SendJoinChannel(self._roomId) == True):
            self.connected = True
            print ('进入房间成功。。。。。')
            print ('链接弹幕成功。。。。。')
            await self.ReceiveMessageLoop()

    async def HeartbeatLoop(self):
        while self.connected == False:
            await asyncio.sleep(0.5)

        while self.connected == True:
            await self.SendSocketData(0, 16, self._protocolversion, 2, 1, "")
            await asyncio.sleep(30)


    async def SendJoinChannel(self, channelId):
        self._uid = (int)(100000000000000.0 + 200000000000000.0*random.random())
        body = '{"roomid":%s,"uid":%s}' % (channelId, self._uid)
        await self.SendSocketData(0, 16, self._protocolversion, 7, 1, body)
        return True


    async def SendSocketData(self, packetlength, magic, ver, action, param, body):
        bytearr = body.encode('utf-8')
        if packetlength == 0:
            packetlength = len(bytearr) + 16
        sendbytes = pack('!IHHII', packetlength, magic, ver, action, param)
        if len(bytearr) != 0:
            sendbytes = sendbytes + bytearr
        self._writer.write(sendbytes)
        await self._writer.drain()


    async def ReceiveMessageLoop(self):
        while self.connected == True:
            tmp = await self._reader.read(4)
            expr, = unpack('!I', tmp)
            tmp = await self._reader.read(2)
            tmp = await self._reader.read(2)
            tmp = await self._reader.read(4)
            num, = unpack('!I', tmp)
            tmp = await self._reader.read(4)
            num2 = expr - 16

            if num2 != 0:
                num -= 1
                if num==0 or num==1 or num==2:
                    tmp = await self._reader.read(4)
                    num3, = unpack('!I', tmp)
                    print ('房间人数为 %s' % num3)
                    self._UserCount = num3
                    continue
                elif num==3 or num==4:
                    tmp = await self._reader.read(num2)
                    # strbytes, = unpack('!s', tmp)
                    try: # 为什么还会出现 utf-8 decode error??????
                        messages = tmp.decode('utf-8')
                    except:
                        continue
                    self.parseDanMu(messages)
                    continue
                elif num==5 or num==6 or num==7:
                    tmp = await self._reader.read(num2)
                    continue
                else:
                    if num != 16:
                        tmp = await self._reader.read(num2)
                    else:
                        continue

    def parseDanMu(self, messages):
        try:
            dic = json.loads(messages)
        except: # 有些情况会 jsondecode 失败，未细究，可能平台导致
            return
        cmd = dic['cmd']
        if cmd == 'LIVE':
            print ('直播开始。。。')
            return
        if cmd == 'PREPARING':
            print ('房主准备中。。。')
            return
        if cmd == 'DANMU_MSG':
            commentText = dic['info'][1]
            commentUser = dic['info'][2][1]
            isAdmin = dic['info'][2][2] == '1'
            isVIP = dic['info'][2][3] == '1'
            if isAdmin:
                commentUser = '管理员 ' + commentUser
            if isVIP:
                commentUser = 'VIP ' + commentUser
            try:
                print (commentUser + ' say: ' + commentText)
            except:
                pass
            return
        if cmd == 'SEND_GIFT':
            GiftName = dic['data']['giftName']
            GiftUser = dic['data']['uname']
            Giftrcost = dic['data']['rcost']
            GiftNum = dic['data']['num']
            try:
                print(GiftUser + ' 送出了 ' + str(GiftNum) + ' 个 ' + GiftName)
            except:
                pass
            return
        if cmd == 'WELCOME':
            commentUser = dic['data']['uname']
            try:
                print ('欢迎 ' + commentUser + ' 进入房间。。。。')
            except:
                pass
            return
        return


class MyBilibiliClient(bilibiliClient):
    def parseDanMu(self, messages):
        try:
            dic = json.loads(messages)
        except: # 有些情况会 jsondecode 失败，未细究，可能平台导致
            return

        cmd = dic['cmd']
        if cmd == 'DANMU_MSG':
            content = dic['info'][1]
            print(content)
            if not add_danmaku(content):
                print('添加弹幕失败！')


def main():
    danmuji = MyBilibiliClient(ROOM_ID)
    loop = asyncio.get_event_loop()
    while True:
        tasks = [
            danmuji.connectServer(),
            danmuji.HeartbeatLoop()
        ]
        try:
            loop.run_until_complete(asyncio.wait(tasks))
        except KeyboardInterrupt:
            break
        except ConnectionAbortedError:
            print('掉线重连中')
            continue
    loop.close()


if __name__ == '__main__':
    main()
