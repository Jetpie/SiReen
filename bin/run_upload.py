#!/usr/bin/env python
# encoding: utf-8
'''
run_upload - upload computed result to redis


@author:     recsearch

@copyright:  2014 Emar. All rights reserved.

@license:    license

@contact:    recsearch@emar.com

'''
import glob,os,time,logging
import logging.config
# SAFTY FLAG
PID = time.strftime("_%d_%m_%Y")


########################################
# PATH CONFOGURATION
########################################
HOME = '/data/rsworkspace/imagesearch/'
BIN = HOME + 'bin/'
RESULT = HOME + 'res/data/'
CONF = HOME + 'conf/'
# program prefix
PROG = '/usr/bin/python ' + BIN + 'upload_redis.py '


########################################
# LOGGER CONFIGURATION
########################################

logging.config.fileConfig(CONF + 'logging.conf')
# create logger
logger = logging.getLogger('Main@Redis')
logger_result = logging.getLogger("Result@Redis")

logger.info("##############################Start##############################")
start = time.time()

# list all available files
all_files = glob.glob(RESULT + '*.txt')

num_of_files = len(all_files)
for current_in_path in all_files:
    current_in_file = current_in_path.split('/')[-1]

    logger.info('----------Current : %s----------' % current_in_file)
    CMD = PROG + ' -i ' + current_in_path
    logger.debug("%s"%CMD)
    status = os.system(CMD)
    if status==0:
        logger.info("--------------------SUCCEED--------------------")
        logger_result.info("%s"%current_in_file)
    else:
        logger_result.error("%s - FAIL TO UPLOAD"%current_in_file)
        logger.error("-------------------- FAIL  --------------------")
logger.info('Elasped Time for All: %s' % str(time.time() - start))
logger.info("##############################End##############################")
