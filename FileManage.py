# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""
import os

# Get current working directory
cwd = os.getcwd()

print("Current Directory is {0}".format(cwd))

# Change Directory
os.chdir("/Users/haoyuxu/Desktop/MakeDirectory")

cwd = os.getcwd()

print("Now the directory is {0}".format(cwd))

# Loop and Extract file name and Rename files
for file in os.listdir():
    filesplit = file.split('.')
    print(filesplit[5])
    
    # Extract the date of the files
    test_string = '201301120004'
    # Extract for year
    year = []
    for idx in range(0,4,4):
        year.append(int(test_string[idx:idx+4]))
    
    # Extract for Month
    month = []
    for idx in range(4,6,2):
        month.append(int(test_string[idx:idx+2]))
        
    
    # Extract for Day
    day = []
    for idx in range(6,8,2):
        day.append(int(test_string[idx:idx+2]))
        
    # Extract the rest
    rest = []
    rest.append(int(test_string[8:len(test_string)]))
        
    # Rename files
    New_file_head = filesplit[0]+'.'+filesplit[1]+'.'+filesplit[2]+'.'+filesplit[3]
    New_file_date = str(year[0])+'_'+str(month[0])+'_'+str(day[0])
    New_file_tail = str(rest[0])+'.sac'
    
    New_file_name = New_file_head+'_'+New_file_date+'_'+New_file_tail
    
    print(New_file_name)
    
        
    
    
    
    