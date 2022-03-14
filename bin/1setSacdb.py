#coding:utf-8
#!/usr/bin/pyenv python

# 提取sac相关信息，写成一个sacdb文件
import readConfig
import os, glob, datetime, re
if __name__ == "__main__":
    myConfig = readConfig.config()
    myConfig.readConfig("Config")
    #print(myConfig.main_dir)
    # 进入数据主目录下，按照file_dir中提供的数据目录依次进入月目录
    os.chdir(myConfig.main_dir)
    with open("file_dir", 'r') as fileF:
        for fileInfo in fileF:
            # 将台站信息复制到月数据目录下，存为station.lst文件
            fileInfo  = fileInfo.strip()
            os.system("cp station.lst.all "+fileInfo+"/station.lst")
            try:
                os.chdir(fileInfo)
                outFile = open("event.dat", 'w')
                # 对fileInfo文件夹下所有"*_*_*_*_*"格式的文件名进行循环
                for days in sorted(glob.glob("*_*_*_*_*"),key=lambda i:int(re.findall(r'(\d+)',i)[2])):  # wkx
                    # 从文件名中读取每条数据的时间，存在event.dat文件中
                    my_date = datetime.datetime.strptime(days, '%Y_%m_%d_%H_%M_%S')
                    my_time = my_date.strftime("%H%M%S")
                    #print(my_time)
                    year ="%4d" % my_date.year
                    month = "   %2d" % my_date.month
                    day = " %2d" % my_date.day
                    outFile.write(year+month+day+" "+my_time+"\n")
                outFile.close()
                # 执行set_sacdb_cut
                os.system(myConfig.code_dir+"/SET_SACDB/set_sacdb_cut")    
                # 返回主目录，准备进入下一个月的数据
                os.chdir("..")
            except Exception:
                break
