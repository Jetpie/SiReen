#!/usr/bin/env python
# encoding: utf-8
'''
run_feature_mp - feature extraction multiprocessing

@author:     recsearch

@copyright:  2014 Emar. All rights reserved.

@license:    license

@contact:    recsearch@emar.com

'''
import glob,os,time,logging
import logging.config
from multiprocessing import Pool

########################################
# PATH CONFOGURATION
########################################
HOME = '/data/rsworkspace/imagesearch/'
BIN = HOME + 'bin/'
CONF = HOME + 'conf/'
RES = HOME + 'res/'
# program prefix
PROG = BIN + 'EDAautorun '
PID_FILE = CONF + 'pid'
########################################
# MP SETTINGS
########################################
# number of process
PROCESSES = 10

########################################
# LOGGER CONFIGURATION
########################################

logging.config.fileConfig(CONF + 'logging.conf')
# create logger
logger = logging.getLogger('Main@Feature')


def run_cmd(cat):

    logger.info('**********Current : %s **********' % cat)
    CMD = PROG + '-c ' + cat
    logger.debug("%s"%CMD)
    status = os.system(CMD)
    if status==0:
        logger.info("[%s] >>>>>>>>>>>>>>>>>>>>SUCCEED<<<<<<<<<<<<<<<<<<<<" % cat)
        with open(PID_FILE , 'a') as pid:
            pid.write(cat + '\n')
    else:
        logger.info("[%s] >>>>>>>>>>>>>>>>>>>>FAIL<<<<<<<<<<<<<<<<<<<<" % cat )
    return status
if __name__ == '__main__':

    logger.info("##############################Start##############################")
    start = time.time()
    ########################################
    # TEMP SETTING FOR CATEGORY
    ########################################
    raw_cats =[x.split('/')[-1].split('.')[0] for x in glob.glob(RES + 'sqldata/*.txt')]
    with open(PID_FILE,'r') as exist:
        exist_cats = [x.strip() for x in exist]
    cats = [x for x in raw_cats if not (x in exist_cats)]

    print "num of categories:",len(cats)

    pool = Pool(PROCESSES)
    processes = pool._pool
    pool.map(run_cmd , cats)

    pool.close()
    time.sleep(1)
    try:
        for worker in processes:
            print worker
            assert not worker.is_alive()
        logger.info('\tGarbage Collection Succeeded\n')
    except AssertionError:
        pool.terminate()
    logger.info('Elasped Time for All: %s' % str(time.time() - start))
    logger.info("##############################End##############################")
