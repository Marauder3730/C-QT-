# 公司员工管理系统

> 西安理工大学 · 计算机科学与技术 · C++ 面向对象课程设计  
> Qt Creator 可视化界面 · 选题：公司员工管理系统

基于 **C++ / Qt 5.12** 实现三类员工（工程师 / 领导 / 主任工程师）的增删改查，含 JSON 持久化、组织关系、证件照与统计图表，体现 **封装、继承、多态**。

**演示账号**：`WangYixian` / `123456`

[English below](#english)

---

## 功能概览

| 功能 | 说明 |
|------|------|
| 员工 CRUD | 新增、编辑、删除、查询三类员工 |
| 多态薪资 | 不同岗位自动使用不同计算公式 |
| 组织关系 | 工程师选择直属领导，下属人数自动统计 |
| 证件照 | Base64 存入 JSON，列表与详情预览 |
| 数据持久化 | 自动读写 `employee.json`，支持导入导出 |
| 统计图表 | 岗位人数饼图、平均薪资柱状图（Qt Charts） |
| 登录 | 演示账号见下方 |

**演示登录**：用户名 `WangYixian`，密码 `123456`

---

## 界面截图

### 登录

![登录界面](docs/screenshots/01-login.png)

### 主界面

![主界面](docs/screenshots/02-main.png)

### 新增/编辑员工

![新增员工](docs/screenshots/03-add-employee.png)

### 统计图表

![统计](docs/screenshots/04-statistics.png)

---

## 项目结构

```
TheFirstTestForEMS/
├── main.cpp                 # 程序入口（登录 → 主窗口）
├── employee.h / .cpp        # 抽象基类
├── engineer.*               # 工程师
├── leader.*                 # 领导
├── chiefengineer.*          # 主任工程师
├── employeemanager.*        # 业务层：CRUD、JSON、统计
├── mainwindow.*             # 主界面
├── addeditdialog.*          # 新增/编辑对话框
├── logindialog.*            # 登录
├── statdialog.*             # 统计图表
├── uistyle.*                # 界面样式
├── employee.json            # 示例数据
├── resources/               # 图标与背景
└── docs/
    ├── REFLECTIONS.md       # 开发心得
    └── screenshots/         # 截图目录
```

---

## 环境要求

- Qt **5.12.9** 或兼容的 Qt 5.12.x（需模块：widgets、charts、svg）
- Qt Creator 或 qmake + 对应编译器
- C++11

---

## 如何运行

1. 用 Qt Creator 打开 `TheFirstTestForEMS.pro`
2. 选择 Kit（如 Desktop Qt 5.12.9 MinGW）
3. 点击运行（Ctrl+R）
4. 程序会在构建目录复制 `employee.json` 作为初始数据

---

## OOP 设计要点

- **封装**：员工属性私有，通过 getter/setter 访问
- **继承**：三类员工继承 `Employee` 基类
- **多态**：`calcSalary()`、`getType()`、`getDetail()` 为虚函数；`EmployeeManager` 用 `Employee*` 统一管理

详见 [docs/REFLECTIONS.md](docs/REFLECTIONS.md)

---

## 薪资规则

| 岗位 | 公式 |
|------|------|
| 工程师 | 3000 + 项目数 × 500 |
| 领导 | 5000 + 2000 + 下属数 × 100 |
| 主任工程师 | 4000 + 负责项目数 × 800 + 技术津贴 |

---

## 相关文档

- [开发心得体会](docs/REFLECTIONS.md)
- 课程设计报告：`OOP课程设计报告_公司员工管理系统.html`（可用浏览器打开，或导入 Word）
- 答辩要点：`答辩要点_公司员工管理系统.html`

---

## License

MIT License — 见 [LICENSE](LICENSE)

---

## English

A Qt/C++ employee management system for an OOP course project. Supports three employee types with polymorphic salary calculation, JSON persistence, leader–engineer relationships, photo storage, and Qt Charts statistics.

**Demo login**: `WangYixian` / `123456`

**Build**: Open `TheFirstTestForEMS.pro` in Qt Creator 5.12+, run.

---

*Author: WangYixian*
