import time
import random

class Game:
    def __init__(self):
        self.initialize_game()

    def initialize_game(self):
        self.current_state = [['.','.','.','.','.'],
                              ['.','.','.','.','.'],
                              ['.', '.', '.', '.', '.'],
                              ['.', '.', '.', '.', '.'],
                              ['.','.','.','.','.']]

        # 用户永远先手
        self.player_turn = 'X'

    def draw_board(self):
        for i in range(0, 5):
            for j in range(0, 5):
                print('{}|'.format(self.current_state[i][j]), end=" ")
            print()
        print()

    # 判断当前移动是否合法
    def is_valid(self, px, py):
        if px < 0 or px > 4 or py < 0 or py > 4:
            return False
        elif self.current_state[px][py] != '.':
            return False
        else:
            return True

    # 检查棋局是否结束，并返回赢家/平局
    def is_end(self):
        # 竖向
        for i in range(0, 5):
            if (self.current_state[0][i] != '.' and
                    self.current_state[0][i] == self.current_state[1][i] and
                    self.current_state[1][i] == self.current_state[2][i] and
                    self.current_state[2][i] == self.current_state[3][i] and
                    self.current_state[3][i] == self.current_state[4][i]):
                return self.current_state[0][i]

        # 横向
        for i in range(0, 5):
            if (self.current_state[i] == ['X', 'X', 'X', 'X', 'X']):
                return 'X'
            elif (self.current_state[i] == ['O', 'O', 'O', 'O', 'O']):
                return 'O'

        # 左上——右下
        if (self.current_state[0][0] != '.' and
                self.current_state[0][0] == self.current_state[1][1] and
                self.current_state[0][0] == self.current_state[2][2] and
                self.current_state[0][0] == self.current_state[3][3] and
                self.current_state[0][0] == self.current_state[4][4]):
            return self.current_state[0][0]

        # 右上——左下
        if (self.current_state[0][4] != '.' and
                self.current_state[0][4] == self.current_state[1][3] and
                self.current_state[0][4] == self.current_state[2][2] and
                self.current_state[0][4] == self.current_state[3][1] and
                self.current_state[0][4] == self.current_state[4][0]):
            return self.current_state[0][4]

        # 棋盘满了吗
        for i in range(0, 5):
            for j in range(0, 5):
                # There's an empty field, we continue the game
                if (self.current_state[i][j] == '.'):
                    return None

        # 平局
        return '.'

    # AI的评估函数（无剪枝）
    def max(self):
        maxv = -2

        px = None
        py = None

        result = self.is_end()

        if result == 'X':
            return (-1, 0, 0)
        elif result == 'O':
            return (1, 0, 0)
        elif result == '.':
            return (0, 0, 0)

        for i in range(0, 5):
            for j in range(0, 5):
                if self.current_state[i][j] == '.':
                    self.current_state[i][j] = 'O'
                    (m, min_i, min_j) = self.min()
                    if m > maxv:
                        maxv = m
                        px = i
                        py = j
                    self.current_state[i][j] = '.'
        return (maxv, px, py)

    # 人类玩家的评估函数（无剪枝）
    def min(self):

        minv = 2

        qx = None
        qy = None

        result = self.is_end()

        if result == 'X':
            return (-1, 0, 0)
        elif result == 'O':
            return (1, 0, 0)
        elif result == '.':
            return (0, 0, 0)

        for i in range(0, 5):
            for j in range(0, 5):
                if self.current_state[i][j] == '.':
                    self.current_state[i][j] = 'X'
                    (m, max_i, max_j) = self.max()
                    if m < minv:
                        minv = m
                        qx = i
                        qy = j
                    self.current_state[i][j] = '.'

        return (minv, qx, qy)

    def play(self):
        num_drawn = 0
        while True:
            self.draw_board()
            self.result = self.is_end()

            # 打印游戏结果
            if self.result != None:
                if self.result == 'X':
                    print('The winner is X!')
                elif self.result == 'O':
                    print('The winner is O!')
                elif self.result == '.':
                    print("It's a tie!")

                self.initialize_game()
                return

            # 人类回合
            if self.player_turn == 'X':

                while True:
                    if(num_drawn>=16):
                        start = time.time()
                        (m, qx, qy) = self.min()
                        end = time.time()
                        print('Minmax Evaluation time: {}s'.format(round(end - start, 7)))
                        print('Recommended move: X = {}, Y = {}'.format(qx, qy))

                    px = int(input('Insert the X coordinate: '))
                    py = int(input('Insert the Y coordinate: '))

                    (qx, qy) = (px, py)

                    if self.is_valid(px, py):
                        self.current_state[px][py] = 'X'
                        self.player_turn = 'O'
                        num_drawn += 1
                        break
                    else:
                        print('The move is not valid! Try again.')

            # 机器回合
            else:
                if (num_drawn >= 16):
                    (m, px, py) = self.max()
                else:
                    while (True):
                        px = random.randint(0, 4)
                        py = random.randint(0, 4)
                        if (self.current_state[px][py] == '.'):
                            break
                self.current_state[px][py] = 'O'
                self.player_turn = 'X'
                num_drawn += 1
    # 使用alpha-beta剪枝的max
    def max_alpha_beta(self, alpha, beta):
        maxv = -4
        px = None
        py = None
        # print("max 2")

        result = self.is_end()

        if result == 'X':
            return (-1, 0, 0)
        elif result == 'O':
            return (1, 0, 0)
        elif result == '.':
            return (0, 0, 0)

        for i in range(0, 5):
            for j in range(0, 5):
                if self.current_state[i][j] == '.':
                    self.current_state[i][j] = 'O'
                    (m, min_i, in_j) = self.min_alpha_beta(alpha, beta)
                    if m > maxv:
                        maxv = m
                        px = i
                        py = j
                    self.current_state[i][j] = '.'

                    # alpha-beta剪枝作用在这里，在特定深度要求下提前退出无效dfs
                    if maxv >= beta:
                        return (maxv, px, py)

                    if maxv > alpha:
                        alpha = maxv

        return (maxv, px, py)

    # 使用alpha-beta剪枝的min
    def min_alpha_beta(self, alpha, beta):

        minv = 4
        # print("min 1")

        qx = None
        qy = None

        result = self.is_end()

        if result == 'X':
            return (-1, 0, 0)
        elif result == 'O':
            return (1, 0, 0)
        elif result == '.':
            return (0, 0, 0)

        for i in range(0, 5):
            for j in range(0, 5):
                if self.current_state[i][j] == '.':
                    self.current_state[i][j] = 'X'
                    (m, max_i, max_j) = self.max_alpha_beta(alpha, beta)
                    if m < minv:
                        minv = m
                        qx = i
                        qy = j
                    self.current_state[i][j] = '.'
                    # alpha-beta剪枝作用在这里，在特定深度要求下提前退出无效dfs
                    if minv <= alpha:
                        return (minv, qx, qy)

                    if minv < beta:
                        beta = minv

        return (minv, qx, qy)

    def play_alpha_beta(self):
        num_drawn = 0
        while True:
            self.draw_board()
            self.result = self.is_end()

            if self.result != None:
                if self.result == 'X':
                    print('The winner is X!')
                elif self.result == 'O':
                    print('The winner is O!')
                elif self.result == '.':
                    print("It's a tie!")

                self.initialize_game()
                return

            if self.player_turn == 'X':

                while True:
                    if(num_drawn >= 14):
                        start = time.time()
                        (m, qx, qy) = self.min_alpha_beta(-4, 4)
                        end = time.time()
                        print('Alpha-beta Evaluation time: {}s'.format(round(end - start, 7)))
                        print('Recommended move: X = {}, Y = {}'.format(qx, qy))

                    px = int(input('Insert the X coordinate: '))
                    py = int(input('Insert the Y coordinate: '))

                    qx = px
                    qy = py

                    if self.is_valid(px, py):
                        self.current_state[px][py] = 'X'
                        self.player_turn = 'O'
                        num_drawn += 1
                        break
                    else:
                        print('The move is not valid! Try again.')

            else:
                if(num_drawn >= 14 ):
                    (m, px, py) = self.max_alpha_beta(-4, 4)
                else:
                    while(True):
                        px = random.randint(0,4)
                        py = random.randint(0,4)
                        if(self.current_state[px][py]== '.'):
                            break
                self.current_state[px][py] = 'O'
                self.player_turn = 'X'
                num_drawn += 1
def main():
    g = Game()
    g.play() # 使用g.play() 和g.play_alpha_beta()比较剪枝的作用

if __name__ == "__main__":
    main()