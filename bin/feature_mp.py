#!/usr/bin/env python
# encoding: utf-8
'''
run_feature_mp - feature extraction multiprocessing

@author: Bingqing Qu

@license: MIT

@contact: sylar.qu@gmail.com

'''
import glob,os,time,logging
import logging.config
from multiprocessing import Pool
import ConfigParser
########################################
# LOAD CONFOGURATION
########################################

# init config parser
config = ConfigParser.ConfigParser()
config.read("init.cfg")
# get localhost names
lh_names = config.get('Hostname','local').split(',')
# determine section
section = "Local" if os.uname()[1] in lh_names else "Server"
params = dict(config.items(section))
# number of processes
PROCESSES = config.getint('Feature','processes')
# feature extraction program
PROG=params['bin']+config.get('Feature','program')
########################################
# LOGGER CONFIGURATION
########################################
logging.config.fileConfig(params['logcfg'])
# create logger
logger_feature = logging.getLogger('feature')
logger_fail = logging.getLogger("fail")


def run_cmd(cat):
    """run c++ program by calling shell"""
    CMD = PROG + '-c ' + cat
    logger.debug("%s"%CMD)
    status = os.system(CMD)
    if status==0:
        logger_feature.info('[%s] Done (Elasped Time: %f)' % \
                           (current_in_file,time.time()-start))
        with open(PID_FILE , 'a') as pid:
            pid.write(cat + '\n')
    else:
        logger_feature.error('[%s] Fail' % cat)
        logger_fail.error('[%s](Search) Fail' % cat)
    return status

if __name__ == '__main__':

    logger_feature.info("\tStart")
    start = time.time()
    # get categories download from hive
    raw_cats =[x.split('/')[-1].split('.')[0] for x in \
               glob.glob(params['sqldata'] + '/*')]
    with open(PID_FILE,'r') as exist:
        exist_cats = [x.strip() for x in exist]
    cats = [x for x in raw_cats if not (x in exist_cats)]
    logger_feature.info("number of categories: %d" % len(cats))

    pool = Pool(PROCESSES)
    processes = pool._pool
    pool.map(run_cmd , cats)

    pool.close()
    time.sleep(1)
    try:
        for worker in processes:
            logger_feature.info('\t%s'%worker)
            assert not worker.is_alive()
        logger_feature.info('\tGarbage Collection Succeeded')
    except AssertionError:
        pool.terminate()
        sys.exit(0)

    logger_feature.info('\tEnd (Elasped Time : %s)\n' % str(time.time() - start))
