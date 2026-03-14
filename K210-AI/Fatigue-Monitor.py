import sensor, image, time, lcd
from maix import KPU
import gc
from machine import UART
from fpioa_manager import fm
import math

# --- 1. 硬件初始化 ---
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_vflip(True)
sensor.skip_frames(time = 500)
clock = time.clock()

# UART 初始化
fm.register(13, fm.fpioa.UART2_TX, force=True)
fm.register(15, fm.fpioa.UART2_RX, force=True)
uart = UART(UART.UART2, 115200, 8, 0, 1, timeout=1000, read_buf_len=4096)

# --- 2. 模型加载 ---
od_img = image.Image(size=(320,256), copy_to_fb=False)
anchor = (0.893, 1.463, 0.245, 0.389, 1.55, 2.58, 0.375, 0.594, 3.099, 5.038, 0.057, 0.090, 0.567, 0.904, 0.101, 0.160, 0.159, 0.255)
kpu = KPU()
kpu.load_kmodel("/sd/face_detect.kmodel")
kpu.init_yolo2(anchor, anchor_num=9, img_w=320, img_h=240, net_w=320 , net_h=256 ,layer_w=10 ,layer_h=8, threshold=0.7, nms_value=0.2, classes=1)

lm68_kpu = KPU()
lm68_kpu.load_kmodel("/sd/landmark68.kmodel")

# --- 3. 参数设置与优化变量 ---
RATIO = 0.08
EAR_THRESHOLD = 0.21       # 闭眼阈值（可根据实际在0.18-0.23之间微调）
CLOSED_TIME_MS = 2000      # 修改为2秒
STABLE_FRAMES = 2          # 容错帧数：允许中间有2帧误判为睁眼而不重置计时

# 状态追踪变量
closed_start_time = 0      # 记录开始闭眼的时间戳
blink_counter = 0          # 连续闭眼帧数计数
lost_frame_counter = 0     # 容错计数器

def calculate_ear(pts):
    def dist(p1, p2):
        return math.sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)
    # EAR = (|p2-p6| + |p3-p5|) / (2 * |p1-p4|)
    d_v1 = dist(pts[1], pts[5])
    d_v2 = dist(pts[2], pts[4])
    d_h = dist(pts[0], pts[3])
    if d_h == 0: return 0
    return (d_v1 + d_v2) / (2.0 * d_h)

while True:
    gc.collect()
    clock.tick()
    img = sensor.snapshot()

    od_img.draw_image(img, 0, 0)
    od_img.pix_to_ai()
    kpu.run_with_output(od_img)
    dect = kpu.regionlayer_yolo2()

    current_eyes_closed = False # 本帧是否检测为闭眼

    if len(dect) > 0:
        l = dect[0] # 只取第一个人脸
        img.draw_rectangle(l[0], l[1], l[2], l[3], color=(0, 255, 0))

        x1 = max(int(l[0] - RATIO*l[2]), 1)
        y1 = max(int(l[1] - RATIO*l[3]), 1)
        x2 = min(int(l[0] + l[2] + RATIO*l[2]), 319)
        y2 = min(int(l[1] + l[3] + RATIO*l[3]), 255)

        face_cut = img.cut(x1, y1, x2-x1+1, y2-y1+1)
        face_cut_128 = face_cut.resize(128, 128)
        face_cut_128.pix_to_ai()

        out = lm68_kpu.run_with_output(face_cut_128, getlist=True)

        if out:
            all_pts = []
            for j in range(68):
                px = int(KPU.sigmoid(out[2 * j]) * (x2 - x1 + 1) + x1)
                py = int(KPU.sigmoid(out[2 * j + 1]) * (y2 - y1 + 1) + y1)
                all_pts.append((px, py))

            # 计算左右眼 EAR
            ear_l = calculate_ear(all_pts[36:42])
            ear_r = calculate_ear(all_pts[42:48])
            avg_ear = (ear_l + ear_r) / 2.0

            # 判定本帧是否闭眼
            if avg_ear < EAR_THRESHOLD:
                current_eyes_closed = True

            # 绘制眼睛关键点（蓝色代表睁开，红色代表闭合）
            eye_color = (255, 0, 0) if current_eyes_closed else (0, 0, 255)
            for p in (all_pts[36:48]):
                img.draw_circle(p[0], p[1], 1, color=eye_color, fill=True)

        del face_cut
        del face_cut_128

    # --- 4. 核心逻辑：带容错的计时器 ---
    current_time = time.ticks_ms()
    send_char = 'N'

    if current_eyes_closed:
        # 如果是本帧闭眼
        lost_frame_counter = 0 # 重置容错计数
        if closed_start_time == 0:
            closed_start_time = current_time # 记录第一次闭眼时间

        duration = time.ticks_diff(current_time, closed_start_time)
        if duration >= CLOSED_TIME_MS:
            send_char = 'F'
            img.draw_string(10, 40, "WARNING: SLEEP!", color=(255, 0, 0), scale=2)
        else:
            img.draw_string(10, 40, "Wait: %dms" % duration, color=(255, 255, 0), scale=1.5)
    else:
        # 如果本帧检测为睁眼，先不急着重置，看看是不是误判
        if closed_start_time != 0:
            lost_frame_counter += 1
            if lost_frame_counter > STABLE_FRAMES:
                # 只有连续多帧都检测为睁眼，才真正重置计时器
                closed_start_time = 0
                lost_frame_counter = 0
            else:
                # 还在容错范围内，继续维持闭眼计时
                duration = time.ticks_diff(current_time, closed_start_time)
                if duration >= CLOSED_TIME_MS:
                    send_char = 'F'
                img.draw_string(10, 40, "Fixing...", color=(0, 0, 255), scale=1.5)
        else:
            img.draw_string(10, 40, "Status: Active", color=(0, 255, 0), scale=1.5)

    # 发送串口数据
    uart.write(send_char)

    # 显示FPS
    img.draw_string(0, 0, "%2.1ffps" %(clock.fps()), color=(0, 60, 255), scale=1.5)
    lcd.display(img)
