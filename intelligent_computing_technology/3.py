import math
import numpy as np
import matplotlib.pyplot as plt

# 随机生成图信息（节点数100）
nCity = 100
City = np.random.uniform(-10, 10, [nCity, 2])
Dis = {}
for i in range(nCity):
    for j in range(nCity):
        if i > j:
            dis = ((City[i][0] - City[j][0]) ** 2 + (City[i][1] - City[j][1]) ** 2) ** 0.5
            Dis[(i, j)] = dis
            Dis[(j, i)] = dis

Data_BestFit =[]   #用于保存每一代蚂蚁的最优适应度

#适应度计算函数 适应值= 节点数量 / 路径距离
def Cal_Fit(X):
    total_dis = Dis[(X[-1], X[0])]
    for i in range(nCity - 1):
        total_dis += Dis[(X[i], X[i + 1])]
    return nCity / total_dis

def ACA_TSP():
    nPop = 100        # 种群大小
    Maxit = 20       # 最大迭代次数
    Rou = 1.0         # 蒸发系数
    Rou_damp = 0.95   # 蒸发系数衰减度
    Rou_min = 0.1     # 最小蒸发系数
    alpha = 1         # 信息素重要程度
    beta = 0.2        # 启发式信息重要程度
    epsilon = 1e-5    # 初始信息素浓度
    Phe = {}          # 保存信息素的字典
    for key in Dis.keys():
        Phe[key] = epsilon

    Fit = [0.0 for i in range(nPop)]
    Best_Ant = None
    Best_Fit = -math.inf

    # 迭代求解
    for j in range(Maxit):
        Ant = [[] for i in range(nPop)]
        #蚂蚁寻路
        for i in range(nPop):
            # 以第一个城市为起点，依次从剩下的城市中按照概率挑选目标
            # Open保存已被选取的城市
            # Close保存未被选取的城市
            Open = [0]
            Close = [i for i in range(1,nCity)]
            while Close:
                if len(Close) == 1:
                    Open.append(Close.pop(0))
                else:
                    P = np.zeros([len(Close)])
                    for k in range(len(Close)):
                        P[k] = Phe[(Open[-1], Close[k])] ** alpha + Dis[(Open[-1], Close[k])] ** beta
                    P = P / sum(P)
                    next_index = np.random.choice(range(len(Close)), size=1, p=P)[0]
                    Open.append(Close.pop(next_index))
            Ant[i] = Open.copy()

        #计算每只蚂蚁的路径适应值
        for i in range(nPop):
            Fit[i] = Cal_Fit(Ant[i])
            if Fit[i] > Best_Fit:
                Best_Fit = Fit[i]
                Best_Ant = Ant[i].copy()

        #根据蚂蚁路径更新信息素表
        for i in range(nPop):
            for k in range(nCity-1):
                Phe[(Ant[i][k],Ant[i][k+1])] = Fit[i] + \
                                               (1 - Rou) * Phe[(Ant[i][k],Ant[i][k+1])]
        Rou = max(Rou * Rou_damp ,Rou_min)

        Data_BestFit.append(Best_Fit)

    return Best_Ant, Best_Fit

# 绘制路径与迭代曲线
def Draw_City(City, X ,Best_Fit):
    X = list(X)
    X.append(X[0])
    coor_x = []
    coor_y = []
    for i in X:
        i = int(i)
        coor_x.append(City[i][0])
        coor_y.append(City[i][1])

    plt.plot(coor_x, coor_y, 'r-o')
    plt.title('TSP with Ant Colony Algorithm\n'+'total_dis = '+str(round(Best_Fit,3)))
    plt.show()

    plt.plot(range(len(Data_BestFit)), Data_BestFit)
    plt.title('Iteration_BestFit')
    plt.xlabel('iteration')
    plt.ylabel('fitness')
    plt.show()


if __name__ == '__main__':
    Best_X, Best_Fit = ACA_TSP()
    Draw_City(City, Best_X, (Best_Fit/nCity)**-1)


