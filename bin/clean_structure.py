#!/usr/bin/env python
# encoding: utf-8
'''
clean_structure - clean the directory structure

@author:     recsearch

@copyright:  2014 Emar. All rights reserved.

@license:    license

@contact:    recsearch@emar.com

'''
import os,glob,time,sys
PID = time.strftime("_%d_%m_%Y")
# verify=raw_input("Please enter [yes] to continue...")
# if not verify == 'yes':
#     print "EXIT!"
#     sys.exit(-1)
########################################
# PATH CONFOGURATION
########################################
HOME = '/data/rsworkspace/imagesearch/'
BIN = HOME + 'bin/'
CONF = HOME + 'conf/'
RES = HOME + 'res/'
s=0
# Step 1 - pid
if os.path.isfile(CONF + 'pid'):
    os.system('rm -rf '+CONF+'pid')

s = os.system('touch ' + CONF + 'pid')

if not s == 0 :
    print "pid initialization error!"
    sys.exit(-1)
print 'pid reload!'

# Step 2 - sql & sqldata
# sql
os.system('find ' + RES + '*.txt -type f -ctime +1 | xargs rm -f')
# sqldata
if os.path.exists(RES + 'sqldata/'):
    os.system('rm -rf ' + RES + 'sqldata/')
s = os.system('mkdir ' + RES + 'sqldata')

if not s == 0 :
    print "sqldata reload error!"
    sys.exit(-1)
print "sqldata reload!"
# Step 3 - result
if os.path.exists(RES + 'result/'):
    os.system('rm -rf ' + RES + 'result/')
s = os.system('mkdir ' + RES + 'result')
if not s == 0 :
    print "result reload error!"
    sys.exit(-1)
print "result reload!"
# Step 4 - data
if os.path.exists(RES + 'data/'):
    os.system('rm -rf ' + RES + 'data/')
s = os.system('mkdir ' + RES + 'data')
if not s == 0 :
    print "data reload error!"
    sys.exit(-1)
print "data reload!"
