import time, os, sys, image
from media.sensor import Sensor, CAM_CHN_ID_0
from media.display import Display
from media.media import MediaManager
from libs.YbProtocol import YbProtocol
from ybUtils.YbUart import YbUart
from libs.otherKey import YbKey1, YbKey2

#功能函数
def get_closest_rgb(lab_threshold):
    """根据LAB阈值计算最接近的RGB颜色 / Calculate closest RGB color based on LAB threshold"""
    # 获取LAB空间的中心点值
    l_center = (lab_threshold[0] + lab_threshold[1]) // 2
    a_center = (lab_threshold[2] + lab_threshold[3]) // 2
    b_center = (lab_threshold[4] + lab_threshold[5]) // 2
    return image.lab_to_rgb((l_center,a_center,b_center))

def process_blobs(img, blobs, color, pto):
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

# 主函数
def find_color_app(sensor, key_main, key_aux, key_esc,
                   thresholds, WIDTH=640, HEIGHT=480):
    if not thresholds:
        raise ValueError("thresholds 不能为空")
    #检测的阈值：自己命名
    threshold = thresholds[0]  # 明确选择第一个阈值
    threshold1 = thresholds[1]
    detect_color = get_closest_rgb(threshold)
    detect_color1 = get_closest_rgb(threshold1)
    pto = YbProtocol()
    mode = 1  #初始化 mode，否则会报错

    while True:
        os.exitpoint()
        img = sensor.snapshot()
        img = img.gaussian(1)

        # 一键退出
        if key_esc.is_pressed():
            time.sleep(0.05)
            return thresholds

        # 按下主按键切换模式
        if key_main.is_pressed():
            time.sleep(0.1)
            while key_main.is_pressed():
                time.sleep(0.1)
            mode = mode + 1
        if mode >= 6:
            mode = 1  # 模式循环

        # 模式1：色块检测与坐标发送
        if mode == 1:
            blobs = img.find_blobs([threshold], area_threshold=100, merge=True)
            if blobs:
                process_blobs(img, blobs, detect_color, pto)  # 画框和十字
        draw_message(img, mode)
        Display.show_image(img)  # 显示图像
        #模式2：色块检测与坐标发送
        if mode == 2:
            blobs = img.find_blobs([threshold1], area_threshold=100, merge=True)
            if blobs:
                process_blobs(img, blobs, detect_color1, pto)  # 画框和十字
        draw_message(img, mode)
        Display.show_image(img)  # 显示图像


