#coding:utf-8
#!/usr/bin/pyenv python

# 滤波和谱白化
import readConfig
import os, glob

if __name__ == "__main__":
    myConfig = readConfig.config()
    myConfig.readConfig("Config")
    os.chdir(myConfig.main_dir)
    filter_dir = str(myConfig.filter_range[2])+"to"+str(myConfig.filter_range[1])+"_cor"
    with open("file_dir", 'r') as fileF:
        for fileInfo in fileF:
            fileInfo  = fileInfo.strip()
            try:
                # 进入月数据目录，建滤波文件夹，用于存储滤波结果
                os.chdir(fileInfo)
                if not os.path.exists(filter_dir):
                    os.mkdir(filter_dir)
                daysDir = glob.glob("*_*_*_*_*_*")
                # 进入滤波文件夹，把上层文件夹中的文件复制到滤波文件夹，运行信息重定向输出至“warning”文件中
                os.chdir(filter_dir)
                os.system("cp -f ../* ./ &> warning")
                for dayDir in daysDir:
                    # 在滤波文件夹下创建用于存储每天数据的文件夹,复制ft*数据
                    if not os.path.exists(dayDir):
                        os.mkdir(dayDir)
                    os.chdir(dayDir)
                    os.system("cp -r ../../"+dayDir+"/ft_* ./")
                    # 用param.dat文件储存滤波参数(频段和数据文件)
                    outParam = open("param.dat",'w')
                    for sacFile in glob.glob("ft_*"):
                        sacFile.strip()
                        filerRange = list(map(str, myConfig.filter_range))
                        outParam.write(" ".join(filerRange)+" 1 1 "+sacFile+"\n")
                    outParam.close()
                    # 按照param.dat文件中的配置,执行filter4和whiten_phamp
                    os.system(myConfig.code_dir+"/FTA/filter4_f/filter4 param.dat")
                    os.system(myConfig.code_dir+"/FTA/white_outphamp/whiten_phamp param.dat")
                    os.chdir("..")
                os.chdir(myConfig.main_dir)

            except Exception:
                break
