#coding:utf-8
#!/usr/bin/pyenv python

# 该程序用于检查并读取配置文件，输出参数信息或错误信息
import sys, os
import readConfig
def check():
    # 检查是否有配置文件
    if not os.path.exists("Config"):
        print("Config File not exists !!!!!")
    # 读参数
    config = readConfig.config()
    config.readConfig("Config")
    # 检查参数，如果无误则打印设置信息以供验看
    if not config.checkConfig():
        print("Error: Config File has some problem!!!")
        return False
    else:
        print("----------------------------------------")
        print("| Config has been setted sucessfully!!!")
        print("----------------------------------------")
        print("| Main Directory:")
        print("| \t"+config.main_dir)
        print("| Code Directory:")
        print("| \t"+config.code_dir)
        print("| Cut Time Begin : "+str(config.cut_start_time))
        print("| Length of Time Window : "+str(config.cut_length))
        filter_range = list(map(str, config.filter_range))
        #print(filter_range)
        str_filter = " ".join(filter_range)
        print("| Filter Range : "+str_filter)
        print("| Cross Correlation Length : "+str(config.correlation_length))
        print("| Day Thresold : "+str(config.threshold_days))

        print("| minimum group velocity : "+str(config.group_velocity_min))
        print("| maximum group velocity : "+str(config.group_velocity_max))
        print("| minimum time period : "+str(config.period_min))
        print("| maximum time period : "+str(config.period_max))    # jxy
        
        period_list = list(map(str, config.period))
        str_period = " ".join(period_list)
        print("| Desired Period : "+str_period) # wkx
        
        print("-----------------------------------------")
    # 检查所需目录和数据是否存在
    if not os.path.exists(config.main_dir):
        print("Error : data directory not exists!!!")
        return False
    if not os.path.exists(config.code_dir):
        print("Error : code directory not exists!!!")
        return False
    if not os.path.exists(config.main_dir+"/station.lst.all"):
        print("Error : station list file not exist!!!")
        return False
    
    if not os.path.exists(config.main_dir+"/model"):
        print("Error : model directory not exists!!!")
        return False    # wkx

    if not os.path.exists(config.main_dir+"/file_dir"):
        print("Error : file list not exists!!!!")
        return False    # wkx
    else:
        print("-------------------------------------")
        print("| Files check sucessfully !!!")
        print("-------------------------------------")

#直接运行0check.py时，调用check函数，外部调用0check时，不进行此操作
if __name__ == "__main__":
    check()
