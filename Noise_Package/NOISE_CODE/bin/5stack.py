#coding:utf-8
#!/usr/bin/pyenv python

# 将同一台站对的互相关序列进行叠加
import readConfig
import os, glob

if __name__ == "__main__":
    myConfig = readConfig.config()
    myConfig.readConfig("Config")
    os.chdir(myConfig.main_dir)
    filter_dir = str(myConfig.filter_range[2])+"to"+str(myConfig.filter_range[1])+"_cor"
    # 创建存放脚本等的文件夹
    if not os.path.exists("listfile"):
        os.mkdir("listfile")
    with open("file_dir", 'r') as fileF:
        for fileInfo in fileF:
            try:
                # 进入当月的互相关文件夹（月文件夹/滤波文件夹/互相关文件夹）
                fileInfo  = fileInfo.strip()
                os.chdir(fileInfo)
                os.chdir(filter_dir)
                os.chdir("COR")
                path = myConfig.main_dir+"/"+fileInfo+"/"+filter_dir+"/COR/"
                
                ccFiles = glob.glob("COR*.SAC")
                # 回到滤波文件夹
                os.chdir("..")
                os.system("cp ../event_station.tbl ./")
                # 创建以月命名的文件，记录互相关结果文件名
                outF1 = open(fileInfo, "w")
                for ccFile in ccFiles:
                    ccFile.strip()
                    print(ccFile)
                    outF1.write(ccFile+"\n")
                outF1.close()
                # 执行getdays_xia，输出台站信息和得到互相关所用天数
                os.system(myConfig.code_dir+"/STACK/getdays_xia "+fileInfo+" event_station.tbl")
                
                # 创建temp文件记录路径和互相关结果文件名
                outF3 = open('temp', 'w')
                outF3.write(path+"\n")
                with open(fileInfo+".lst", 'r') as infos:
                    for info in infos:
                        info = info.strip()
                        outF3.write(info+"\n")
                outF3.close()

                os.system("mv temp "+myConfig.main_dir+"/listfile/"+fileInfo+".lst")
                os.system("rm -rf "+fileInfo+" "+fileInfo+".lst")
                os.chdir(myConfig.main_dir)
            except Exception:
                break
    os.chdir(myConfig.main_dir)
    
    # 创建存放叠加后数据的文件夹stack
    if not os.path.exists("stack"):
        os.mkdir("stack")
    stackPath = myConfig.main_dir+"/stack/"
    os.chdir("listfile")
    if os.path.exists("all.list"):
        os.system("rm -rf all.list")
    os.system("ls *.lst > all.list")
    if not os.path.exists("scripts"):
        os.mkdir("scripts")
    # 执行ymkstack_xia
    os.system(myConfig.code_dir+"/STACK/ymkstack_xia all.list "+stackPath+" "+str(myConfig.threshold_days))
