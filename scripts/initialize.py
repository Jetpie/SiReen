#!/usr/bin/env python
# encoding: utf-8
'''
clean_structure - clean the directory structure

@author:     Bingqing Qu

@license:    MIT

@contact:    sylar.qu@gmail.com

'''
import os,glob,time,sys
import ConfigParser
PID = time.strftime("_%d_%m_%Y")

########################################
# LOAD CONFIGURATION
#######################################
# init config parser
config = ConfigParser.ConfigParser()
config.read(sys.path[0] + "/init.cfg")
# get localhost names
lh_names = config.get('Hostname','local').split(',')
# determine section
section = "Local" if os.uname()[1] in lh_names else "Server"
params = dict(config.items(section))
s=0
# Step 1 - pid
if os.path.isfile(params['conf'] + 'pid'):
    os.system('rm -rf '+ params['conf'] +'pid')

s = os.system('touch ' + params['conf'] + 'pid')
if not s == 0 :
    print("pid initialization error!")
    sys.exit(-1)
print('pid reload!')

# Step 2 - sql & sqldata
# sql
os.system('find ' + params['res'] + '*.txt -type f -ctime +1 | xargs rm -f')
# sqldata
if os.path.exists(params['sqldata']):
    os.system('rm -rf ' + params['sqldata'])
s = os.system('mkdir ' + params['sqldata'])

if not s == 0 :
    print("sqldata reload error!")
    sys.exit(-1)
print("sqldata reload!")
# Step 3 - result
if os.path.exists(params['result']):
    os.system('rm -rf ' + params['result'])
s = os.system('mkdir ' + params['result'])
if not s == 0 :
    print("result reload error!")
    sys.exit(-1)
print("result reload!")
# Step 4 - data
if os.path.exists(params['data']):
    os.system('rm -rf ' + params['data'])
s = os.system('mkdir '+  params['data'])
if not s == 0 :
    print("data reload error!")
    sys.exit(-1)
print("data reload!")
