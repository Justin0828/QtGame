# QtGame_Justin

This is a Qt game project repository, created by Justin0828.

## 项目简介

这是一个基于Qt6框架开发的游戏项目，使用C++17标准。

## 环境要求

- Qt 6.5.3 或更高版本
- MinGW 11.2.0 编译器
- CMake 3.28 或更高版本
- Windows 10/11

## 构建说明

### 1. 克隆项目
```bash
git clone <repository-url>
cd QtGame
```

### 2. 创建构建目录
```bash
mkdir build
cd build
```

### 3. 配置项目（使用MinGW编译器）
```bash
cmake .. -G "MinGW Makefiles"
```

### 4. 构建项目
```bash
cmake --build .
```

### 5. 运行程序
```bash
.\QtGame_Justin.exe
```

## 完整的构建流程

```bash
# 在项目根目录执行
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
.\QtGame_Justin.exe
```

## 重新构建

如果修改了源代码，只需要重新构建：
```bash
cd build
cmake --build .
```

如果修改了CMakeLists.txt文件，需要重新配置：
```bash
cd build
cmake ..
cmake --build .
```

## 项目结构

```
QtGame/
├── src/                    # 源代码目录
│   ├── main.cpp           # 主程序入口
│   ├── MyGame.cpp         # 游戏主类
│   ├── Items/             # 游戏物品系统
│   │   ├── Characters/    # 角色相关
│   │   ├── Armors/        # 护甲系统
│   │   ├── HeadEquipments/ # 头部装备
│   │   ├── LegEquipments/  # 腿部装备
│   │   └── Maps/          # 地图系统
│   └── Scenes/            # 场景系统
├── assets/                # 资源文件
│   ├── assets.qrc         # Qt资源文件
│   └── Items/             # 游戏物品资源
├── build/                 # 构建输出目录
├── CMakeLists.txt         # CMake配置文件
└── README.md              # 项目说明文档
```

## 注意事项

- 确保Qt环境变量已正确配置
- 使用MinGW编译器而不是MSVC，以避免C++17兼容性问题
- 项目使用Qt6的Core、Gui、Widgets模块
