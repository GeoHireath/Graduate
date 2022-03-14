#coding:utf-8
#!/usr/bin/pyenv python

# 进行互相关处理
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
                # 进入月数据下的滤波文件夹,复制储存sac信息的.out文件(第1步生成)
                os.chdir(fileInfo)
                os.chdir(filter_dir)
                os.system('cp -f ../sac_db.out ./')
                # 创建互相关文件夹
                if not os.path.exists("COR"):
                    os.mkdir("COR")
                # 执行justCOR
                os.system(myConfig.code_dir+"/NEWCORR/justCOR "+str(myConfig.correlation_length))
                os.chdir(myConfig.main_dir)
            except Exception:
                break
