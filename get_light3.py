import time
from media.sensor import *
from media.display import *
from media.media import *
from ybUtils.YbKey import YbKey
from ybUtils.YbUart import YbUart

key = YbKey()
uart = YbUart(baudrate=115200)

WIDTH = 640
HEIGHT = 480
sensor = Sensor(width = WIDTH, height = HEIGHT, fps=60)
sensor.reset()
time.sleep_ms(100)
sensor.set_framesize(width = WIDTH, height = HEIGHT)
sensor.set_pixformat(sensor.RGB565)

light = (14, 100, -45, 102, -13, 41)#(70, 100, -19, 26, -1, 126)#(21, 100, -39, 65, 14, 37)#(66, 100, -50, 74, 5, 26)#(21, 100, -39, 65, 14, 37)#(64, 97, -52, 59, -17, 9)
red_light = (68, 100, 2, 39, -16, 30)#(58, 99, 11, 55, 4, 28)#(64, 99, 1, 50, 6, 27)#(13, 100, 11, 52, 3, 38)
green_light = (91, 99, -78, -10, -14, 25)#(69, 100, -23, 2, -21, 15)
light_roi =(45,40,440,434)#根据自己的硬件结构修改！！！

color_list = [red_light, green_light]

Display.init(Display.ST7701, width = WIDTH, height = HEIGHT, to_ide = True)
MediaManager.init()

sensor.run()

def color_packet_sent(img, hano, color_list):
    # results[i] 存储第i种颜色的所有blob
    results = [[] for _ in color_list]
    blobs = img.find_blobs([hano], roi=light_roi, merge=True)
    if blobs:
        for blob in blobs:
            img.draw_cross(blob[5], blob[6], color=(255,255,255), thickness=1)
            roi = (blob.x()-5, blob.y()-5, blob.w()+10, blob.h()+10)
            for idx, color in enumerate(color_list):
                color_blobs = img.find_blobs([color], roi=roi, x_stride=1, y_stride=1,
                                             area_threshold=0, pixels_threshold=0, merge=False, margin=1)
                if color_blobs:
                    # 选面积最大的色块
                    max_blob = max(color_blobs, key=lambda b: b.area())
                    cx = max_blob[5]
                    cy = max_blob[6]
                    results[idx].append((cx, cy, max_blob.area()))

    # 对每种颜色的点选面积最大的
    best_points = []
    for pts in results:
        if pts:
            # 选面积最大的点
            best = max(pts, key=lambda p: p[2])
            best_points.append((best[0], best[1]))
        else:
            best_points.append((0, 0))  # 没检测到该颜色时填0

    # 画十字
    for x, y in best_points:
        img.draw_cross(x, y, color=(255,0,0), thickness=1)

   # 如果有任意一个颜色的坐标为(0, 0)，则不发送
    if any((x == 0 and y == 0) for x, y in best_points):
        return None

   # 画十字
    for x, y in best_points:
        img.draw_cross(x, y, color=(255,0,0), thickness=1)

    # 打包
    bufs = []
    for x, y in best_points:
        x = max(0, min(65535, int(x)))
        y = max(0, min(65535, int(y)))
        x_high = (x >> 8) & 0xFF
        x_low = x & 0xFF
        y_high = (y >> 8) & 0xFF
        y_low = y & 0xFF
        buf = bytes([x_high, x_low, y_high, y_low])
        bufs.append(buf)

    all_bufs = b''.join(bufs)
    packet = bytes([0xA3]) + all_bufs + bytes([0xC3])
    uart.write(packet)
    return packet


while True:
    os.exitpoint()
    img = sensor.snapshot()
    img = img.gaussian(1)
    #img = sensor.snapshot().lens_corr(strength = 0.8 , zoom = 1.0)
    color_packet_sent(img, light, color_list)
    img.draw_rectangle(light_roi, color = (255, 255, 255), thickness=1)
    Display.show_image(img)



