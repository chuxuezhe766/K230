import time, os, sys
from media.sensor import *
from media.display import *
from media.media import *
from libs.YbProtocol import YbProtocol
from ybUtils.YbUart import YbUart
from libs.otherKey import YbKey1, YbKey2

# 初始化串口、协议、按键等 / Initialize UART, protocol, keys, etc.
uart = YbUart(baudrate=115200)
pto = YbProtocol()
key = YbKey2()
key1 = YbKey1()
mode = 1
num = 0
data = 0

# 显示参数 / Display parameters
DISPLAY_WIDTH = 640   # LCD显示宽度 / LCD display width
DISPLAY_HEIGHT = 480  # LCD显示高度 / LCD display height

# LAB颜色空间阈值 / LAB color space thresholds
# (L Min, L Max, A Min, A Max, B Min, B Max)
THRESHOLDS = [
# 红色阈值 / Red threshold，(0, 91, 33, 89, -33, 53),(70, 87, 1, 54, -34, 28)，黑线里(63, 80, -1, 62, -34, 28)
    (67, 100, 4, 40, -12, 24),    # 红色阈值 / Red threshold，(0, 91, 33, 89, -33, 53),(80, 100, -18, 44, -26, 33)
    (42, 100, -128, -17, 6, 66),     # 绿色阈值 / Green threshold
    (43, 99, -43, -4, -56, -7),      # 蓝色阈值 / Blue threshold
    (37, 100, -128, 127, -128, -27)    # 亚博智能Logo的颜色 color of YAHBOOM
]

sensor = Sensor(width = DISPLAY_WIDTH, height = DISPLAY_HEIGHT, fps=30)
sensor.reset()
time.sleep_ms(100)
sensor.set_framesize(width = DISPLAY_WIDTH, height = DISPLAY_HEIGHT)
sensor.set_pixformat(sensor.RGB565)

flag_find_first_rect = False
flag_find_second_rect = False
first_rect_corners = [[0,0] for _ in range(4)]
second_rect_corners =[[0,0] for _ in range(4)]
show_first_rect = True
show_second_rect = True
show_target_rect = True

Display.init(Display.ST7701, width = DISPLAY_WIDTH, height = DISPLAY_HEIGHT, to_ide = True)
MediaManager.init()

sensor.run()
clock = time.clock()

found_rect = 0
send_state = 0


def get_closest_rgb(lab_threshold):
    """根据LAB阈值计算最接近的RGB颜色 / Calculate closest RGB color based on LAB threshold"""
    # 获取LAB空间的中心点值
    l_center = (lab_threshold[0] + lab_threshold[1]) // 2
    a_center = (lab_threshold[2] + lab_threshold[3]) // 2
    b_center = (lab_threshold[4] + lab_threshold[5]) // 2
    return image.lab_to_rgb((l_center,a_center,b_center))

def process_blobs(img, blobs, color):
    """处理检测到的色块 / Process detected color blobs"""
    for blob in blobs:
        img.draw_rectangle(blob[0:4], color=color, thickness=4)  # 画色块矩形框
        img.draw_cross(blob[5], blob[6], color=color, thickness=2)  # 在色块中心画十字
        x = blob[0]
        y = blob[1]
        w = blob[2]
        h = blob[3]
        pto_data = pto.get_color_data(x, y, w, h)  # 获取色块数据
        #uart.send(pto_data)  # 可选：通过串口发送色块数据
        print(pto_data)      # 打印色块数据
        break                # 只处理第一个色块

def draw_message(img, message):
    """绘制message信息 / Draw message information"""
    img.draw_string_advanced(0, 0, 30, f'Mode: {message:.3f}', color=(255, 255, 255))

def send_xy(x, y):
    # 转为整数并限制范围，支持最大 65535
    x = max(0, min(65535, int(x)))
    y = max(0, min(65535, int(y)))
    # 拆分为高低字节
    x_high = (x >> 8) & 0xFF
    x_low = x & 0xFF
    y_high = (y >> 8) & 0xFF
    y_low = y & 0xFF
    # 按协议组包：帧头、x高、x低、y高、y低、帧尾
    buf = bytes([0xA3, x_high, x_low, y_high, y_low, 0xC3])
    uart.write(buf)  # 通过串口发送数据

