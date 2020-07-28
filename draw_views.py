#!/usr/bin/python
import os
import numpy as np
import cv2 as cv

img_x=1500
img_y=800
white=100
view_size=0
line_width=20

def init_view_size(data):
    global view_size
    for line in data:
        size = int(line[2], 16)
        if size > view_size:
            view_size = size
    print("view_size=", view_size)

def transform_line_size(s):
    return int(s/(view_size/(img_x-white)))

def create_img(data):
    x=(len(data)+3)*line_width #img_y
    y=img_x
    z=3
    img = np.zeros((x, y, z), np.uint8)
    print(img.shape)
    return img

def draw_text(img, text, x, y):
    org = (x, y)
    fontFace = cv.FONT_HERSHEY_COMPLEX
    fontScale = 0.5
    fontcolor = (0, 0, 255) # BGR
    thickness = 1 
    lineType = 4
    bottomLeftOrigin = 1
    # cv.putText(img, text, org, fontFace, fontScale, fontcolor, thickness, lineType, bottomLeftOrigin)
    cv.putText(img, text, org, fontFace, fontScale, fontcolor, thickness, lineType)

def draw_line(img, ptStart, ptEnd, point_color = (0, 255, 0)):
    thickness = 1 
    lineType = 4
    cv.line(img, ptStart, ptEnd, point_color, thickness, lineType)

def draw_view(data):
    print('xx')
    
def show_img(img):
    cv.namedWindow("file view")
    cv.imshow('file view', img)
    cv.waitKey (10000) # 显示 10000 ms 即 10s 后消失
    cv.destroyAllWindows()

def getData():
    lines = []
    result = os.popen('./myreadelf -f libnative-lib.so -V')  
    res = result.read()  
    for line in res.splitlines():  
        l = line.split(' ')
        lines.append(l)
    return lines

lines = getData()
init_view_size(lines)
img = create_img(lines)
i=line_width

for line in lines:
    print(line[0], line[1], line[2])
    lx=transform_line_size(int(line[1], 16))
    ly=transform_line_size(int(line[2], 16))
    draw_line(img, (lx,i), (ly,i))
    draw_text(img, line[0], ly+10, i)
    i=i+line_width

#与文件对齐线
line=lines[0]
lx=transform_line_size(int(line[1], 16))
ly=transform_line_size(int(line[2], 16))
draw_line(img, (ly,0), (ly, i), point_color = (255, 0, 0))
draw_text(img, 'file align', ly, i+line_width)

#show_img(img)
cv.imwrite('file_view.png', img)
