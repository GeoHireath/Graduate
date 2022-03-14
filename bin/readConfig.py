#coding:utf-8
#!/usr/bin/pyenv python
# 读参数并检查是否正确

# 定义类
class config:

    # 初始化(包含的参数)
    def __init__(self):
        self.main_dir = ''
        self.code_dir = ''
        self.cut_start_time = 0
        self.cut_length = 0
        self.filter_range = []
        self.correlation_length = 0
        self.threshold_days  = 0
        
        self.group_velocity_min = 0
        self.group_velocity_max = 0
        self.period_min = 0
        self.period_max = 0 # jxy
        
        self.label = True

        self.period = []    # wkx

    # 从文件里读参数
    def readConfig(self, file):
        with open(file, 'r') as conFile:
            for s in conFile:
                s = s.strip()   # 删除首尾的空白符
                if len(s) == 0:   continue
                elif s[0] == '#': continue
                elif s[:8] == "main_dir":
                    s =s.replace("=", " ")
                    self.main_dir = s.split()[1]    
                    # 配置文件中目录没有加引号所以不能直接执行
                elif s[:8] == "code_dir":
                    s = s.replace("=", " ")
                    self.code_dir = s.split()[1]
                else:             exec("self."+s)

    # 检查参数是否都有值，返回是否正确
    def checkConfig(self):
        if len(self.main_dir) == 0:
            print("Error: Not Set Main Directory!!!!!")
            label = False
        if len(self.code_dir) == 0:
            print("Error: Not Set Code Directory!!!!!")
            label = False
        if self.cut_start_time <= 0:
            print("Warning: Cutting time window start from "+str(self.cut_start_time)+ " second!!!")
        if self.cut_length <= 0:
            self.label = False
            print("Error: Length of time window is not correct!!!")
        if len(self.filter_range) != 4:
            self.label = False
            print("Error: Format of filter range is not correct!!!")
        else:
            for i in range(0, 3):
                if self.filter_range[i] <= self.filter_range[i + 1]:
                    print("Error: Filter range is not corrected !!!!")
                    self.label = False
                    break
        if self.correlation_length <= 0:
            self.label = False
            print("Error: Length of Cross Correlation is less than zero !!!")

        if self.threshold_days <= 0:
            print("Warning: The threshold days is "+str(self.threshold_days)+"!!!")
        
        if self.group_velocity_min <= 0:
            print("Warning: The minimum group velocity is "+str(self.group_velocity_min)+"!!!")
        if self.group_velocity_max <= self.group_velocity_min:
            print("Warning: The minimum and maxmimum group velocity are "+str(self.group_velocity_min)+" "+str(self.group_velocity_max)+"!!!")
        if self.period_min <= 0:
            print("Warning: The minimum time period is "+str(self.period_min)+"!!!")
        if self.period_max <= self.period_min:
            print("Warning: The minimum and maxmum time period are "+str(self.period_min)+" "+str(self.period_max)+"!!!")   # jxy

        if len(self.period) < 1:
            self.lable = False
            print("Error: Not Set Desired Period!!!!!") # wkx
        return self.label

