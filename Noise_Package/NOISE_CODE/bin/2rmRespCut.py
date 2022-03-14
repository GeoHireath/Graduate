#coding:utf-8
#!/usr/bin/pyenv python

# 去仪器响应，截取相同长度的时间窗
import readConfig
import os, glob, datetime

if __name__ == "__main__":
    myConfig = readConfig.config()
    myConfig.readConfig("Config")
    os.chdir(myConfig.main_dir)
    with open("file_dir", 'r') as fileF:
        for fileInfo in fileF:
            fileInfo  = fileInfo.strip()
            try:
                # 进入月数据目录，根据config设置开始时间和时长
                os.chdir(fileInfo)
                startTime = str(myConfig.cut_start_time)
                cutLength = str(myConfig.cut_length)
                # 执行cut_trans_coda
                os.system(myConfig.code_dir+"/CUT_TRANS/cut_trans_z "+startTime+" "+cutLength)
                os.chdir("..")
            except Exception:
                break
