import time, os, gc
import json
import image
from media.sensor import Sensor, CAM_CHN_ID_0
from media.display import *
from media.media import MediaManager
from machine import TOUCH
from ybUtils.YbKey import YbKey
from libs.otherKey import YbKey1, YbKey2, YbKey3
from ybUtils.YbUart import YbUart
#加入自己写的函数
from change_threshold import run_threshold_ui
from get_rect_ui import run_get_rect
from find_color import find_color_app
#分辨率
WIDTH, HEIGHT = 640, 480
#按键
key = YbKey()
key_main = YbKey2()
key_aux = YbKey1()
key_esc = YbKey3()
#串口
uart = YbUart(baudrate=115200)
#初始化
sensor = Sensor()
sensor.reset()
sensor.set_framesize(chn=CAM_CHN_ID_0, width=WIDTH, height=HEIGHT)
sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)
Display.init(Display.ST7701, width=WIDTH, height=HEIGHT, to_ide=True)
MediaManager.init()
sensor.run()
#触屏初始化
tp  = TOUCH(0)

# 默认阈值设为0
DEFAULT_THRESHOLDS = [
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0]
]

# 从SD卡读取阈值
def load_thresholds_from_sd():
    try:
        with open('/sdcard/thresholds.json', 'r') as f:
            thresholds = json.load(f)
        # 验证数据完整性
        if (isinstance(thresholds, list) and len(thresholds) == 2 and
            all(isinstance(group, list) and len(group) == 6 for group in thresholds)):
            print("从SD卡加载阈值:", thresholds)
            return thresholds
        else:
            print("阈值文件格式不正确，使用默认阈值")
            return DEFAULT_THRESHOLDS.copy()
    except OSError:
        print("未找到阈值文件，使用默认阈值")
        return DEFAULT_THRESHOLDS.copy()
    except Exception as e:
        print(f"读取阈值文件出错: {e}，使用默认阈值")
        return DEFAULT_THRESHOLDS.copy()

#自定义阈值
MY_THRESHOLDS = load_thresholds_from_sd()

def draw_touch_btn(img, tp,
                   x, y, w, h,
                   text, text_color=(0, 0, 0),
                   bg_color=(0, 255, 255), thickness=5,
                   font_size=25,
                   text_dx=0, text_dy=0):

    img.draw_rectangle(int(x), int(y), int(w), int(h),
                       color=bg_color, thickness=int(thickness))
    img.draw_string_advanced(
        int(x + (w - len(text) * font_size * 0.6) // 2 + text_dx),
        int(y + (h - font_size) // 2 + text_dy),
        int(font_size),
        text,
        color=text_color
    )

    points = tp.read(1)
    touch = points[0] if points else None
    if touch and x <= touch.x <= x + w and y <= touch.y <= y + h:
        return True
        #返回 True 表示按钮被按下
    return False

#主界面
while True:

    os.exitpoint()
    img = sensor.snapshot(chn=CAM_CHN_ID_0)
    img.clear()
    img.draw_rectangle(0, 0, WIDTH, HEIGHT, color=(255,255,255), fill=True)

    #自定义触屏按键，自己加
    if draw_touch_btn(img, tp,
                      x=10, y=10,
                      w=200, h=200,
                      text="LAB设置",
                      font_size=25):
        print("进入阈值调节")
        # 每次进入阈值调节时重新加载阈值，确保获取最新的值
        current_thresholds = load_thresholds_from_sd()
        run_threshold_ui(sensor, tp, key, key_esc, current_thresholds, WIDTH, HEIGHT)
        # 更新全局阈值变量
        MY_THRESHOLDS[:] = current_thresholds
    if draw_touch_btn(img, tp,
                      x=230, y=10,
                      w=200, h=200,
                      text="模式1",
                      font_size=25):
        print("进入模式1")
        run_get_rect(sensor, key_main, key_aux, key_esc, uart, WIDTH, HEIGHT)
    if draw_touch_btn(img, tp,
                      x=450, y=10,
                      w=190, h=200,
                      text="模式2",
                      font_size=25):
        print("进入模式2")
        find_color_app(sensor, key_main, key_aux, key_esc, MY_THRESHOLDS, uart)

    Display.show_image(img)
    gc.collect()
    time.sleep_ms(50)