def draw_first_rect(img, corners, shrink_ratio_x, shrink_ratio_y):
    # 多轮合并x/y
    new_corners = [list(pt) for pt in corners]
    changed = True
    while changed:
        changed = False
        for i in range(4):
            for j in range(i+1, 4):
                if abs(new_corners[i][0] - new_corners[j][0]) < 4:
                    avg_x = int(round((new_corners[i][0] + new_corners[j][0]) / 2))
                    if new_corners[i][0] != avg_x or new_corners[j][0] != avg_x:
                        new_corners[i][0] = avg_x
                        new_corners[j][0] = avg_x
                        changed = True
    changed = True
    while changed:
        changed = False
        for i in range(4):
            for j in range(i+1, 4):
                if abs(new_corners[i][1] - new_corners[j][1]) < 4:
                    avg_y = int(round((new_corners[i][1] + new_corners[j][1]) / 2))
                    if new_corners[i][1] != avg_y or new_corners[j][1] != avg_y:
                        new_corners[i][1] = avg_y
                        new_corners[j][1] = avg_y
                        changed = True
    xs = [pt[0] for pt in new_corners]
    ys = [pt[1] for pt in new_corners]
    unique_x = list(set(xs))
    unique_y = list(set(ys))
    if len(unique_x) == 2 and len(unique_y) == 2:
        x_max, x_min = max(unique_x), min(unique_x)
        y_max, y_min = max(unique_y), min(unique_y)
        # 右上、右下、左下、左上
        corners_sorted = [None] * 4
        for pt in new_corners:
            x, y = pt
            if x == x_max and y == y_min:
                corners_sorted[0] = pt  # 右上
            elif x == x_max and y == y_max:
                corners_sorted[1] = pt  # 右下
            elif x == x_min and y == y_max:
                corners_sorted[2] = pt  # 左下
            elif x == x_min and y == y_min:
                corners_sorted[3] = pt  # 左上
        if None in corners_sorted:
            corners_sorted = new_corners.copy()
    else:
        # 原有逻辑
        corners_sorted = sorted(corners, key=lambda c: c[1])
        if corners_sorted[1][0] <= corners_sorted[2][0]:
            corners_sorted[1], corners_sorted[2] = corners_sorted[2], corners_sorted[1]
        corners_sorted[2], corners_sorted[3] = corners_sorted[3], corners_sorted[2]

    # 计算中心点
    cx = sum([pt[0] for pt in corners_sorted]) / 4
    cy = sum([pt[1] for pt in corners_sorted]) / 4

    # 分别按x和y方向缩小
    shrinked = []
    for x, y in corners_sorted:
        new_x = int(cx + (x - cx) * shrink_ratio_x)
        new_y = int(cy + (y - cy) * shrink_ratio_y)
        shrinked.append([new_x, new_y])

    # 画小矩形
    for i in range(4):
        x1, y1 = shrinked[i]
        x2, y2 = shrinked[(i+1)%4]
        img.draw_line(x1, y1, x2, y2, color=(255, 255, 255))  # 白线
        img.draw_circle(x1, y1, 3, color=(0, 255, 0))         # 绿点

    return shrinked  # 返回缩小后的四个点

# 高八位低八位数据处理函数
def data_process(corners):
    bufs = []
    for x, y in corners:
        x = max(0, min(65535, int(x)))
        y = max(0, min(65535, int(y)))
        x_high = (x >> 8) & 0xFF
        x_low = x & 0xFF
        y_high = (y >> 8) & 0xFF
        y_low = y & 0xFF
        buf = bytes([x_high, x_low, y_high, y_low])
        bufs.append(buf)
    return bufs


def process_and_pack_corners(corners, zuobiao):
    def process_single_corners(corners, label):
        # 多轮合并x
        new_corners = [list(pt) for pt in corners]
        changed = True
        while changed:
            changed = False
            for i in range(4):
                for j in range(i+1, 4):
                    if abs(new_corners[i][0] - new_corners[j][0]) < 4:
                        avg_x = int(round((new_corners[i][0] + new_corners[j][0]) / 2))
                        if new_corners[i][0] != avg_x or new_corners[j][0] != avg_x:
                            new_corners[i][0] = avg_x
                            new_corners[j][0] = avg_x
                            changed = True
        # 多轮合并y
        changed = True
        while changed:
            changed = False
            for i in range(4):
                for j in range(i+1, 4):
                    if abs(new_corners[i][1] - new_corners[j][1]) < 4:
                        avg_y = int(round((new_corners[i][1] + new_corners[j][1]) / 2))
                        if new_corners[i][1] != avg_y or new_corners[j][1] != avg_y:
                            new_corners[i][1] = avg_y
                            new_corners[j][1] = avg_y
                            changed = True

        xs_new = [pt[0] for pt in new_corners]
        ys_new = [pt[1] for pt in new_corners]
        unique_x = list(set(xs_new))
        unique_y = list(set(ys_new))
        if len(unique_x) == 2 and len(unique_y) == 2:
            x_max, x_min = max(unique_x), min(unique_x)
            y_max, y_min = max(unique_y), min(unique_y)
            # 右上、右下、左下、左上
            sorted_corners = [None] * 4
            for pt in new_corners:
                x, y = pt
                if x == x_max and y == y_min:
                    sorted_corners[0] = pt  # 右上
                elif x == x_max and y == y_max:
                    sorted_corners[1] = pt  # 右下
                elif x == x_min and y == y_max:
                    sorted_corners[2] = pt  # 左下
                elif x == x_min and y == y_min:
                    sorted_corners[3] = pt  # 左上
            if None in sorted_corners:
                sorted_corners = new_corners.copy()
        else:
            # 第一种情况：原有逻辑
            sorted_corners = sorted(new_corners, key=lambda c: c[1])
            if sorted_corners[1][0] < sorted_corners[2][0]:
                sorted_corners[1], sorted_corners[2] = sorted_corners[2], sorted_corners[1]
            sorted_corners[2], sorted_corners[3] = sorted_corners[3], sorted_corners[2]
        print("输入:", corners)
        print("合并后:", new_corners)
        print("unique_x:", unique_x, "unique_y:", unique_y)
        print("排序后:", sorted_corners)
        # 打包
        bufs = []
        for x, y in sorted_corners:
            x = max(0, min(65535, int(x)))
            y = max(0, min(65535, int(y)))
            x_high = (x >> 8) & 0xFF
            x_low = x & 0xFF
            y_high = (y >> 8) & 0xFF
            y_low = y & 0xFF
            buf = bytes([x_high, x_low, y_high, y_low])
            bufs.append(buf)
        return bufs
    # 处理两个角点数组
    bufs1 = process_single_corners(corners, "corners")
    bufs2 = data_process(zuobiao)
    # 合并所有协议包为一个大bytes
    all_bufs = b''.join(bufs1 + bufs2)
    # 加包头0xB3和包尾0xC3
    packet = bytes([0xB3]) + all_bufs + bytes([0xC3])
    uart.write(packet)  # 通过串口发送数据
    return packet  # 返回包


