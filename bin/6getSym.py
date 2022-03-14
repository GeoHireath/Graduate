#coding:utf-8
#!/usr/bin/pyenv python

# 获得因果和非因果信号对称叠加结果
import readConfig
import os, glob

if __name__ == "__main__":
    myConfig = readConfig.config()
    myConfig.readConfig("Config")
    os.chdir(myConfig.main_dir)
    os.chdir("stack")
    os.system("ls *.SAC > filelist")
    # 执行yangch
    os.system(myConfig.code_dir+"/CH/yangch")
