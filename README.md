# InkSight（Waveshare 4.2 精简固件说明版）

## 官方网址
**www.inksight.site**

## 源码来源
本仓库基于原项目修改而来：
- 原项目：<https://github.com/datascale-ai/inksight>
- 当前适配仓库：<https://github.com/xiaosheng123/inksight-WF>

本 README 主要说明当前仓库中 **Waveshare 4.2 寸屏幕精简固件** 的使用方式与现状。

## 目标屏幕型号（务必确认）

当前适配工作默认针对这块墨水屏：

- **WFT0420CZ15LW** —— 微雪 4.2" 三色 e-Paper，分辨率 400×300  
  - 官方资料通常以名称 **“4.2inch e-Paper (B) V2”** 出现在 PDF 中：  
    `4.2inch_e-Paper_(B)_V2.pdf`  
  - 本仓库后续提到的 “4.2 屏 / Waveshare 4.2 三色” 如无特别说明，均指 **WFT0420CZ15LW 这一 WF 开头型号**。

如果你手上的屏不是这一型号（例如 4in2G、4.2" 纯黑白等），请谨慎直接使用本仓库固件。

## 当前结论
当前这份适配工作的主要结论如下：

- 已切换到 **Waveshare 4.2 驱动后端**
- 默认构建已去掉大部分旧的 **GxEPD2 / U8g2** 依赖
- 目前仅维护 **ESP32-C3 + WFT0420CZ15LW** 这一条 4.2 寸固件线
- 固件体积已明显瘦身，适合先做真机联调
- 当前重点是先保证 **黑白稳定显示**
- 三色显示与 WROOM32E 支持仍在联调中，暂未提供可用 Release 固件

## 固件发布目录
仓库内已提交的固件位于：

- `firmware/releases/`

### ESP32-C3（WFT0420CZ15LW 4.2"）

新命名（推荐使用）：

- `firmware/releases/WFT0420CZ15LW_esp32c3_inksight.bin`
- `firmware/releases/WFT0420CZ15LW_esp32c3_inksight_merged.bin`

旧命名（兼容保留）：

- `firmware/releases/waveshare_c3_4in2.bin`
- `firmware/releases/waveshare_c3_4in2_merged.bin`

> 说明：原先的 WROOM32E 对应固件（包括 `waveshare_wroom32e_4in2*.bin` 和 `WFT0420CZ15LW_wroom32e_tb_board*.bin`）
> 因实际测试存在“无法正常开机”的问题，已从仓库移除，待后续调试稳定后再重新发布。

## 推荐烧录文件
推荐优先使用带 `*_merged.bin` 后缀的固件：

- `WFT0420CZ15LW_esp32c3_inksight_merged.bin`

> **For detailed purchasing schemes and part links**, please refer to the [**Hardware Purchasing Guide**](docs/en/bom.md) (Note: links are mostly for Taobao, but equivalent parts can be found on AliExpress/Amazon).

The public documentation and setup flow are centered on **ESP32-C3 + 4.2-inch e-paper**.

- 已合并 bootloader / partitions / app
- 直接从 `0x0` 烧录即可
- 不容易因为偏移地址写错而翻车

## 平台说明

### ESP32-C3
适用于当前已验证的 C3 接线方案，对应 WFT0420CZ15LW 屏幕。

已知使用过的 4.2 寸屏幕接线为：

- SCK = GPIO4
- MOSI = GPIO6
- CS = GPIO7
- DC = GPIO1
- RST = GPIO2
- BUSY = GPIO10

### ESP32-WROOM32E（暂不提供固件）

WROOM32E 这条线当前仍在联调中：

- 早期固件在实机上存在“无法正常开机”的问题
- 为避免误导下载和刷写，相关固件已从 `firmware/releases/` 中移除
- 后续若确认稳定，将重新发布，并在 README 中补充说明

## 当前构建状态

### 已完成
- `wft_4in2b`（ESP32-C3, WFT0420CZ15LW）编译通过
- 默认构建已瘦身成功
- 已将 C3 固件提交到 `firmware/releases/`

### 联调中
- C3 三色显示路径（基于 WFT0420CZ15LW 的红色通道）
- ESP32-WROOM32E + WFT0420CZ15LW 固件（启动与引脚映射）

## Community Showcase

We are thrilled to see the amazing cases and custom PCBs created by the InkSight community! Here are some excellent community contributions:

### 3D Printable Cases
- **[Orange Desktop Case (MakerWorld)](https://makerworld.com/zh/@CANLAY)**
  
  <img src="images/community/case1.png" width="400">

- **[Pink/Red Minimalist Cases (MakerWorld)](https://makerworld.com/zh/@CANLAY)**
  
  <img src="images/community/case2.png" width="400">
  <img src="images/community/case3.png" width="400">

- **[Huawei Nova 14 Color Display Phone Case 3.98-inch 4-Color E-ink Screen](https://makerworld.com.cn/zh/models/2399226-gu-jian-kai-yuan-diy-hua-wei-nova14cai-xian-shou-j#profileId-2744953)**

  <img src="images/community/case4.png" width="400">

### Custom PCBs
- **[InkSight 4.2" Custom Driver Board (OSHWHUB)](https://oshwhub.com/kidstory/4-2)**
  
  <img src="images/community/pcb1.png" width="400">

- **[Moyu AI E-ink Screen (Verified)](https://oshwhub.com/qq173972819/project_mqihrlpc)**
  
  <img src="images/community/pcb2.png" width="400">

## Self-Host or Develop

## 编译环境
当前仓库在本地使用过的可用 PlatformIO 隔离环境为：

- `.venv-pio`

例如：

```bash
/root/.openclaw/workspace/.venv-pio/bin/pio run -e wft_4in2b
```

## 烧录建议
如果使用 merged 固件，通常可直接从 `0x0` 烧录。

请根据自己的芯片与串口环境，选择对应烧录方式，例如：

```bash
esptool.py --chip esp32c3 --port <PORT> --baud 460800 \ 
  write_flash 0x0 firmware/releases/WFT0420CZ15LW_esp32c3_inksight_merged.bin
```

## 说明
这份 README 现在重点服务于：

- 固件产物位置说明
- 当前仅维护的 C3 + WFT0420CZ15LW 4.2 寸固件状态
- 后续三色显示与 WROOM32E 固件将另行说明

如果后续把旧日历渲染完整迁回新链路，再继续补充更完整的功能说明。