# 选择要检测的颜色索引 (0:红, 1:绿, 2:蓝) / Select color index to detect
color_index = 0  # 可以修改这个值来选择检测不同的颜色
threshold = THRESHOLDS[color_index]
detect_color = get_closest_rgb(threshold)


while True:

    clock.tick()
    os.exitpoint()
    img = sensor.snapshot()
    img = img.gaussian(1)

    # 按下主按键切换模式
    if key.is_pressed():
        time.sleep(0.1)
        while key.is_pressed():
            time.sleep(0.1)
        mode = mode + 1
    if mode >= 6:
        mode = 1  # 模式循环

    # 模式1：色块检测与坐标发送
    if mode == 1:
        blobs = img.find_blobs([threshold], area_threshold=1, merge=True)  # 识别颜色面积
        if blobs:
            process_blobs(img, blobs, detect_color)  # 画框和十字
            # 检测辅助按键，发送色块坐标
            if key1.is_pressed():
                time.sleep(0.1)
                while key1.is_pressed():
                    time.sleep(0.1)
                    send_xy(blobs[0][0], blobs[0][1])  # 发送第一个色块的坐标（高低字节）
                    num = 1
                    #print(num)
        # 串口接收数据
        data = uart.read()
        #print(data)
        flag_find_first_rect = False
        flag_find_second_rect = False
        draw_message(img, mode)  # 显示当前模式
        img.draw_rectangle(215, 85, 217, 290, color = (255, 255, 255), thickness=1)
        Display.show_image(img)  # 显示图像

    elif mode == 2:
        blobs = img.find_blobs([threshold], area_threshold=1, merge=True)  # 识别颜色面积
        if blobs:
            process_blobs(img, blobs, detect_color)  # 画框和十字
            send_xy(blobs[0][0], blobs[0][1])  # 发送第一个色块的坐标（高低字节）
            # 串口接收数据
        data = uart.read()
        #print(data)
        draw_message(img, mode)  # 显示当前模式
        img.draw_rectangle(215, 85, 217, 290, color = (255, 255, 255), thickness=1)
        Display.show_image(img)  # 显示图像

    elif mode == 3:
        # 找外框
        if flag_find_second_rect == False:
            for rect in img.find_rects(threshold = 20000, x_gradient=10, y_gradient=10):
                if rect:
                    area = rect.magnitude()
                    if area > 80000:
                        flag_find_second_rect = True
                        # 获取矩形的四个角的坐标
                        second_rect_corners = rect.corners()
                    print(area)

        if flag_find_second_rect == True:
            if found_rect == 0:
                found_rect = 1
                print("first:", first_rect_corners)
                print("second:", second_rect_corners)
            if show_second_rect:
                # 绘制外框矩形的四条边
                img.draw_line(second_rect_corners[0][0], second_rect_corners[0][1], second_rect_corners[1][0], second_rect_corners[1][1], color=(255, 255, 255))
                img.draw_line(second_rect_corners[1][0], second_rect_corners[1][1], second_rect_corners[2][0], second_rect_corners[2][1], color=(255, 255, 255))
                img.draw_line(second_rect_corners[2][0], second_rect_corners[2][1], second_rect_corners[3][0], second_rect_corners[3][1], color=(255, 255, 255))
                img.draw_line(second_rect_corners[3][0], second_rect_corners[3][1], second_rect_corners[0][0], second_rect_corners[0][1], color=(255, 255, 255))
                # 圈出外框顶点
                for p in second_rect_corners:
                    img.draw_circle(p[0], p[1], 3, color = (255, 0, 0))
                first_rect_corners = draw_first_rect(img, second_rect_corners, shrink_ratio_x=0.86, shrink_ratio_y=0.87)
        # 检测辅助按键，发送色块坐标
        if key1.is_pressed():
            time.sleep(0.1)
            while key1.is_pressed():
                time.sleep(0.1)
            process_and_pack_corners(second_rect_corners, first_rect_corners)
        Display.show_image(img)
        time.sleep_ms(1)

