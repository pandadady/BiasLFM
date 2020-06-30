# BiasLFM
用于学习SGD的矩阵分解

含有偏置项的LFM

g++ -g -Wall -std=c++11 biaslfm.cpp -o biaslfm


原生LFM

g++ -g -Wall -std=c++11 lfm.cpp -o lfm



此项目的数据是user对item的打分数据，没有无评分项，这样的数据跟推荐系统中不同

在推荐系统中，一般会把时间窗口内的全部用户正负向行为统计出来，点击正向，展示负向。


