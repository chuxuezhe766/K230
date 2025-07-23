import time, os, gc
from media.sensor import Sensor, CAM_CHN_ID_0
from media.display import *
from media.media import *
from machine import TOUCH
from ybUtils.YbKey import YbKey

#由外部传入阈值
def run_threshold_ui(sensor, tp, key, key_esc, thresholds,
                     WIDTH=640, HEIGHT=480):

    original_thresholds = [row[:] for row in thresholds]
    #初始阈值
    thresholds = [
        [0, 100, -128, 127, -128, 127],  # 第一组
        [0, 100, -128, 127, -128, 127]   # 第二组
    ]
    current_group = 0

    value_labels = ["Lmin", "Lmax", "Amin", "Amax", "Bmin", "Bmax"]
    threshold_ranges = [
        (0, 100),
        (0, 100),
        (-128, 127),
        (-128, 127),
        (-128, 127),
        (-128, 127),
    ]

    buttons = []
    BTN_W, BTN_H = 100, 80

    button_definitions = {
        "Lmin+": (0, 1), "Lmin-": (0, -1),
        "Lmax+": (1, 1), "Lmax-": (1, -1),
        "Amin+": (2, 1), "Amin-": (2, -1),
        "Amax+": (3, 1), "Amax-": (3, -1),
        "Bmin+": (4, 1), "Bmin-": (4, -1),
        "Bmax+": (5, 1), "Bmax-": (5, -1),
    }

    left_texts = ["Lmin+", "Lmin-", "Lmax+", "Lmax-", "Amin+", "Amin-"]
    right_texts = ["Amax+", "Amax-", "Bmin+", "Bmin-", "Bmax+", "Bmax-"]

    for i, text in enumerate(left_texts):
        buttons.append({
            'rect': (0, i * BTN_H, BTN_W, BTN_H),
            'text': text,
            'action': button_definitions[text],
            'color': (200, 200, 255),
            'text_color': (255, 0, 0),
            'pressed_color': (0, 255, 0)
        })
    for i, text in enumerate(right_texts):
        buttons.append({
            'rect': (WIDTH - BTN_W, i * BTN_H, BTN_W, BTN_H),
            'text': text,
            'action': button_definitions[text],
            'color': (200, 200, 255),
            'text_color': (255, 0, 0),
            'pressed_color': (0, 255, 0)
        })

    switch_btn = {
        'rect': (430, 10, 100, 80),
        'text': "切换组",
        'action': "switch",
        'color': (200, 200, 255),
        'text_color': (255, 0, 0),
        'pressed_color': (0, 255, 255)
    }
    buttons.append(switch_btn)

    toggle_display_btn = {
        'rect': (430, 100, 100, 80),
        'text': "切换显示",
        'action': "toggle_display",
        'color': (200, 200, 255),
        'text_color': (255, 0, 0),
        'pressed_color': (0, 255, 255)
    }
    buttons.append(toggle_display_btn)

    exit_btn = {
        'rect': (430, 180, 100, 80),
        'text': "退出",
        'action': "exit",
        'color': (200, 200, 255),
        'text_color': (255, 0, 0),
        'pressed_color': (0, 255, 255)
    }
    buttons.append(exit_btn)

    def is_point_in_button(x, y, btn_x, btn_y, btn_w, btn_h):
        return btn_x <= x <= btn_x + btn_w and btn_y <= y <= btn_y + btn_h

    def draw_botton(img, x, y, w, h, color, text="", text_color=(255,0,0), font_size=20, center_text=False):
        img.draw_rectangle(x, y, w, h, color=color, thickness=2)
        if text:
            if center_text:
                text_width = int(len(text) * font_size * 0.6)
                tx = x - 15 + (w - text_width) // 2
            else:
                tx = x + 25 + (w - len(text) * font_size) // 2
            ty = y + (h - font_size) // 2
            img.draw_string_advanced(tx, ty, font_size, text, color=text_color)

    show_binary = False
    last_touch = False
    save_hint_counter = 0

    while True:
        img = sensor.snapshot(chn=CAM_CHN_ID_0)
        # 一键退出
        if key_esc.is_pressed():
            time.sleep(0.05)
            return thresholds

        if show_binary:
            threshold_tuple = tuple(thresholds[current_group])
            img.binary([threshold_tuple])

        if key.is_pressed():
            time.sleep(0.1)
            while key.is_pressed():
                time.sleep(0.1)
            original_thresholds[current_group] = thresholds[current_group][:]
            print(f"已保存当前组{current_group+1}的阈值为原始阈值")
            save_hint_counter = 30

        points = tp.read(1)
        touch_point = points[0] if points else None

        if touch_point and not last_touch:
            for btn in buttons:
                action = btn.get('action')
                x, y, w, h = btn['rect']
                if is_point_in_button(touch_point.x, touch_point.y, x, y, w, h):
                    if action == "switch":
                        current_group = (current_group + 1) % len(thresholds)
                        print(f"切换到第{current_group+1}组")
                        time.sleep_ms(100)
                    elif action == "toggle_display":
                        show_binary = not show_binary
                        print(f"切换显示模式，当前为{'阈值处理' if show_binary else '原始画面'}")
                        time.sleep_ms(100)
                    elif action == "exit":
                        return thresholds
                    break

        if touch_point:
            for btn in buttons:
                if btn.get('action') not in ["switch", "toggle_display", "exit"]:
                    x, y, w, h = btn['rect']
                    color = btn['color']
                    if is_point_in_button(touch_point.x, touch_point.y, x, y, w, h):
                        color = btn['pressed_color']
                        index, change = btn['action']
                        min_val, max_val = threshold_ranges[index]
                        new_val = thresholds[current_group][index] + change
                        new_val = max(min_val, min(max_val, new_val))
                        thresholds[current_group][index] = new_val
                        print(f"阈值更新: {value_labels[index]} = {thresholds[current_group][index]}")
                        time.sleep_ms(100)

        for btn in buttons:
            x, y, w, h = btn['rect']
            color = btn['color']
            if btn.get('action') in ["switch", "toggle_display", "exit"]:
                draw_botton(img, x, y, w, h, color, btn['text'], btn['text_color'], 30, center_text=True)
            else:
                draw_botton(img, x, y, w, h, color, btn['text'], btn['text_color'], 30)

        start_x = 100
        gap_x = 74
        y = HEIGHT - 40
        for i, label in enumerate(value_labels):
            display_text = f"{label}: {thresholds[current_group][i]}"
            img.draw_string_advanced(start_x + i * gap_x, y, 15, display_text, color=(173, 216, 230))
            orig_text = f"{original_thresholds[current_group][i]}"
            img.draw_string_advanced(start_x + i * gap_x, y + 18, 15, orig_text, color=(255, 0, 0))
        img.draw_string_advanced(100, 10, 20, f"当前组: {current_group+1}", color=(0,255,255))

        if save_hint_counter > 0:
            img.draw_string_advanced(WIDTH // 2 - 60, HEIGHT // 2 - 20, 25, "阈值已保存!", color=(255, 0, 0))#by kongcheng
            save_hint_counter -= 1

        Display.show_image(img)
        gc.collect()
        time.sleep_ms(5)

        last_touch = bool(touch_point)
