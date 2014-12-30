#!/usr/bin/env python
# encoding: utf-8
'''
run_upload - push computed result to redis

@author:     Jiang Hua
@modified by:   Bingqing Qu

@license:    MIT

@contact:    sylar.qu@gmail.com

'''

import sys,os,logging,time,glob
import logging.config
import ConfigParser
from string import Template

# SAFTY FLAG #
PID = time.strftime("_%d_%m_%Y")

########################################
# LOAD CONFIGURATION
########################################
# init config parser
config = ConfigParser.ConfigParser()
config.read(sys.path[0] + "/init.cfg")
# get localhost names
lh_names = config.get('Hostname','local').split(',')
# determine section
section = "Local" if os.uname()[1] in lh_names else "Server"
params = dict(config.items(section))

# min lines of file
MIN_ROW_SIZE = config.getint('Hive','minrow')
########################################
# LOGGER CONFIGURATION
########################################
logging.config.fileConfig(params['logcfg'])
# create logger
logger_hive = logging.getLogger('hive')
logger_fail = logging.getLogger("fail")


with open(params['conf'] + config.get('Hive','categoryfile'),'r') as cate_file:
    categories = ','.join(["'"+cat.strip()+"'" for cat in cate_file])
with open(params['conf'] + config.get('Hive','sqlfile'),'r') as hive_file:
    hive_temp=Template(" ".join([line.strip() for line in hive_file]))

HIVE_QL = hive_temp.safe_substitute({"CATES":categories})

logger_hive.debug("HIVE_QL:")
logger_hive.debug(HIVE_QL)

result_file = params['res'] + "sql" + PID + ".txt"
logger_hive.info("\tStart")

# refresh the result file
open(result_file,"w").close()
os.chmod(result_file,666)
status=os.system("su - hadoop-cdh -c \' hive -S -e \"" + HIVE_QL + \
                 "\" > "+ result_file+"\'")

if status==0:
    logger_hive.info("Write data from Hive into %s" % result_file)
else:
    logger_hive.error("Fail to donwload data from hive")
    logger_fail.error("Fail to donwload data from hive")

lines=os.popen("wc -l "+result_file,'r').readlines()[0].strip()
if lines < MIN_ROW_SIZE:
    logger_hive.error("Number of items download from hive not enough")
    logger_fail.error("Number of items download from hive not enough")
    sys.exit(-1)
logger_hive.info("Number of items download from hive: %s" % lines)

if len(glob.glob(params['sqldata'] + '/*')) > 0:
    logger_hive.error("[sqldata] directory not empty")
    logger_fail.error("[sqldata] directory not empty")
    sys.exit(-1)
with open(result_file,'r') as sql:
    for line in sql:
        cur_cat = line.split('\x01')[0]
        with open(params['sqldata'] + cur_cat + '.txt','a') as out:
            out.write(line)
logger_hive.info("Data splited")
logger_hive.info("\tEnd")
