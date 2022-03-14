#coding:utf-8
#!/usr/bin/pyenv python

# 计算信噪比
import readConfig
import os

if __name__ == "__main__":
    myConfig = readConfig.config()
    myConfig.readConfig("Config")
    os.chdir(myConfig.main_dir+"/stack")
    
    # 将需要求信噪比的周期参数写为period文件
    with open("period",'w') as Pfile:
        for p in myConfig.period:
            Pfile.write(str(p)+"\n")
    os.system("cp -f ./period ../")
    
    # 记录文件名，将.dat文件作为参数，执行spectral_snr_f_V2
    os.system("ls *_s > file_s.dat")
    os.system("ls *_n > file_n.dat")
    os.system("ls *_p > file_p.dat")
    os.system(myConfig.code_dir+"/SPECTRAL_SNR/lin_rms/spectral_snr_f_V2 file_s.dat")
    os.system(myConfig.code_dir+"/SPECTRAL_SNR/lin_rms/spectral_snr_f_V2 file_n.dat")
    os.system(myConfig.code_dir+"/SPECTRAL_SNR/lin_rms/spectral_snr_f_V2 file_p.dat")

    # 对每一个周期，执行find_rms_per_w，结果存于SNR_*文件
    with open("period", 'r') as Pfile:
        for per in Pfile:
            try:
                per = per.strip()
                os.system(myConfig.code_dir+"/SPECTRAL_SNR/find_rms_per_v2 "+per+" file_s.dat SNR_"+per+".dat")
            except Exception:
                break
