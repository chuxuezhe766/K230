////////////////////////////////////////////////////////////////////////////////////////////////////////
本项目用亚博的K230做的，因为不同的K230之间固件可能不一样，所以代码不一定能正常使用。
由于时间比较匆忙，所以代码中有部分内容可能没用，但没用删除，请见谅。
///////////////////////////////////////////////////////////////////////////////////////////////////////
下面大致讲一下我的代码思路：
代码有两份，get_rect.py这个代码是红色激光笔的K230代码，get_light3.py这个代码是绿色激光笔的K230代码。
1，首先先用红色激光定位，铅笔线框的四个点，此时K230屏幕上显示mode1，这是识别红光的模式。当红光对准点时，按下按键向stm32发送点坐标，其他三个点同理，
电控通过坐标计算出中心点。此时再按另一个按键进入mode2，mode2是一直发送模式，会一直发送红光坐标，此时就可以让红光回原点。
2，绕铅笔线框走和绕正黑框走这两部分没有用视觉。
3，识别斜矩形，刚开始想识别两个矩形框（内矩形和外矩形），后面发现识别很多情况下会只识别到外矩形框，所以决定不如就识别外矩形框，然后内矩形框就通过比例缩
放，这种方法识别矩形速度非常快，也很准确，计算出来，然后将八个坐标点发送给电控处理。
4，追踪部分用get_light3.py这个代码，K230通过一直识别红光和绿光的位置，然后把坐标发送给电控，电控进行处理追踪。识别红光和绿光用了两个阈值，加强了识别的精
确度。首先识别亮光区域，把识别的阈值调到红光和绿光都能识别到的情况，然后把亮光区域作为ROI区域进行识别，识别红光和绿光，将两个点的坐标发送电控，进行处理。


补充：
1.这题用了两个自己定义的按键，当时打算用K230上面自带的按键，发现当按键按的时候屏幕抖动严重，影响识别，所以将按键引出了不放K230上，OthKey.py是自定义按
键的代码。
2.红光的K230有三种模式，按其中一个按键就可以切换模式，开始时为模式1，最多可以到模式5。模式1和模式2是控制红光回原点功能，模式三是识别斜矩形的功能。
3.看到这里希望大家给我的代码库点点Star,代码有写的不好的地方请大家见谅，也欢迎大家给出更好的方案，谢谢大家。



																										by Kongcheng

