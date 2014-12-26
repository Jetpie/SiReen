#!/usr/bin/env python
# encoding: utf-8
'''
run_compute_mp - muilti-processing nearest neighbor searching

@author:  Bingqing Qu

@license: MIT

@contact: sylar.qu@gmail.com

'''
import glob,os,time,logging,sys
import logging.config
from multiprocessing import Pool
import ConfigParser

########################################
# LOAD CONFIGURATION
#######################################

# init config parser
config = ConfigParser.ConfigParser()
config.read("init.cfg")
# get localhost names
lh_names = config.get('Hostname','local').split(',')
# determine section
section = "Local" if os.uname()[1] in lh_names else "Server"
params = dict(config.items(section))
# add environment variable
sys.path.append(params['lib'])
from nenese.nearest_neighbour import KNearestSearch
# number of processes
PROCESSES = config.getint('Search','processes')

########################################
# LOGGER CONFIGURATION
########################################
logging.config.fileConfig(params['logcfg'])
# create logger
logger_search = logging.getLogger('search')
logger_fail = logging.getLogger("fail")

def search(current_in_path):
    #  split to get current file
    current_in_file = current_in_path.split('/')[-1]
    try:
        logger_detail = logging.getLogger("detail")
        kns = KNearestSearch(logger = logger_detail,delimiter = "\t",use_sparse = True)
        kns.process(in_file = current_in_path,
                    out_file = params["result"] + current_in_file,
                    max_chunk_size = 2000)
        logger_search.info('[%s] Done' % current_in_file)
        return 0
    except:
        logger_search.error('[%s] Fail' % current_in_file)
        logger_fail.error('[%s](Search) Fail' % current_in_file)
        return -1

if __name__ == '__main__':

    logger_search.info("\tStart")
    start = time.time()

    # list all available files
    all_files = glob.glob(params['data'] + '*.txt')
    pool = Pool(PROCESSES)
    processes = pool._pool
    pool.map(search , all_files)

    pool.close()
    time.sleep(1)
    try:
        for worker in processes:
            logger_search.info('\t%s'%worker)
            assert not worker.is_alive()
        logger_search.info('\tGarbage Collection Succeeded')
    except AssertionError:
        pool.terminate()
        sys.exit(0)

    logger_search.info('\tEnd (Elasped Time : %s)\n' % str(time.time() - start))
