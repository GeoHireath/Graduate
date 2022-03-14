#!/usr/bin/pyenv python
# -*- coding:utf-8 -*-
#计算频散曲线

#运行前确保已经修改好README中提到的三处路径
#并在aftani_c_pgl文件夹下make clean + make

import readConfig
import os, glob
if __name__ == "__main__":
	myConfig = readConfig.config()
	myConfig.readConfig("Config")
	os.chdir(myConfig.main_dir) #进入数据文件夹
	dir_disp = "GLOBAL_DISP01" #创建存放计算文件的文件夹，命名为GLOBAL_DISP**
	if not os.path.exists(dir_disp): 
		os.mkdir(dir_disp)
	os.system("cp ./model/* ./" + dir_disp) #准备文件夹中所需文件
	os.system("cp period ./" + dir_disp)
	os.system("cp station.lst.all ./" + dir_disp + "/station.lst")
	os.system("cp " + myConfig.code_dir + "/DISP_CURVE/disp_curve ./"  + dir_disp)
	os.system("cp " + myConfig.code_dir + "/DISP_CURVE/plot_CC_dist.gmt ./stack/")
	os.chdir(dir_disp) 
	os.system("chmod -R 755 disp_curve")
	os.system("./disp_curve")
	os.chdir("../stack")

	outFile = open("param.dat", 'w') #创建参数记录文件
	for files in sorted(glob.glob("*_s")): #对于所有SAC_s文件循环
		outFile.write("-1 "+str(myConfig.group_velocity_min)+" "+str(myConfig.group_velocity_max)+" "+str(myConfig.period_min)+" "+str(myConfig.period_max)+" 20 1 0.5 0.2 2 "+files+"\n")
	outFile.close()

	os.system(myConfig.code_dir+"/FTA/aftani_c_pgl/aftani_c_pgl param.dat ") #运行相应源程序
