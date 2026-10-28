# Velvet 工作布局

适用于照片中的全数字行、每侧五个底部按键、未安装轨迹球的 Velvet。
右半是主控，USB Studio 请接右半。硬件矩阵保留上游的 61 个逻辑位置，
其中三个中间鼠标按钮未安装，绑定为无操作；其余 58 个位置对应实体按键。

## 主层及切层

- 左底部从外向内：Ctrl、Win、Alt、EXCEL、Space。
- 右底部从内向外：Enter、Space、SYMBOL、Backspace、Delete。
- 右手最下方字母行：Shift、N、M、逗号、句号、斜杠。
- 按住左 EXCEL 进入数字/导航层；按住右 SYMBOL 进入符号层。
- 同时按住 EXCEL 和 SYMBOL 进入 SYSTEM；松开后返回。
- 备用入口：同时按住 Q+W+E 进入 EXCEL，U+I+O 进入 SYMBOL。
- W+D+X 为 Studio Unlock。当前构建沿用上游免锁设置。

## EXCEL

左 QWERT 位置：Ctrl / Shift / Alt / F2 / Tab。
左 ASDFG 位置：Home / 上 / End / PageUp / Insert。
左 ZXCVB 位置：PageDown / 左 / 下 / 右 / F4。

右字母区：

```text
/   7   8   9   -   +
*   4   5   6   =   Enter
0   1   2   3   .   %
```

数字使用主键区键码，不依赖 Num Lock。Ctrl、Shift 与方向可组合选择单元格。

## SYMBOL 与 AHK v2

沿用当前 KK.ahk，不需要重复添加函数或快捷键。

| 主层物理位置 | 信号 | AHK 输出 |
|---|---|---|
| Q | F13 | 成对花括号 |
| W | F14 | 成对方括号 |
| E | F15 | 成对圆括号 |
| R | F16 | 管道符 |
| T | F17 | 下划线 |
| Y | F18 | 英文分号 |
| U | F19 | 英文冒号 |
| I | F20 | {1} |
| O | F21 | {2} |
| P | F22 | {name} |
| 右侧反斜杠键 | F23 | TEXTJOIN 模板 |
| A | F24 | {0} |

两台电脑需要分别运行 AHK；未运行时仍可使用本层其他普通符号键，
但普通键码不能保证绕过中文输入法标点转换。

## SYSTEM

- 数字 1~5：选择蓝牙槽 0~4。
- Q：USB 优先；W：BLE 优先；E：Studio Unlock。
- R / T：上一 / 下一蓝牙槽。
- 右侧数字行及 YUIOP 行：F1~F12。
- 左 ASDFG：撤销、重做、复制、粘贴、剪切。
- 右 HJKL 区域：分组、取消分组、复制对象、Shift+A。
- 右分号、引号位置：Ctrl+[、Ctrl+]。
- SYSTEM 下同时按 Esc+右底部 Delete：仅清除当前蓝牙槽。
- 不提供一键清空全部配对，避免误触。

## 刷写与生效

1. 先导出当前 DYA 配置作备份。
2. velvet_left.uf2 刷左半，velvet_right.uf2 刷右半。
3. 不要刷 settings_reset，保留配对。
4. 右半 USB 连接 DYA，在键位页使用恢复固件默认键位的操作并保存；
   仅刷固件不会覆盖已保存的运行时键位。不要选择整机/蓝牙恢复出厂。
5. 保存后用 EXCEL+I 测试输出 8；SYMBOL+Q 测试 AHK 的成对花括号。

## 诊断

设备信息、看门狗、按键扫描诊断、栈查询及实时按键流均包含在配置中。
两半均采集本地扫描数据，由右主控转发查询。栈查询默认报告主控线程。
诊断中的可疑标记需要重复测试，不能单凭一次警告认定轴体损坏。
停止实时按键流后再断开 Studio。右侧无轨迹球节点，不启用鼠标自动层。
