# LiteEMF

EN | [中文](./README.md)

LiteEMF lite embedded middle framework is an embedded general framework, which integrates modules and functions common to embedded devices based on a HAL layer general hardware API.

through LiteEMF can be quickly ported to any chiand system, helping developers quickly achieve product development

---

[toc]

---

## 快速开始

欢迎使用LietEMF开源API项目, 在开始进入项目之前请详细阅读项目介绍文档

## 编译

LiteEMF 是一个API接口框架,需要移植到你的使用项目中编译

移植步骤:

* 下载LiteEMF到你的项目中,推荐使用 `gitmodule`方式
* 将LiteEMF原文件包含到你的项目中
* 编写适配HAL层硬件驱动

*没有使用到的驱动可以不适配*

* 适配hw_board.h,hw_config.h配置文件
* 添加工程,修改功能和编译

## 平台支持

* 支持裸机系统
* 支持操作系统

## 免责声明

LietEMF可以作为开发，评估，甚至量产使用
