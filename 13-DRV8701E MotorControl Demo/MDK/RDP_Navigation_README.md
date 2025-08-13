# 基于RDP算法的智能路径记忆系统

## 系统概述

这个新的路径记忆系统完全重构了原有的记忆逻辑，采用了更鲁棒和智能的方法：

1. **固定距离采样**：每隔20cm记录一个点，确保记录的密度合适
2. **RDP算法简化**：使用Ramer-Douglas-Peucker算法自动简化路径，去除冗余点
3. **智能路径分类**：基于几何特征自动识别直道、转弯和环岛
4. **前瞻速度控制**：在长直道上智能加速，接近转弯时提前减速

## 主要特点

### 1. 内存优化
- 使用300个点的限制，适应单片机RAM限制
- 先在raw_points中采样，后使用RDP算法压缩到path_points
- 对于35m赛道，每20cm采样约175个点，RDP简化后通常只需50-100个关键点

### 2. 鲁棒性改进
- 基于距离的固定采样，不受传感器噪声影响
- RDP算法自动保留关键转折点，忽略微小抖动
- 智能的一圈检测逻辑，基于位置和航向角双重确认

### 3. 长直道优化
- 自动识别长直道（距离>80cm*distance_ratio，角度变化<8°）
- 在长直道上以高速行驶（speed_high = 600）
- 接近转弯时根据转弯角度智能减速

## 关键算法

### RDP简化算法
```c
// 递归地找到偏离直线最远的点，如果超过阈值则保留
void rdp_simplify(PathPoint *points, uint16 start, uint16 end, uint8 *keep)
```

### 路径类型识别
```c
// 根据段长度和角度变化自动分类路径类型
uint8 analyze_path_type(PathPoint *start_point, PathPoint *end_point, float segment_length)
```

### 前瞻速度控制
```c
// 在fast_tracking中检查前方30cm是否有转弯，提前减速
approaching_turn = 1; // 设置接近转弯标志
```

## 配置参数

```c
#define SAMPLE_DISTANCE (20 * distance_ratio)     // 20cm采样间隔
#define RDP_EPSILON (5 * distance_ratio)          // RDP算法阈值，5cm
#define STRAIGHT_ANGLE_THRESHOLD 8.0f             // 直道角度阈值
#define TURN_ANGLE_THRESHOLD 25.0f                // 转弯角度阈值
#define ROUNDABOUT_ANGLE_THRESHOLD 90.0f          // 环岛累计角度阈值
```

## 使用流程

### 第一圈（记录）
1. 调用`refresh()`初始化系统
2. 在主循环中调用`Path_record()`
3. 系统每20cm自动采样一个点到raw_points
4. 检测到完成一圈后，自动调用RDP简化
5. `flag_end`置1，表示记录完成

### 第二圈（回放）
1. 检查`flag_end == 1`后调用`fast_tracking()`
2. 系统根据当前里程自动找到对应路径点
3. 使用前瞻机制检测接近的转弯
4. 智能设置速度：长直道高速，转弯减速

## 速度策略

- **长直道**：speed_high (600) - 无转弯时的最高速度
- **短直道**：speed_low (450) - 安全的中等速度
- **转弯**：根据角度选择speed_low (450)或speed_90 (350)
- **环岛**：speed_90 (350) - 环岛专用速度

## 优势

1. **内存效率高**：RDP算法大幅减少存储点数
2. **抗干扰能力强**：固定距离采样，不受传感器抖动影响
3. **速度优化**：长直道自动加速，显著提高成绩
4. **维护简单**：参数少，逻辑清晰，易于调试

## 调试建议

1. 首先测试采样功能，确保raw_point_count正常增长
2. 检查一圈检测逻辑，确保能正确触发RDP处理
3. 验证RDP简化效果，检查path_point_count是否合理（通常为原始点数的1/3-1/2）
4. 测试速度选择逻辑，确保长直道能正确加速

## 注意事项

1. distance_ratio需要根据实际编码器标定
2. 速度参数需要根据车辆性能调整
3. 如果赛道特别复杂，可以适当减小RDP_EPSILON阈值
4. 建议在实际测试中逐步调整各项阈值参数
